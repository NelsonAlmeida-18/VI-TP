//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "vector.hpp"
#include "ray.hpp"


typedef struct BB {
public:
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }
    /*
     * I suggest you implement:
     *  bool intersect (Ray r) { }
     *
     * based on PBRT's 3rd ed. book , sec 3.1.2, pags 125..128 + 214,217,221
     *
     * or https://doi.org/10.1007/978-1-4842-7185-8_2
     *
     */
    bool intersect (Ray r) {
        
        // Found on: https://tavianator.com/2022/ray_box_boundary.html

        // AABB are defined by two terms, a minimum and a max bounding box
        float tmin = 0.0;
        float tmax = std::numeric_limits<float>::max();


        float t1x = (min.X - r.o.X) * r.invDir.X;
        float t2x = (max.X - r.o.X) * r.invDir.X;

        tmin = std::fmin(std::fmax(t1x, tmin), std::fmax(t2x, tmin));
        tmax = std::fmax(std::fmin(t1x, tmax), std::fmin(t2x, tmax));

        float t1y = (min.Y - r.o.Y) * r.invDir.Y;
        float t2y = (max.Y - r.o.Y) * r.invDir.Y;

        tmin = std::fmin(std::fmax(t1y, tmin), std::fmax(t2y, tmin));
        tmax = std::fmax(std::fmin(t1y, tmax), std::fmin(t2y, tmax));

        float t1z = (min.Z - r.o.Z) * r.invDir.Z;
        float t2z = (max.Z - r.o.Z) * r.invDir.Z;

        tmin = std::fmin(std::fmax(t1z, tmin), std::fmax(t2z, tmin));
        tmax = std::fmax(std::fmin(t1z, tmax), std::fmin(t2z, tmax));
        
        // This mitigates the edge case where the ray is parallel to the plane or touches the edge of the plane
        return tmin<=tmax; 
    }
} BB;

#endif /* AABB_hpp */
