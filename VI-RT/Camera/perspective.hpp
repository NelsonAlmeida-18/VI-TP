//
//  perspective.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#ifndef perspective_hpp
#define perspective_hpp

#include "camera.hpp"
#include "ray.hpp"
#include "vector.hpp"

class Perspective: public Camera {
    Point Eye, At;
    Vector Up;
    float fovW, fovH;
    int W, H;
    float c2w[3][3];  // camera 2 world transform
public:
    Perspective (const Point Eye, const Point At, const Vector Up, const int W, const int H, const float fovW, const float fovH): Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH)  {
        // compute camera 2 world transform
        // Here we calculated the F(forward) vector
        Point fAux = At - Eye;
        Vector f(fAux.X, fAux.Y, fAux.Z);
        f.normalize();
        // Lets get the R(right) vector
        Vector r = f.cross(Up);
        r.normalize();
        // Lets get the U(up) vector
        Vector u = Up;
        u.normalize();

        // Now we can build the camera 2 world transform
        c2w[0][0] = r.X; c2w[0][1] = r.Y; c2w[0][2] = r.Z;
        c2w[1][0] = u.X; c2w[1][1] = u.Y; c2w[1][2] = u.Z;
        c2w[2][0] = f.X; c2w[2][1] = f.Y; c2w[2][2] = f.Z;
        // Based on the slides 
        
    }
    bool GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter=NULL);
    void getResolution (int *_W, int *_H) {*_W=W; *_H=H;}
};

#endif /* perspective_hpp */
