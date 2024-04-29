//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "WhittedShader.hpp"
#include "Phong.hpp"
#include "ray.hpp"
#include "PointLight.hpp"

RGB WhittedShader::directLighting (Intersection isect, Phong *f) {
    RGB color(0.,0.,0.);
    
    // Loop over scene's light sources
    for (auto l = scene->lights.begin() ; l != scene->lights.end() ; l++) {
        
        if ((*l)->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            // If the light is an ambient light, we just add the ambient component of the material
            if (!f->Ka.isZero())  color += f->Ka * (*l)->L();
            continue;
        }

        if ((*l)->type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()){
                Point lpoint;

                PointLight *light = (PointLight *)(*l);

                RGB L = (*l)->Sample_L(NULL, &lpoint);

                // Lets debug the radiance sample
                // std::cout << "Radiance Sample: " << L.R << " " << L.G << " " << L.B << "\n";

                // Compute the directoin from the intersection to the light
                Vector lightDir = isect.p.vec2point(lpoint);
                const float lightDist = lightDir.norm();
                lightDir.normalize();

                // Rever isto da shading normal
                float cosL = lightDir.dot(isect.sn);

                if (cosL>0.){ // The light is not behind the primitive
                    Ray shadow(isect.p, lightDir);
                    // Adjust the epsilon of the origin to the normal
                    shadow.adjustOrigin(isect.gn);

                    if(scene->visibility(shadow, lightDist-EPSILON)) 
                        color+=f->Kd * L * cosL;

                }
                // ...
            }
        }
    }
    return color;
}

RGB WhittedShader::specularReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    
    float cos = isect.gn.dot(isect.wo);

    Vector rdir = 2*cos*isect.gn - isect.wo;
    Ray specular(isect.p, rdir);

    specular.adjustOrigin(isect.gn);

    Intersection sIsect;

    bool intersected = scene->trace(specular, &sIsect);

    color = shade(intersected, sIsect, depth+1);

    return color;
}

RGB WhittedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }
    // Está sempre a bater nesta isect is light...
    
    if (isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    
    
    // get the BRDF
    Phong *f = (Phong *)isect.f;
    
    // if there is a specular component sample it
    if (!f->Ks.isZero() && depth<3) {
        std::cout << "Specular Reflection\n";   
        color += specularReflection (isect, f, depth+1);
    }
    
    color += directLighting(isect, f);
    
    // std::cout << "Color: " << color.R << " " << color.G << " " << color.B << "\n";
    return color;
};
