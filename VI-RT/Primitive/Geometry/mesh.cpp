//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"
#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

using namespace std;
// see pbrt book (3rd ed.), sec 3.6.2, pag 157
//
// Suggestion: use:
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm


bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {

    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    // Lets check if the ray is parallel to the triangle
    // The ray is parallel to the triangle if the dot product of the ray direction and the 
    // triangle normal is close to zero
    // We will consider epsilon to be the baseline for this comparison
    // if (std::abs(r.dir.dot(f.geoNormal)) < epsilon) return false;

    
    Point v0v1Aux = vertices[f.vert_ndx[1]] - vertices[f.vert_ndx[0]];
    Vector e1(v0v1Aux.X, v0v1Aux.Y, v0v1Aux.Z);
    

    Point v0v2Aux = vertices[f.vert_ndx[2]] - vertices[f.vert_ndx[0]];
    Vector e2(v0v2Aux.X, v0v2Aux.Y, v0v2Aux.Z);
    
    Vector ray_cross_e2 = r.dir.cross(e2);
    float det = e1.dot(ray_cross_e2);

    // if the determinant is close to zero, the ray is parallel to the triangle
    if (det > -epsilon && det < epsilon) return false;


    float invDet = 1.0 / det;
    Point aux = r.o - vertices[f.vert_ndx[0]];
    Vector s(aux.X, aux.Y, aux.Z);
    float u = s.dot(ray_cross_e2) * invDet;
    if (u < 0.0 || u > 1.0) return false;
    

    Vector s_cross_e1 = s.cross(e1);

    float dot = r.dir.dot(s_cross_e1);
    float v = dot * invDet;

    if (v < 0 || u + v > 1) return false;
    float t = e2.dot(s_cross_e1) * invDet;


    if (t>epsilon) {
        isect->depth = t;
        isect->p = r.o + t*r.dir;

        // compute the normal
        Vector normal = f.geoNormal;
        Vector wo = -1.0 * r.dir;
        normal.Faceforward(wo);

        isect->gn = normal;
        isect->sn = normal; 

        isect->wo = -1.0 * r.dir;
        isect->FaceID = f.FaceID;
        return true;
    }
    

    // float invDet = 1.0 / det;
    // auto aux = r.o - vertices[f.vert_ndx[0]];
    // Vector tvec(aux.X, aux.Y, aux.Z);
    // float u = tvec.dot(pvec) * invDet;
    // if (u < 0 || u > 1) return false;

    // Vector qvec = tvec.cross(v0v1);
    // float v = r.dir.dot(qvec) * invDet;
    // if (v < 0 || u + v > 1) return false;
    // float t = v0v2.dot(qvec) * invDet;
    
    return false;
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = false;
    
    //bool intersect = true, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth=MAXFLOAT;

    // intersect the ray with the mesh BB
    bool bbIntersectionResult = bb.intersect(r);

    // cout << "BB intersection result: " << bbIntersectionResult << endl;
    
    if (!bbIntersectionResult) return false;
    
    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        
        // Lets see if the ray intersects the face bounding box, if not continue
        if(face_it->bb.intersect(r)) {
            

            //intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
            if (TriangleIntersect(r, *face_it, isect)){
                // std::cout << "Mesh intersect: Some triangle intersects " << face_it->FaceID << " intersects the ray" << std::endl;

                intersect = true;
                if (isect->depth < min_depth) {  // this is closer
                    min_depth = isect->depth;
                    min_isect = *isect;
                }
            }
            
        }
    }
    *isect = min_isect;
    return intersect;
}
