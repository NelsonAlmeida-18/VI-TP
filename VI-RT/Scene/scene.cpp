//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "primitive.hpp"
#include "mesh.hpp"
#include "Phong.hpp"
#include "vector.hpp"

#include <iostream>
#include <set>
#include <stdio.h>
#include <unordered_map>

using namespace tinyobj;

static void PrintInfo (const ObjReader myObj) {
    const tinyobj::attrib_t attrib = myObj.GetAttrib();
    const std::vector<tinyobj::shape_t> shapes = myObj.GetShapes();
    const std::vector<tinyobj::material_t> materials = myObj.GetMaterials();
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
              << std::endl;

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    
    // Iterate shapes
    auto it_shape = shapes.begin();
    for ( ; it_shape != shapes.end() ; it_shape++) {
        // assume each face has 3 vertices
        std::cout << "Processing shape " << it_shape->name << std::endl;
        // iterate faces
        // assume each face has 3 vertices
        auto it_vertex = it_shape->mesh.indices.begin();
        for ( ; it_vertex != it_shape->mesh.indices.end() ; ) {
            // process 3 vertices
            for (int v=0 ; v<3 ; v++) {
                std::cout << it_vertex->vertex_index;
                it_vertex++;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        
        printf("There are %lu material indexes\n", it_shape->mesh.material_ids.size());
    }
    
}

/*
 Use tiny load to load .obj scene descriptions
 https://github.com/tinyobjloader/tinyobjloader
 */

bool Scene::Load (const std::string &fname) {
    
    //PrintInfo (myObjReader);
    // convert loader's representation to my representation
    //Verify if the file passed is a .obj or a .mtl file
    if (fname.find(".obj") != std::string::npos) {
        // Load the .obj file
        if (!LoadObj(fname)) {
            return false;
        }
    }
    else {
        return false;
    }
    
    // your code here
    return true;
}



bool Scene::LoadObj(const std::string& fname) {

    tinyobj::ObjReader reader;
    int FaceID=0; 

    if (!reader.ParseFromFile(fname)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return false;
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();


    // Each shape is one mesh
    // Lets iterate over shapes/meshes
    for (auto shape : shapes) {

        auto mat = &materials[shape.mesh.material_ids[0]];
        Phong *phong = new Phong();
        // Ambient component
        phong->Ka.R = mat->ambient[0];
        phong->Ka.G = mat->ambient[1];
        phong->Ka.B = mat->ambient[2];
        // Diffuse component
        phong->Kd.R = mat->diffuse[0];
        phong->Kd.G = mat->diffuse[1];
        phong->Kd.B = mat->diffuse[2];
        // Specular component
        phong->Ks.R = mat->specular[0];
        phong->Ks.G = mat->specular[1];
        phong->Ks.B = mat->specular[2];
        // Transmittance
        phong->Kt.R = mat->transmittance[0];
        phong->Kt.G = mat->transmittance[1];
        phong->Kt.B = mat->transmittance[2];
        // Shininess 
        phong->Ns = mat->shininess;


        BRDFs.push_back(phong);
        numBRDFs++;
        

        Mesh *mesh = new Mesh();
        Primitive *prim = new Primitive();
        prim->g = mesh;
        // Lets assume all faces in the mesh have the same material
        prim->material_ndx = shape.mesh.material_ids[0];

        // The primitives geomtry bounding box is calculated on the fly
        const int V1st = shape.mesh.indices[0].vertex_index;
        mesh->bb.min.set(attrib.vertices[V1st], attrib.vertices[V1st+1], attrib.vertices[V1st+2]);
        mesh->bb.max.set(attrib.vertices[V1st], attrib.vertices[V1st+1], attrib.vertices[V1st+2]);



        // Lets loop over all the faces and vertices
        std::unordered_map<int, Point> vertices_rehash;
        int indexOffset=0;
        for (size_t faceNum = 0; faceNum<shape.mesh.num_face_vertices.size(); faceNum++){
            size_t numVerticesPerFace = size_t(shape.mesh.num_face_vertices[faceNum]);
            
            Face *f = new Face();
            Point myVtcs[3];

            // Lets get the vertices in the face
            for (size_t vertexNum=0; vertexNum<numVerticesPerFace; vertexNum++) {
                auto index = shape.mesh.indices[indexOffset+vertexNum];
                
                auto vx = attrib.vertices[3*index.vertex_index+0];
                auto vy = attrib.vertices[3*index.vertex_index+1];
                auto vz = attrib.vertices[3*index.vertex_index+2];

                Point newPoint(vx, vy, vz);
                myVtcs[vertexNum] = newPoint;
                f->vert_ndx[vertexNum] = mesh->vertices.size();
                f->bb.update(newPoint);

                // Lets add the face to vertex
                // If the vertex is new then add it to the mesh
                auto known_vert = vertices_rehash.find(index.vertex_index);

                if (known_vert == vertices_rehash.end()) {
                    // New vertex, add it to the mesh
                    vertices_rehash.insert(std::make_pair(index.vertex_index, newPoint));
                    mesh->vertices.push_back(newPoint);
                    mesh->numVertices++;
                } else {
                    // Vertex already exists
                    f->vert_ndx[vertexNum] = std::distance(vertices_rehash.begin(), known_vert);
                }
            }

            // add face to mesh and compute the geometric normal
            Vector v1 = myVtcs[0].vec2point(myVtcs[1]);
            Vector v2 = myVtcs[0].vec2point(myVtcs[2]);

            Vector normal = v1.cross(v2);
            normal.normalize();
            f->geoNormal.set(normal);
            f->FaceID = FaceID++;

            mesh->faces.push_back(*f);
            mesh->numFaces++;

    
            // Add primitives to the scene
            prims.push_back(prim);
            numPrimitives++;

            std::cout << "Face " << faceNum << " has " << numVerticesPerFace << " vertices" << std::endl;
            indexOffset += numVerticesPerFace;

        }

        // for (auto index : shape.mesh.indices) {
            
        //     Face *f = new Face();
        //     Point myVtcs[3];

        //     // We will process 3 vertices at a time
        //     for (int i=0; i<3; i++) {
        //         const int Vndx = index.vertex_index;
        //         myVtcs[i].set(attrib.vertices[Vndx*3], attrib.vertices[Vndx*3+1], attrib.vertices[Vndx*3+2]);
        //         f->vert_ndx[i] = mesh->vertices.size();
                
        //         if (i==0){
        //             f->bb.min.set(myVtcs[0].X, myVtcs[0].Y, myVtcs[0].Z);
        //             f->bb.max.set(myVtcs[0].X, myVtcs[0].Y, myVtcs[0].Z);
        //         }else{
        //             f->bb.update(myVtcs[i]);
        //         }
        //         // Lets add the face to vertex
        //         // If the vertex is new then add it to the mesh
        //         // auto known_vert = vertices_rehash.find(myVtcs[i]);

                
        //         auto known_vert = vertices_rehash.find(Vndx);

        //         if (known_vert == vertices_rehash.end()) {
        //             // TODO: Rever isto
        //             auto newVert = std::make_pair(Vndx, myVtcs[i]);
        //             // New vertex, add it to the mesh
        //             vertices_rehash.insert(newVert);

        //             mesh->vertices.push_back(myVtcs[i]);
        //             mesh->numVertices++;
        //             f->vert_ndx[i] = mesh->numVertices;
        //             mesh->bb.update(myVtcs[i]);
        //         } else {
        //             // Vertex already exists
        //             f->vert_ndx[i] = std::distance(vertices_rehash.begin(), known_vert);
        //         }
        //     }
        //     // add face to mesh and compute the geometric normal
        //     Vector v1 = myVtcs[0].vec2point(myVtcs[1]);
        //     Vector v2 = myVtcs[0].vec2point(myVtcs[2]);
            
        //     // TODO: Rever isto
        //     // edge1 e edge2 estão na classe triangle.hpp, provavelmente temos de criar um triângulo com os 3 pontos que vamos buscar
        //     // f->edge1 = v1;
        //     // f->edge2 = v2;

        //     Vector normal = v1.cross(v2);
        //     normal.normalize();
        //     f->geoNormal.set(normal);
        //     f->FaceID = FaceID++;

        //     mesh->faces.push_back(*f);
        //     mesh->numFaces++;

        //     std::cout << "Index " << index.vertex_index << std::endl;
 
        // }
        // // Add primitives to the scene
        // prims.push_back(prim);
        // numPrimitives++;


        // // FOr each mesh we will debug the number of vertices and faces
        // std::cout << "Mesh " << shape.name << " has " << mesh->numVertices << " vertices and " << mesh->faces.size() << " faces" << std::endl;
        // // For each mesh we will print the position of its vertices
        // for (int i=0 ; i<mesh->numVertices ; i++) {
        
        //     std::cout << "Vertex " << i << " : " << mesh->vertices[i].X << " " << mesh->vertices[i].Y << " " << mesh->vertices[i].Z << std::endl;
        // }

    }


    return true;
}

bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    if (numPrimitives==0) return false;
    //std::cout << "Tracing primitives: " << numPrimitives << std::endl;
    // iterate over all primitives
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() ; prim_itr++) {
        if ((*prim_itr)->g->intersect(r, &curr_isect)) {
            if (!intersection) { // first intersection
                intersection = true;
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
                
            }
            else if (curr_isect.depth < isect->depth) {
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
        }
    }

    if (intersection) std::cout << "Intersected" << std::endl;


    return intersection;
}

// checks whether a point on a light source (distance maxL) is visible
bool Scene::visibility (Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;
    
    if (numPrimitives==0) return true;
    
    // iterate over all primitives while visible
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() && visible ; prim_itr++) {
        if ((*prim_itr)->g->intersect(s, &curr_isect)) {
            if (curr_isect.depth < maxL) {
                visible = false;
            }
        }
    }
    return visible;
}
