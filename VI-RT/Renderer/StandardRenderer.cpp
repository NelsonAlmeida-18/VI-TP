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
            // Initialize a floating-point image for accumulation
    cv::Mat floatImage = cv::Mat::zeros(H, W, CV_32FC3);

    // Initialize the final 8-bit image
    cv::Mat image = cv::Mat::zeros(H, W, CV_8UC3);

    // Array to store accumulated colors per pixel
    RGB colors[W * H] = { RGB(0, 0, 0) };

    for (int ss = 0; ss < spp; ss++) {
        // std::cout << "Sample: " << ss << "\n";
        // Main rendering loop: get primary rays from the camera until done
        for (int y = 0; y < H; y++) {  // loop over rows
            for (int x = 0; x < W; x++) { // loop over columns
                // For debugging let's project only one ray into a direction we know has a hit
                Ray primary;
                Intersection isect;
                bool intersected;
                RGB color;

                int depth = 0;

                // Generate Ray (camera)
                float jitterComponents[2];
                // Jitter is a random number between -0.1 and 0.1
                float jitterx = ((float)rand()) / ((float)RAND_MAX);
                float jittery = ((float)rand()) / ((float)RAND_MAX);

                jitterComponents[0] = jitterx * jitter;
                jitterComponents[1] = jittery * jitter;

                cam->GenerateRay(x, y, &primary, jitterComponents);

                // Trace ray (scene)
                intersected = scene->trace(primary, &isect);

                // Shade
                color = shd->shade(intersected, isect, depth);

                // Normalize the color by spp
                // color = color / spp;

                // Accumulate the color values
                colors[y * W + x] += color;

                // Update the floating-point image
                floatImage.at<cv::Vec3f>(y, x)[0] += color.B;
                floatImage.at<cv::Vec3f>(y, x)[1] += color.G;
                floatImage.at<cv::Vec3f>(y, x)[2] += color.R;

                // Write the result into the image frame buffer (image)
                if (ss == spp - 1) {
                    img->set(x, y, colors[y * W + x]/spp);
                }

                // Debug output for color values
                // std::cout << "Pixel color: R: " << color.R << " G: " << color.G << " B: " << color.B << "\n";
                // std::cout << "Pixel color [DEBUG]: " << floatImage.at<cv::Vec3f>(y, x) << "\n";
            } // loop over columns
        } // loop over rows

        // Display the image in an OpenCV window after each sample run
        if (interactiveOutput) {
            // Normalize the accumulated image to 8-bit for display
            cv::Mat displayImage;
            floatImage.convertTo(displayImage, CV_8UC3, (255.0/ss));
            std::string windowName = "Rendered Image";
            cv::imshow(windowName, displayImage);
            cv::waitKey(1);
        }
    }
    }

    return numThreads;
}

void StandardRenderer::paralelRender(int numThreads, int W, int H, int spp, int jitter) {
    cv::Mat image = cv::Mat::zeros(H, W, CV_32FC3); // Use a floating-point image for accurate accumulation
    RGB* colors = new RGB[W*H]();
    
        // Seed random number generator for each thread
        unsigned int seed = omp_get_thread_num();

        for (int ss = 0; ss < spp; ss++) {
            // std::cout << "Sample: " << ss << "\n";
            #pragma omp parallel num_threads(numThreads) 
    {
            #pragma omp for schedule(dynamic)
            // Main rendering loop: get primary rays from the camera until done
            for (int y = 0; y < H; y++) {  // loop over rows
                for (int x = 0; x < W; x++) { // loop over columns
                    // For debugging let's project only one ray into a direction we know has a hit
                    Ray primary;
                    Intersection isect;
                    bool intersected;
                    RGB color;
                    int depth = 0;

                    // Generate Ray (camera) with jitter
                    float jitterComponents[2];
                    float jitterx = static_cast<float>(rand_r(&seed)) / static_cast<float>(RAND_MAX);
                    float jittery = static_cast<float>(rand_r(&seed)) / static_cast<float>(RAND_MAX);

                    jitterComponents[0] = jitterx * jitter;
                    jitterComponents[1] = jittery * jitter;

                    cam->GenerateRay(x, y, &primary, jitterComponents);

                    // Trace ray (scene)
                    intersected = scene->trace(primary, &isect);

                    // Shade
                    color = shd->shade(intersected, isect, depth);

                    // Normalize the color by spp
                    // color = color / spp;

                    // Accumulate the color values
                    #pragma omp atomic
                    colors[y * W + x].R += color.R/spp;
                    
                    #pragma omp atomic
                    colors[y * W + x].G += color.G/spp;
                    #pragma omp atomic
                    colors[y * W + x].B += color.B/spp;

                    if(interactiveOutput){
                        // Update the floating-point image
                        #pragma omp atomic
                        image.at<cv::Vec3f>(y, x)[0] += color.B;
                        // image.at<cv::Vec3f>(y, x)[0] = std::min(1.0f, image.at<cv::Vec3f>(y, x)[0]);
                        #pragma omp atomic
                        image.at<cv::Vec3f>(y, x)[1] += color.G;
                        // image.at<cv::Vec3f>(y, x)[1] = std::min(1.0f, image.at<cv::Vec3f>(y, x)[1]);
                        #pragma omp atomic
                        image.at<cv::Vec3f>(y, x)[2] += color.R;
                        // image.at<cv::Vec3f>(y, x)[2] = std::min(1.0f, image.at<cv::Vec3f>(y, x)[2]);
                    }

                    if (ss == spp - 1) {
                        img->set(x, y, colors[y * W + x]);
                    }
                } // loop over columns
            }   // loop over rows

            if (interactiveOutput) {
                #pragma omp single
                {
                    // Normalize the accumulated image to 8-bit for display
                    cv::Mat displayImage;
                    image.convertTo(displayImage, CV_8UC3, 255.0/float(ss+1));

                    // Display the image in an OpenCV window after each sample run
                    cv::imshow("Rendered Image", displayImage);
                    cv::waitKey(1); // Small delay to allow the window to refresh without blocking
                }
            }
        }
    }
    delete[] colors;
}