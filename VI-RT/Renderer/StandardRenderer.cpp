//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;

void StandardRenderer::Render () {
    int W=512,H=512;  // resolution
    int x,y;

    // get resolution from the camera
    cam->getResolution(&W, &H);
    
    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            // For debugging lets project only one ray into a direction we know has a hit

            Ray primary;
            Intersection isect;
            bool intersected;
            RGB color;
            int depth = 0;
          
            // Generate Ray (camera)
            cam->GenerateRay(x,y,&primary);
            
            // trace ray (scene)
            // bool trace (Ray r, Intersection *isect);
            intersected = scene->trace(primary, &isect);

            if (!intersected) continue; // no intersection, continue with next ray
            std::cout << "Intersection at pixel (" << x << "," << y << ")\n";
            // shade this intersection (shader) - remember: depth=0
            std::cout << "Shading at pixel (" << x << "," << y << ")\n";
            color = shd->shade(intersected, isect, depth);
            //color = RGB(1.,0.,0.); // for debugging purposes (red color)
            
            // write the result into the image frame buffer (image)
            std::cout << "Color at pixel (" << x << "," << y << ") = R:" << color.R  <<" G: " << color.G << " B: " << color.B << "\n";
            img->set(x,y,color);
            
        } // loop over columns
    }   // loop over rows
}
