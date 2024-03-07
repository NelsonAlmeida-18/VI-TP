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

#include <iostream>
#include <set>
#include <vector>
#include <regex>

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
    else if (fname.find(".mtl") != std::string::npos) {
        // Load the .mtl file
        if (!LoadMtl(fname)) {
            return false;
        }
    }
    else {
        return false;
    }
    
    // your code here
    return true;
}

bool LoadObj(const std::string fname) {
    ObjReader myObjReader;
    if (!myObjReader.ParseFromFile(fname)) 
        return false;
    
    const tinyobj::attrib_t attrib = myObjReader.GetAttrib();
    // Obj structure:
    // List of geometric vertices with x,y,z coordinates
    // v x y z
    // List of texture coordinates with u,[v,w] in [0 ... 1]
    //vt u [v[w]]
    // List of vertex normals with x,y,z coordinates
    //vn x y z
    // Which materials definition file to use
    //mtllib [filename]
    //wich material to use in the subsequent objets
    //usemtl [material_name]
    // group subsequent faces onto an object
    //o [object_name]
    //Polygonal face element f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
    //arguments are the vertices indices according to their order in the file
    // f 1 2 4
    // f 3/1 4/2 5/3
    // f 6//3 2//1 7//2

    //Lets start by identifying the regex of the file structure
    std::regex vertex_pattern(R"(v\s+([\d\.-]+)\s+([\d\.-]+)\s+([\d\.-]+))");
    std::regex texture_pattern(R"(vt\s+([\d\.-]+)\s+([\d\.-]+)\s*([\d\.-]+)?)");
    std::regex normal_pattern(R"(vn\s+([\d\.-]+)\s+([\d\.-]+)\s+([\d\.-]+))");
    std::regex material_pattern(R"(mtllib\s+([\w\.]+))");
    std::regex usemtl_pattern(R"(usemtl\s+([\w]+))");
    std::regex object_pattern(R"(o\s+([\w]+))");
    std::regex face_pattern(R"(f\s+([\d]+)(?:/([\d]+))?(?:/([\d]+))?(\s+([\d]+)(?:/([\d]+))?(?:/([\d]+))?)(?:\s+([\d]+)(?:/([\d]+))?(?:/([\d]+))?)?)");

    std::ifstream file(fname);
    if (file.is_open()){
        std::string line;
        while (std::getline(file, line)){
            std::smatch match;
            if (std::regex_match(line, match, vertex_pattern)){
                //std::cout << "Vertex: " << match[1] << " " << match[2] << " " << match[3] << std::endl;
            }
            else if (std::regex_match(line, match, texture_pattern)){
                //std::cout << "Texture: " << match[1] << " " << match[2] << " " << match[3] << std::endl;
            }
            else if (std::regex_match(line, match, normal_pattern)){
                //std::cout << "Normal: " << match[1] << " " << match[2] << " " << match[3] << std::endl;
            }
            else if (std::regex_match(line, match, material_pattern)){
                //std::cout << "Material: " << match[1] << std::endl;
            }
            else if (std::regex_match(line, match, usemtl_pattern)){
                //std::cout << "Usemtl: " << match[1] << std::endl;
            }
            else if (std::regex_match(line, match, object_pattern)){
                //std::cout << "Object: " << match[1] << std::endl;
            }
            else if (std::regex_match(line, match, face_pattern)){
                //std::cout << "Face: " << match[1] << " " << match[2] << " " << match[3] << std::endl;
            }
        }
    }
    else {
        std::cerr << "Error: Could not open file " << fname << std::endl;
        return false;
    }

    return true;
}

bool LoadMtl(const std::string fname) {
    ObjReader myObjReader;
    if (!myObjReader.ParseFromFile(fname)) 
        return false;
    
    const tinyobj::attrib_t attrib = myObjReader.GetAttrib();
    

    return true;
}

bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    if (numPrimitives==0) return false;
    
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
