//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#include "perspective.hpp"
#include <stdio.h>
#include <iostream>

bool Perspective::GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter) {
    // TODO: Verify cause this is returns a boolean value
    // Lets define the ray origin by setting it to the eye position
    
    r->o = Eye;

    // Theta h = Fovy
    // Theta w = Fovx
    // Lets define the ray 
    float xs = (2*(x+0.5)/W)-1;
    float ys =((2*(H-y-1)+0.5)/H)-1;

    float xc = xs*tan(fovW/2);
    float yc = ys*tan(fovH/2);
    

    // Vector dir = Vector(xc, yc, 1);
    float coord[3] = {xc, yc, 1};
    float dir[3] = {0.0,0.0,0.0};
    
    // Lets multiply the dir with the c2w matrix
    for (int i=0; i<3; i++) {
        for (int j=0; j<3;j++) {
            dir[i] += c2w[i][j]*coord[j];
        }
    }

    r->dir = Vector(dir[0], dir[1], dir[2]);
    r->dir.normalize();

    // Vector invDir = Vector(1/(r->dir.X), 1/(r->dir.Y), 1/(r->dir.Z));
    // invDir.normalize();

    // r->invDir = invDir;

    // Suposing that the pix_x and pix_y are the representation in the camera space of the x and y
    r->pix_x = xc;
    r->pix_y = yc;

    return true;
}
