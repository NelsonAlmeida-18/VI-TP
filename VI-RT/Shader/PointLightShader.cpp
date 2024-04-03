
#include "PointLightShader.hpp"
#include "Phong.hpp"
#include "PointLight.hpp"
#include "AmbientLight.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>  

RGB PointLightShader::shade(bool intersected, Intersection isect, int depth) {
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
        
        if((*light_itr)->type == POINT_LIGHT){
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

            // // Check if the shadow ray intersects any object
            Intersection shadowIsect;
            bool shadowIntersected = scene->trace(shadowRay, &shadowIsect);
            if (shadowIntersected) {
                // If the shadow ray intersects an object, the point is in shadow
                continue;
            }

            
            // color +=   f->Kd * lightIntensity * fmax(0.0, lightDir.dot(geoNormal));
            color+= f->Kd * lightIntensity * fmax(0.0, lightDir.dot(geoNormal));

            // Probably add a cap to the light in order for it to not iluminate the whole scene
            // https://learnopengl.com/Lighting/Multiple-lights

            // color += RGB(attenuation,attenuation, attenuation) * (f->Kd + f->Ka + f->Ks);
            
            std::cout << "Color: " << color.R << " " << color.G << " " << color.B << "\n";
            std::cout << "Attenuation factor" << 0 << "\n";

            //             // Point rayorigin = isect.p + 0.001 * lightDir;
            // Point rayorigin = isect.p + 0.001 * lightDir;
            // Ray shadowRay(rayorigin, lightDir);

            // // Check if the shadow ray intersects any object
            // Intersection shadowIsect;
            // bool shadowIntersected = scene->trace(shadowRay, &shadowIsect);
            // if (shadowIntersected) {
            //     // If the shadow ray intersects an object, the point is in shadow
            //     continue;
            // }

            // // Light atenuation based in the inverse square law (inverse of their distance squared)
            // float distance = lightDir.norm();
            // float attenuation = 1.0 / (distance * distance);


            // // Diffuse shading
            // RGB Kd = f->Kd;
            // float diffuseFactor = std::max(0.0f, lightDir.dot(isect.sn));
            // color += Kd * diffuseFactor * lightIntensity * attenuation;

            
            // // Specular shading
            // RGB Ks = f->Ks;
            // float specularFactor = 0.0;
            // if (diffuseFactor > 0) {
            //     Vector viewDir = -isect.wo; // Assuming ray direction points towards viewer
            //     // We use sn instead of gn because we want the shading normal
            //     Vector reflectDir = lightDir - 2 * (lightDir.dot(isect.sn)) * isect.sn;
            //     specularFactor = powf(std::max(0.0f, viewDir.dot(reflectDir)), f->Ns);
            // }
            // color += Ks * specularFactor * lightIntensity * attenuation;
        }
    }
        
    return color;
};
