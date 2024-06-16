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
#include <opencv2/opencv.hpp>

using namespace std;
int StandardRenderer::Render() {
    int W, H;

    // get resolution from the camera
    cam->getResolution(&W, &H);
    int x, y, ss;

    std::cout << "Samples per pixel: " << spp << "\n";
    int numThreads = 1;

    if (parallel) {
        numThreads = omp_get_max_threads();
        std::cout << "Number of threads: " << numThreads << "\n";
        paralelRender(numThreads, W, H, spp, jitter);
    } else {
        std::cout << "Number of threads: " << numThreads << "\n";
        cv::Mat image = cv::Mat::zeros(H, W, CV_8UC3);

        for (ss = 0; ss < spp; ss++) {
            
            // main rendering loop: get primary rays from the camera until done
            for (y = 0; y < H; y++) {  // loop over rows
                for (x = 0; x < W; x++) { // loop over columns
                    // For debugging let's project only one ray into a direction we know has a hit
                    Ray primary;
                    Intersection isect;
                    bool intersected;
                    RGB color;
                    
                    int depth = 0;

                    // Let's add the x and y component of the jitter
                    float jitterComponents[2];
                    // Generate Ray (camera)
                    // Jitter is a random number between -0.1 and 0.1
                    float jitterx = ((float)rand()) / ((float)RAND_MAX);
                    float jittery = ((float)rand()) / ((float)RAND_MAX);

                    jitterComponents[0] = jitterx * jitter;
                    jitterComponents[1] = jittery * jitter;

                    cam->GenerateRay(x, y, &primary, jitterComponents);

                    // trace ray (scene)
                    intersected = scene->trace(primary, &isect);

                    // Shade
                    color += shd->shade(intersected, isect, depth);

                    // write the result into the image frame buffer (image)
                    img->set(x, y, color);

                    // write the result into the OpenCV image
                    cv::Vec3b& cvColor = image.at<cv::Vec3b>(y, x);
                    cvColor[0] = static_cast<uchar>(color.B * 255);
                    cvColor[1] = static_cast<uchar>(color.G * 255);
                    cvColor[2] = static_cast<uchar>(color.R * 255);
                } // loop over columns
            }   // loop over rows
            if(interactiveOutput){
                // Display the image in an OpenCV window after each sample run
                std::string windowName = "Rendered Image";
                // sprintf(windowName, "Rendered Image - Sample %d", ss);
                cv::imshow(windowName, image);
                cv::waitKey(1);
            }

        }
    }


    return numThreads;
}
void StandardRenderer::paralelRender(int numThreads, int W, int H, int spp, int jitter) {
    int x, y, ss;
    cv::Mat image = cv::Mat::zeros(H, W, CV_8UC3);
    #pragma omp parallel num_threads(numThreads) private(x, y, ss)
    {
        for (ss = 0; ss < spp; ss++) {
            #pragma omp for schedule(dynamic, numThreads)
            // main rendering loop: get primary rays from the camera until done
            for (y = 0; y < H; y++) {  // loop over rows
                for (x = 0; x < W; x++) { // loop over columns
                    // For debugging let's project only one ray into a direction we know has a hit
                    Ray primary;
                    Intersection isect;
                    bool intersected;
                    RGB color;
                    int depth = 0;

                    // Let's add the x and y component of the jitter
                    float jitterComponents[2];
                    // Generate Ray (camera)
                    // Jitter is a random number between -0.1 and 0.1
                    float jitterx = ((float)rand()) / ((float)RAND_MAX);
                    float jittery = ((float)rand()) / ((float)RAND_MAX);

                    jitterComponents[0] = jitterx * jitter;
                    jitterComponents[1] = jittery * jitter;

                    cam->GenerateRay(x, y, &primary, jitterComponents);

                    // trace ray (scene)
                    intersected = scene->trace(primary, &isect);

                    // Shade
                    color += shd->shade(intersected, isect, depth);

                    // write the result into the image frame buffer (image)
                    img->set(x, y, color);

                    // write the result into the OpenCV image
                    cv::Vec3b& cvColor = image.at<cv::Vec3b>(y, x);
                    cvColor[0] = static_cast<uchar>(color.B * 255);
                    cvColor[1] = static_cast<uchar>(color.G * 255);
                    cvColor[2] = static_cast<uchar>(color.R * 255);
                } // loop over columns
            }   // loop over rows

            #pragma omp single
            {
                // Display the image in an OpenCV window after each sample run
                cv::imshow("Rendered Image", image);
                cv::waitKey(1); // Small delay to allow the window to refresh without blocking
            }
        }
    }

}