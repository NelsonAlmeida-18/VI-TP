//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"
#include <vector>
#include <limits>

// see pbrt book (3rd ed.), sec 3.6.2, pag 157
//
// Suggestion: use:
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm

bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {
    
    if (!f.bb.intersect(r)) return false;


    // Point a = f.vert_ndx[0];
    // Point b = f.vert_ndx[1];
    // Point c = f.vert_ndx[2];
    
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    float triangle_x = isect->p.X;
    float triangle_y = isect->p.Y;
    float triangle_z = isect->p.Z;
    
    float edgea = triangle_y-triangle_x;
    float edgeb = triangle_z-triangle_x;
    
    // Vector ray_cross_e2 = r.dir.cross(edgeb);
    // float det = dot(edgea, ray_cross_e2);
    
    

    return false;
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = true, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth=MAXFLOAT;
    // intersect the ray with the mesh BB
    if (!bb.intersect(r)) return false;
    
    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
        if (!intersect_this_face) continue;
        
        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }
    
    return intersect;
}
