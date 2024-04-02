//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include <iostream>
#include "scene.hpp"
#include "perspective.hpp"
#include "StandardRenderer.hpp"
#include "ImagePPM.hpp"
#include "AmbientShader.hpp"
#include "WhittedShader.hpp"
#include "DistributedShader.hpp"
#include "PathTracerShader.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"

#include <time.h>

int main(int argc, const char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImagePPM *img;    // Image
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;
    
    
    if (argc>1) {
        // load the scene
        success = scene.Load(argv[1]);
    } else {
        // load the default scene
        success = scene.Load("/home/gustavo/cg/VI-TP/VI-RT/utils/cornell-box.obj");
    }
    
    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    std::cout << std::endl;
    
    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.9,0.9,0.9));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    
    scene.printSummary();

    // Image resolution
    const int W= 512;
    const int H= 512;
    
    img = new ImagePPM(W,H);

    // Camera parameters
    const Point Eye ={300.0, 150.0, -800.0}, At={300.0, 150.0, 300.0};
    const Vector Up={0,1,0};
    const float fovW = 45.0, fovH = 45.0;
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    BB bb; // Create an instance of the BB c d dlass
    bool resultOfAABB = bb.testAABBIntersect(); // Call the testAABBIntersect() function on the instance
    std::cout << "Result of AABB test: " << resultOfAABB << std::endl; // Print the result of the test

    // create the shader
    RGB background(0.05, 0.05, 0.55);
    shd = new AmbientShader(&scene, background);
    std::cout << "Shader created\n";
    // declare the renderer
    int spp=1;     // samples per pixel

    StandardRenderer myRender (cam, &scene, img, shd, spp);
    // render
    start = clock();
    std::cout << "Rendering\n";
    myRender.Render();
    std::cout << "Rendered\n";
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // save the image
    img->Save("./MyImage.ppm");

    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
