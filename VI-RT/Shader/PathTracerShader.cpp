//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "PathTracerShader.hpp"
#include "Phong.hpp"
#include "ray.hpp"
#include "AreaLight.hpp"
#include <stdlib.h>
#include <math.h>

//#include "DEB.h"

RGB PathTracerShader::directLighting (Intersection isect, Phong *f){
    RGB color(0.,0.,0.);
    Light *l;


    // Stochastic selection here, 
    // Pseudo code, get the number of area lights in the scene
    // select a random number between 0 and numAreaLights
    // Get that light
    // Verify the visibility of the light
    //  probability of being selected = 1/Nlights
    // color = color_l/p = color_l*Nlights
    
    
    // // Stochastic sampling
    // // Lets get a random number between 0 and numLights
    int lightNdx = rand() % scene->numLights;

    // // Get the light
    Light *l_iter = scene->lights[lightNdx];


    // // The probability of it being selected = 1/Nlights
    float probOfSelection = 1.0/scene->numLights;

    // // The color of the light is color_l/p
    isect.Le = isect.Le/probOfSelection;

    // for (auto l_iter=scene->lights.begin() ; l_iter != scene->lights.end() ; l_iter++) {
        RGB this_l_color (0.,0.,0.);
        l = (Light *) (l_iter);
                
        if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                this_l_color = Ka * l->L();
            }
        }
        if (l->type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;
                
                // get the position and radiance of the light source
                L = l->Sample_L(NULL, &lpoint);
                
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                
                // now normalize Ldir
                Ldir.normalize();
                
                // compute the cosine between Ldir  and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                
                // shade
                if (cosL>0.)  { // the light is NOT behind the porimitive
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);
                    
                    shadow.pix_x = isect.pix_x;
                    shadow.pix_y = isect.pix_y;
                    
                    shadow.FaceID = isect.FaceID;
                    
                    // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
                    shadow.adjustOrigin(isect.gn);
                    
                    if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
                        this_l_color = Kd * L * cosL;
                    }
                } // end cosL > 0.
            }
        }
        if (l->type == AREA_LIGHT) {  // is it an area light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;
                float l_pdf;
                AreaLight *al = (AreaLight *)l;

                float rnd[2];
                rnd[0] = ((float)rand()) / ((float)RAND_MAX);
                rnd[1] = ((float)rand()) / ((float)RAND_MAX);
                L = al->Sample_L(rnd, &lpoint, l_pdf);

                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                // now normalize Ldir
                Ldir.normalize();
                // cosine between Ldir  and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                // cosine between Ldir  and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                
                // shade
                if (cosL>0. and cosL_LA<=0.) { // light NOT behind primitive AND light normal points to the ray o
                    // generate the shadow ray
            
                    Ray shadow(isect.p, Ldir);
                                       
                    shadow.pix_x = isect.pix_x;
                    shadow.pix_y = isect.pix_y;
                    
                    shadow.FaceID = isect.FaceID;
                    
                    // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
                    shadow.adjustOrigin(isect.gn);
                    
                    if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
                        color+= (Kd * L * cosL)/l_pdf;
                    }
                }
    
            }
            
        }  // end area light
        
        color += this_l_color;

    // } // for loop
    return color/probOfSelection;

}

RGB PathTracerShader::specularReflection (Intersection isect, Phong *f, int depth) {
    // RGB color(0.,0.,0.);
    // Vector Rdir, s_dir;
    
    // // generate the specular ray
    
    // // IDEAL SPECULAR REFLECTION
    // // direction R = 2 (N.V) N - V
    // float cos = isect.gn.dot(isect.wo);
    // Rdir = 2.f * cos * isect.gn - isect.wo;
    

    // if (f->Ns < 1000) { // glossy materials
    //     // actual direction distributed around Rdir according to the cosine lobe
    //     // generate the cosine lobel sampled direction around (0,0,1)
    //     // following item (36) of the Global illumination compendium
    //     // get 2 random number in [0,1[
    //     float rnd[2];
    //     rnd[0] = ((float)rand()) / ((float)RAND_MAX);
    //     rnd[1] = ((float)rand()) / ((float)RAND_MAX);
        
    //     Vector S_around_N;
    //     //  generate s_dir
        
        
    //     Ray specular(isect.p, s_dir);
        
    //     specular.pix_x = isect.pix_x;
    //     specular.pix_y = isect.pix_y;
        
    //     specular.FaceID = isect.FaceID;

    //     specular.adjustOrigin(isect.gn);

    //     // OK, we have the ray : trace and shade it recursively
    //     bool intersected;
    //     Intersection s_isect;
    //     // trace ray
    //     intersected = scene->trace(specular, &s_isect);

    //     // shade this intersection
    //     RGB Rcolor = shade (intersected, s_isect, depth+1);
        
    //     // evaluate this ray contribution, i.e., color
    //     color = (f->Ks  * Rcolor)  ;

    //     return color;

    // }
    // else {          // ideal specular reflection
    //     Ray specular(isect.p, Rdir);
        
    //     specular.pix_x = isect.pix_x;
    //     specular.pix_y = isect.pix_y;
        
    //     specular.FaceID = isect.FaceID;

    //     specular.adjustOrigin(isect.gn);

    //     // OK, we have the ray : trace and shade it recursively
    //     bool intersected;
    //     Intersection s_isect;
    //     // trace ray
    //     intersected = scene->trace(specular, &s_isect);

    //     // shade this intersection
    //     RGB Rcolor = shade (intersected, s_isect, depth+1);
        
    //     color = (f->Ks  * Rcolor)  ;
    //     return color;
    // }
    RGB color(0.,0.,0.);
    Vector Rdir, s_dir;
    float pdf;
    Intersection s_isect;

    float cos = isect.gn.dot(isect.wo);
    Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);

    // trace the ray
    bool intersected = scene->trace(specular, &s_isect);
    RGB rcolor = shade(intersected, s_isect, depth+1);

    color = (f->Ks * rcolor);
    return color;
}

RGB PathTracerShader::diffuseReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    Vector dir;
    float pdf;
    
    // generate the specular ray
    
    // actual direction distributed around N
    // get 2 random number in [0,1[
    float rnd[2];
    rnd[0] = ((float)rand()) / ((float)RAND_MAX);
    rnd[1] = ((float)rand()) / ((float)RAND_MAX);

    // std::cout << "rnd[0] " << rnd[0] << " rnd[1] " << rnd[1] << "\n";
        
    Vector D_around_Z;
    // cosine sampling
    float cos_theta = D_around_Z.Z = sqrtf(rnd[1]);
    D_around_Z.Y = sinf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    D_around_Z.X = cosf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);

    pdf = cos_theta / M_PI;
        
    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);
        
    
    dir = D_around_Z.Rotate  (Rx, Ry, isect.gn);
        
    Ray diffuse(isect.p, D_around_Z.Rotate(Rx, Ry, isect.gn));
        
    // diffuse.pix_x = isect.pix_x;
    // diffuse.pix_y = isect.pix_y;
        
    // diffuse.FaceID = isect.FaceID;

    // diffuse.adjustOrigin(isect.gn);

    // OK, we have the ray : trace and shade it recursively
    
    Intersection d_isect;
    // trace ray
    bool intersected = scene->trace(diffuse, &d_isect);

    if (!d_isect.isLight) {  // if light source return 0 ; handled by direct
        // shade this intersection
        RGB Rcolor = shade(intersected, d_isect , depth+5);
        // TODO divide by pdf
        // color = (f->Kd * cos_theta * Rcolor) / pdf;
        // TODO: Fix in the future, bug in the shade of the rcolor

        // RGB Rcolor = isect.Le;
        color = (f->Kd * cos_theta ) /pdf;
    }
    return color;

}


RGB PathTracerShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }
    
    if (isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    
    // get the BRDF
    Phong *f = (Phong *)isect.f;
    
    float rnd_russian = ((float)rand()) / ((float)RAND_MAX);

    if (depth <MAX_DEPTH || rnd_russian < continue_p) {
        // Montecarlo sampling 
        RGB lColor;
        float s_p = f->Ks.Y() / (f->Kd.Y() + f->Ks.Y());
        float rnd = ((float)rand()) / ((float)RAND_MAX);
        if (rnd<=s_p || s_p >= (1.-EPSILON)) {
            lColor = specularReflection(isect, f, depth)/s_p;
        }
        else {
            lColor = diffuseReflection(isect, f, depth)/(1-s_p);
        }
        if(depth<MAX_DEPTH) {
            color += lColor;
        }
        else color+=lColor/continue_p;
    }
    
    
    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) {
        color += directLighting(isect, f);
        // color+=f->Kd;
    }
        
    return color;
};
