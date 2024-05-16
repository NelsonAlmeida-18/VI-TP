//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

//TODO: IDEAS: // Calculate the eta by running one test of the setup with one spp and then calculating the time it took to render and multiply by the spp


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

std::vector<AreaLight> GenerateAreaLights(Vector center,  RGB color, int width=40){

    // Point v1 = {238.0, 500,238.0 };
    // Point v2 = {328.0, 500,238.0};
    // Point v3 = {328.0, 500, 328.0};
    // Point v4 = {238.0, 500, 328.0};

    // Point v1v2 = (v2 - v1);
    // Point v1v3 = (v3 - v1);
    // Vector normal = {v1v2.X, v1v2.Y, v1v2.Z};
    // normal = normal.cross(Vector(v1v3.X, v1v3.Y, v1v3.Z));


    std::vector<AreaLight> lights;

    Point v1 = {center.X - width/2, center.Y, center.Z - width/2};
    Point v2 = {center.X + width/2, center.Y, center.Z - width/2};
    Point v3 = {center.X + width/2, center.Y, center.Z + width/2};
    Point v4 = {center.X - width/2, center.Y, center.Z + width/2};

    Point v1v2 = (v2 - v1);
    Point v1v3 = (v3 - v1);
    Vector normal = {v1v2.X, v1v2.Y, v1v2.Z};
    normal = normal.cross(Vector(v1v3.X, v1v3.Y, v1v3.Z));

    AreaLight light(RGB(0.65,0.65,0.65), v1,v2,v3, normal);
    AreaLight light2(RGB(0.65,0.65,0.65), v4,v3,v1, normal);
    
    lights.push_back(light);
    lights.push_back(light2);
    
    return lights;
}

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
    AmbientLight ambient(RGB(0.05,0.05,0.05));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    
    // // // Lets position this light in the ceiling where the light source is
    // PointLight light1(RGB(0.65,0.65,0.65), Point(288, 508, 282), 100.0);
    // scene.lights.push_back(&light1);
    // scene.numLights++;

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

    // v 556.0 548.8 0.0
    // v 556.0 548.8 559.2
    // v 0.0 548.8 559.2
    // v 0.0 548.8 0.0
    
    Point v1 = {238.0, 500,238.0 };
    Point v2 = {328.0, 500,238.0};
    Point v3 = {328.0, 500, 328.0};
    Point v4 = {238.0, 500, 328.0};

    Point v1v2 = (v2 - v1);
    Point v1v3 = (v3 - v1);
    Vector normal = {v1v2.X, v1v2.Y, v1v2.Z};
    normal = normal.cross(Vector(v1v3.X, v1v3.Y, v1v3.Z));

    AreaLight light6(RGB(0.65,0.65,0.65), v1,v2,v3, normal);
    scene.lights.push_back(&light6);
    scene.numLights++;

    AreaLight light7(RGB(0.65,0.65,0.65), v4,v3,v1, normal);
    scene.lights.push_back(&light7);
    scene.numLights++;

    // std::cout <<"Here\n";
    // std::vector<AreaLight> light1 = GenerateAreaLights(Vector(278, 548.8, 278), RGB(0.65,0.65,0.65), 80);
    // scene.lights.push_back(&light1[0]);
    // scene.lights.push_back(&light1[1]);
    // scene.numLights+=2;


    Point v5 = {138.0, 500,138.0 };
    Point v6 = {218.0, 500,138.0};
    Point v7 = {218.0, 500, 218.0};
    Point v8 = {138.0, 500, 218.0};

    Point v5v6 = (v6 - v5);
    Point v5v7 = (v7 - v5);
    Vector normal2 = {v5v6.X, v5v6.Y, v5v6.Z};
    normal2 = normal2.cross(Vector(v5v6.X, v5v6.Y, v5v6.Z));

    AreaLight light8(RGB(1,1,1), v5,v6,v7, normal2);
    scene.lights.push_back(&light8);
    scene.numLights++;

    // AreaLight light9(RGB(0.65,0.65,0.65), v7,v6,v5, normal2);
    // scene.lights.push_back(&light9);
    // scene.numLights++;

    // scene.lights.push_back(&light9);
    // scene.numLights++;
    // scene.lights.push_back(&light9);
    // scene.numLights++;
    // scene.lights.push_back(&light9);
    // scene.numLights++;
    // scene.lights.push_back(&light9);
    // scene.numLights++;
    // scene.lights.push_back(&light9);
    // scene.numLights++;

    

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
    RGB background(0, 0.5, 0.55);

    // shd = new AmbientShader(&scene, background);
    
    // Point lights
    // shd = new WhittedShader(&scene, background); 
    
    // Area lights
    // shd = new DistributedShader(&scene, background);

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
