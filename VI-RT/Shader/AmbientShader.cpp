//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "AmbientShader.hpp"
#include "Phong.hpp"

RGB AmbientShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    RGB zeroKaDebug(1.0,0.,0.);

    
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }

    // pus aqui uma negação porque estava sempre a cair neste return
    if (!isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    
    // verify whether the intersected object has an ambient component
    Phong *f = (Phong *)isect.f;
    RGB Ka = f->Ka;
    //std::cout << "Ambient Light Ka " << Ka.R << " " << Ka.G << " " << Ka.B << "\n";
    std::cout << "Face " << isect.FaceID << std::endl;
    if (f->Ka.isZero()) return color;
    
    if (isect.FaceID == 0) {
        std::cout << "Face " << isect.FaceID << " has ambient color " << f->Ka.R << " " << f->Ka.G << " " << f->Ka.B << "\n";
    }
    
    // ambient shade
    // Loop over scene's light sources and process Ambient Lights
    for (auto light_itr = scene->lights.begin() ; light_itr != scene->lights.end() ; light_itr++) {
        
        if ((*light_itr)->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            RGB src = (*light_itr)->L();
            color += Ka * (*light_itr)->L();
            continue;
        }
    }
        
    return color;
};
