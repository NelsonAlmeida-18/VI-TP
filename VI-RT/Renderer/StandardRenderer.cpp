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
            // Jitter is a random number between -0.1 and 0.1
            float jitter = (rand() % 100) / 1000.0 - 0.05;
            jitter=0;
            // std::cout << "Jitter " << jitter << "\n";
            cam->GenerateRay(x,y,&primary, &jitter);
            
            // trace ray (scene)
            intersected = scene->trace(primary, &isect);

            if (!intersected) continue; // no intersection, continue with next ray

            // Shade 
            color = shd->shade(intersected, isect, depth);
            
            // write the result into the image frame buffer (image)
            img->set(x,y,color);
            
        } // loop over columns
    }   // loop over rows
}
