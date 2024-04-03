//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "AmbientShader.hpp"
#include "Phong.hpp"
#include "PointLight.hpp"
#include "AmbientLight.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>  

RGB AmbientShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);

    
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }

    
    // verify whether the intersected object has an ambient component
    Phong *f = (Phong *)isect.f;
    RGB Ka = f->Ka;
    //std::cout << "Ambient Light Ka " << Ka.R << " " << Ka.G << " " << Ka.B << "\n";
    if (f->Ka.isZero()) return color;
    
    
    
    // ambient shade
    // Loop over scene's light sources and process Ambient Lights
    for (auto light_itr = scene->lights.begin() ; light_itr != scene->lights.end() ; light_itr++) {
        
        if ((*light_itr)->type == AMBIENT_LIGHT) { 
            // It is a ambient light
            color += Ka * (*light_itr)->L();
            continue;
        }
        else if((*light_itr)->type == POINT_LIGHT){
            // https://learnopengl.com/Lighting/Multiple-lights


            // It is a point light
            // The pointlight is a point that iluminates in all directions with a certain intensity
            PointLight *light = (PointLight *)(*light_itr);
            float lightIntensity = light->intensity;

            // Lets get the light direction
            Point auxLightDir = light->pos - isect.p;
            Vector lightDir(auxLightDir.X, auxLightDir.Y, auxLightDir.Z);


            auto lightDist = lightDir.norm();
            lightDir.normalize();

            Vector geoNormal = isect.gn;
            geoNormal.normalize();
        

            Point rayorigin = isect.p + geoNormal * 0.001;
            Ray shadowRay(rayorigin, lightDir);

            // Check if the shadow ray intersects any object
            Intersection shadowIsect;
            bool shadowIntersected = scene->trace(shadowRay, &shadowIsect);
            if (!shadowIntersected || shadowIsect.depth > lightDist) {
                // If the shadow ray intersects an object, the point is in shadow
                // We will use linear attenuation therefore the intensity of the light will decrease with the distance
                float attenuation = 1/(lightDist);

                RGB colorInSpot = light->color * f->Kd;
                float intensityMultiplier = light->intensity* attenuation ;

                color+=colorInSpot * intensityMultiplier;
            }
        
        }
    }
        
    return color;
};
