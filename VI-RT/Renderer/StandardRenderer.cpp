//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <iostream>
#include <stdio.h>
#include <omp.h>

using namespace std;

int StandardRenderer::Render () {
    int openmp = 0;
    int W=512,H=512;  // resolution
    int x,y;

    int ss;
    // get resolution from the camera
    cam->getResolution(&W, &H);
    std::cout << "Samples per pixel: " << spp << "\n";
    int numThreads = 1;

    if (openmp==1) numThreads = omp_get_max_threads();
    std::cout << "Number of threads: " << numThreads << "\n";
    #pragma omp parallel num_threads(numThreads) private(x,y,ss)
        #pragma omp for schedule(dynamic, numThreads)
        // main rendering loop: get primary rays from the camera until done
        for (y=0 ; y< H ; y++) {  // loop over rows
            for (x=0 ; x< W ; x++) { // loop over columns
                // For debugging lets project only one ray into a direction we know has a hit
                Ray primary;
                Intersection isect;
                bool intersected;    
                RGB color;
                for (ss=0; ss<spp; ss++){

                    int depth = 0;
                    
                    // Lets add the x and y component of the jitter
                    float jitterComponents[2];
                    // Generate Ray (camera)
                    // Jitter is a random number between -0.1 and 0.1
                    float jitterx = ((float)rand()) / ((float)RAND_MAX);
                    float jittery = ((float)rand()) / ((float)RAND_MAX);
                    
                    jitterComponents[0] = jitterx*jitter;
                    jitterComponents[1] = jittery*jitter;

                    // std::cout << "Jitter " << jitter << "\n";
                    cam->GenerateRay(x,y,&primary, jitterComponents);
                    
                    // trace ray (scene)
                    intersected = scene->trace(primary, &isect);

                    // if (!intersected) continue; // no intersection, continue with next ray

                    // Shade 
                    color+= shd->shade(intersected, isect, depth);
                }

                color = color/spp;
                
                // write the result into the image frame buffer (image)
                img->set(x,y,color);
                
            } // loop over columns
        }   // loop over rows
    
    return numThreads;
}
