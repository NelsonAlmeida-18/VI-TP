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

    if (!f.bb.intersect(r)) return false;

    // Lets check if the ray is parallel to the triangle
    // The ray is parallel to the triangle if the dot product of the ray direction and the 
    // triangle normal is close to zero
    // We will consider epsilon to be the baseline for this comparison
    if (std::abs(r.dir.dot(f.geoNormal)) < epsilon) return false;

    //TODO: Rever isto
    Point v0v1Aux = vertices[f.vert_ndx[1]] - vertices[f.vert_ndx[0]];
    Vector v0v1(v0v1Aux.X, v0v1Aux.Y, v0v1Aux.Z);
    cout << "v0v1: " << v0v1.X << " " << v0v1.Y << " " << v0v1.Z << endl;
    Point v0v2Aux = vertices[f.vert_ndx[2]] - vertices[f.vert_ndx[0]];
    Vector v0v2(v0v2Aux.X, v0v2Aux.Y, v0v2Aux.Z);
    Vector pvec = r.dir.cross(v0v2);
    float det = v0v1.dot(pvec);

    float invDet = 1.0 / det;
    auto aux = r.o - vertices[f.vert_ndx[0]];
    Vector tvec(aux.X, aux.Y, aux.Z);
    float u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;

    Vector qvec = tvec.cross(v0v1);
    float v = r.dir.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qvec) * invDet;

    return false;
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = false;
    
    //bool intersect = true, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth=MAXFLOAT;
    // intersect the ray with the mesh BB
    // cout << "Intersecting mesh with BB: " << bb.intersect(r) << endl;
    if (!bb.intersect(r)) return false;
    
    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        //intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
        if (! TriangleIntersect(r, *face_it, &curr_isect)) continue;
        
        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }
    
    return intersect;
}
