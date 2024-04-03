//
//  geometry.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef geometry_hpp
#define geometry_hpp

#include "BB.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class Geometry {
public:
    Geometry () {}
    virtual ~Geometry () {}
    // return True if r intersects this geometric primitive
    // returns data about intersection on isect
    virtual bool intersect (Ray r, Intersection *isect) { return false; };
    //Bidirectional reflectance distribution function
    //É uma função que define como a luz de uma fonte é refletina numa superficie opaca
    BRDF f;
    // geometric primitive bounding box
    BB bb;  // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
};

#endif /* geometry_hpp */
