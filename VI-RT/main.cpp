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
#include "ImageEXR.hpp"
#include "ImageJPG.hpp"

#include "PointLightShader.hpp"

#include <time.h>



int main(int argc, const char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImageEXR *img;    // Image
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;

    // img = new ImageJPG(640,640);
    // img->createTestImage();
    // img->Save("./TestImage.jpg");


    
    
    if (argc>1) {
        // load the scene
        success = scene.Load(argv[1]);
    } else {
        // load the default scene
        success = scene.Load("./VI-RT/utils/cornell_box_vi.obj");
    }

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    std::cout << std::endl;

    
    
    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.05,0.05,0.05));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    
    // Lets position this light in the ceiling where the light source is
    PointLight light1(RGB(0.65,0.65,0.65), Point(288, 508, 282), 100.0);
    scene.lights.push_back(&light1);
    scene.numLights++;

    // PointLight light2(RGB(0.65,0.65,0.65), Point(248, 508, 242), 200.0);
    // scene.lights.push_back(&light2);
    // scene.numLights++;

    // PointLight light3(RGB(0.65,0.65,0.65), Point(328, 508, 242), 200.0);
    // scene.lights.push_back(&light3);
    // scene.numLights++;

    // PointLight light4(RGB(0.65,0.65,0.65), Point(248, 508, 322), 200.0);
    // scene.lights.push_back(&light4);
    // scene.numLights++;

    // PointLight light5(RGB(0.65,0.65,0.65), Point(328, 508, 322), 200.0);
    // scene.lights.push_back(&light5);
    // scene.numLights++;

    
    Point v1 = {273.0, 273.0, 273.5};
    Point v2 = {495.0, 495.0, 495.};
    Point v3 = {279.0, 279.0, 279.5};

    Point v1v2 = (v1 - v2);
    Point v1v3 = (v1 - v3);
    Vector normal = {v1v2.X, v1v2.Y, v1v2.Z};
    normal = normal.cross(Vector(v1v3.X, v1v3.Y, v1v3.Z));

    AreaLight light6(RGB(0.65,0.65,0.65), v1,v2,v3, -1.f*normal);
    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.lights.push_back(&light6);
    scene.numLights++;

    scene.printSummary();

    // Image resolution
    const int W= 640;
    const int H= 640;
    
    //img = new ImagePPM(W,H);
    // img = new ImageJPG(W,H);
    img = new ImageEXR(W,H);

    // Camera parameters
    const Point Eye ={280.0, 375.0, -830.0}, At={280.0, 265.0, 280.0};
    const Vector Up={0,1,0};
    const float fovW = 45.0, fovH = 45.0;
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    BB bb; // Create an instance of the BB c d dlass
    bool resultOfAABB = bb.testAABBIntersect(); // Call the testAABBIntersect() function on the instance
    std::cout << "Result of AABB test: " << resultOfAABB << std::endl; // Print the result of the test

    // create the shader
    RGB background(0, 0.5, 0.55);

    // shd = new AmbientShader(&scene, background);
    
    // Point lights
    // shd = new WhittedShader(&scene, background); 
    
    // Area lights
    //shd = new DistributedShader(&scene, background);

    // Path tracer
     shd = new PathTracerShader(&scene, background);

    std::cout << "Shader created\n";
    // declare the renderer
    int spp=16;     // samples per pixel
    int jitter=1;

    StandardRenderer myRender (cam, &scene, img, shd, spp, jitter);
    // render
    start = clock();
    std::cout << "Rendering\n";
    int numThreads = myRender.Render();
    std::cout << "Rendered\n";
    end = clock();
    cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC * numThreads);

    // save the image
    img->Save("./MyImage");

    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
