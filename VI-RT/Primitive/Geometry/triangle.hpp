    //
    //  mesh.hpp
    //  VI-RT
    //
    //  Created by Luis Paulo Santos on 05/02/2023.
    //

    #ifndef triangle_hpp
    #define triangle_hpp

    #include "geometry.hpp"
    #include "vector.hpp"
    #include <math.h>

    class Triangle: public Geometry {
    public:
        Point v1, v2, v3;
        Vector normal;           // geometric normal
        Vector edge1, edge2;
        BB bb;      // face bounding box
                    // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
        bool intersect (Ray r, Intersection *isect);
        bool isInside(Point p);
        
        Triangle(Point _v1, Point _v2, Point _v3, Vector _normal): v1(_v1), v2(_v2), v3(_v3), normal(_normal) {
            edge1 = v1.vec2point(v2);
            edge2 = v1.vec2point(v3);
            bb.min.set(v1.X, v1.Y, v1.Z);
            bb.max.set(v1.X, v1.Y, v1.Z);
            bb.update(v2);
            bb.update(v3);
        }
        
        // Heron's formula
        // https://www.mathopenref.com/heronsformula.html
        float area () {

            const float ax = v1.X-v2.X;
            const float ay = v1.Y-v2.Y;
            const float az = v1.Z-v2.Z;
            const float aSize = sqrt(ax*ax + ay*ay + az*az);

            const float bx = v2.X-v3.X;
            const float by = v2.Y-v3.Y;
            const float bz = v2.Z-v3.Z;
            const float bSize = sqrt(bx*bx + by*by + bz*bz);

            const float cx = v3.X-v1.X;
            const float cy = v3.Y-v1.Y;
            const float cz = v3.Z-v1.Z;
            const float cSize = sqrt(cx*cx + cy*cy + cz*cz);

            const float p = (aSize + bSize + cSize) / 2.f;
            const float A = sqrt(p*(p-aSize)*(p-bSize)*(p-cSize));

            return A;
        }
        float points_area (Point v1, Point v2, Point v3) {
            
            // ...
            
            //const float A = ...
            const float A = 1.f;

            return A;

        }
    };

    #endif /* triangle_hpp */
