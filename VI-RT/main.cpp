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

#include "PointLightShader.hpp"

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
        success = scene.Load("/Users/rkeat/Desktop/Universidade/1anoMestrado/2semestre/VI-TP/VI-RT/utils/cornell_box_vi.obj");
    }

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    std::cout << std::endl;

    
    
    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.04,0.04,0.04));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    
    // Lets position this light in the ceiling where the light source is
    PointLight light1(RGB(0.65,0.65,0.65), Point(288, 508, 282), 100.0);
    scene.lights.push_back(&light1);
    scene.numLights++;

    PointLight light2(RGB(0.65,0.65,0.65), Point(248, 508, 242), 200.0);
    scene.lights.push_back(&light2);
    scene.numLights++;

    PointLight light3(RGB(0.65,0.65,0.65), Point(328, 508, 242), 200.0);
    scene.lights.push_back(&light3);
    scene.numLights++;

    PointLight light4(RGB(0.65,0.65,0.65), Point(248, 508, 322), 200.0);
    scene.lights.push_back(&light4);
    scene.numLights++;

    PointLight light5(RGB(0.65,0.65,0.65), Point(328, 508, 322), 200.0);
    scene.lights.push_back(&light5);
    scene.numLights++;

    scene.printSummary();

    // Image resolution
    const int W= 640;
    const int H= 640;
    
    img = new ImagePPM(W,H);

    // Camera parameters
    const Point Eye ={280.0, 375.0, -830.0}, At={280.0, 265.0, 280.0};
    const Vector Up={0,1,0};
    const float fovW = 45.0, fovH = 45.0;
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    BB bb; // Create an instance of the BB c d dlass
    bool resultOfAABB = bb.testAABBIntersect(); // Call the testAABBIntersect() function on the instance
    std::cout << "Result of AABB test: " << resultOfAABB << std::endl; // Print the result of the test

    // create the shader
    RGB background(0.05, 0.05, 0.55);

    // shd = new AmbientShader(&scene, background);
    shd = new WhittedShader(&scene, background);

    std::cout << "Shader created\n";
    // declare the renderer
    int spp=4;     // samples per pixel

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
