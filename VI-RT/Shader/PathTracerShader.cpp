// //
// //  AmbientShader.cpp
// //  VI-RT-LPS
// //
// //  Created by Luis Paulo Santos on 14/03/2023.
// //

// #include "PathTracerShader.hpp"
// #include "Phong.hpp"
// #include "ray.hpp"
// #include "AreaLight.hpp"
// #include <stdlib.h>
// #include <math.h>

// //#include "DEB.h"

// RGB PathTracerShader::directLighting (Intersection isect, Phong *f){
//     RGB color(0.,0.,0.);
//     Light *l;

//     for (auto l_iter=scene->lights.begin() ; l_iter != scene->lights.end() ; l_iter++) {
//         RGB this_l_color (0.,0.,0.);
//         l = (Light *) (*l_iter);
                
//         if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
//             if (!f->Ka.isZero()) {
//                 RGB Ka = f->Ka;
//                 this_l_color += Ka * l->L();
//             }
//         }
//         if (l->type == POINT_LIGHT) {  // is it a point light ?
//             if (!f->Kd.isZero()) {
//                 RGB L, Kd = f->Kd;
//                 Point lpoint;
                
//                 // get the position and radiance of the light source
//                 L = l->Sample_L(NULL, &lpoint);
                
//                 // compute the direction from the intersection point to the light source
//                 Vector Ldir = isect.p.vec2point(lpoint);
//                 const float Ldistance = Ldir.norm();
                
//                 // now normalize Ldir
//                 Ldir.normalize();
                
//                 // compute the cosine between Ldir  and the shading normal at the intersection point
//                 float cosL = Ldir.dot(isect.sn);
                
//                 // shade
//                 if (cosL>0.)  { // the light is NOT behind the porimitive
//                     // generate the shadow ray
//                     Ray shadow(isect.p, Ldir);
                    
//                     shadow.pix_x = isect.pix_x;
//                     shadow.pix_y = isect.pix_y;
                    
//                     shadow.FaceID = isect.FaceID;
                    
//                     // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
//                     shadow.adjustOrigin(isect.gn);
                    
//                     if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
//                         this_l_color += Kd * L * cosL;
//                     }
//                 } // end cosL > 0.
//             }
//         }
//         if (l->type == AREA_LIGHT) {  // is it an area light ?
//             if (!f->Kd.isZero()) {
//                 RGB L, Kd = f->Kd;
//                 Point lpoint;
//                 float l_pdf;
//                 AreaLight *al = (AreaLight *)l;

//                 float rnd[2];
//                 rnd[0] = ((float)rand()) / ((float)RAND_MAX);
//                 rnd[1] = ((float)rand()) / ((float)RAND_MAX);
//                 L = al->Sample_L(rnd, &lpoint, l_pdf);

//                 // compute the direction from the intersection point to the light source
//                 Vector Ldir = isect.p.vec2point(lpoint);
//                 const float Ldistance = Ldir.norm();
//                 // now normalize Ldir
//                 Ldir.normalize();
//                 // cosine between Ldir  and the shading normal at the intersection point
//                 float cosL = Ldir.dot(isect.sn);
//                 // cosine between Ldir  and the area light source normal
//                 float cosL_LA = Ldir.dot(al->gem->normal);
                
//                 // shade
//                 if (cosL>0. and cosL_LA<=0.) { // light NOT behind primitive AND light normal points to the ray o
//                     // generate the shadow ray
//                     Ray shadow(isect.p, Ldir);
                    
//                     shadow.pix_x = isect.pix_x;
//                     shadow.pix_y = isect.pix_y;
                    
//                     shadow.FaceID = isect.FaceID;
                    
//                     // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
//                     shadow.adjustOrigin(isect.gn);
                    
//                     if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
//                         this_l_color += (Kd * L * cosL)/ l_pdf;
//                     }

//                 }
//             }
            
//         }  // end area light
        
//         color += this_l_color;

//     } // for loop
//     return color;
// }

// RGB PathTracerShader::specularReflection (Intersection isect, Phong *f, int depth) {
//     RGB color(0.,0.,0.);
//     Vector Rdir, s_dir;
//     Intersection s_isect;
    
//     // generate the specular ray
    
//     // IDEAL SPECULAR REFLECTION
//     // direction R = 2 (N.V) N - V
//     float cos = isect.gn.dot(isect.wo);
//     Rdir = 2.f * cos * isect.gn - isect.wo;
    
//     Ray specular(isect.p, Rdir);
//     specular.adjustOrigin(isect.gn);

//     bool intersected = scene->trace(specular, &s_isect);

//     RGB rcolor = shade(intersected, s_isect, depth+1);
//     color = f->Ks * rcolor;
//     return color;

    
// }

// RGB PathTracerShader::diffuseReflection (Intersection isect, Phong *f, int depth) {
//     RGB color(0.,0.,0.);
//     Vector dir;
//     float pdf;
    
//     // generate the specular ray
    
//     // actual direction distributed around N
//     // get 2 random number in [0,1[
//     float rnd[2];
//     rnd[0] = ((float)rand()) / ((float)RAND_MAX);
//     rnd[1] = ((float)rand()) / ((float)RAND_MAX);
        
//     Vector D_around_Z;
//     // cosine sampling
//     float cos_theta = D_around_Z.Z = sqrtf(rnd[1]);
//     D_around_Z.Y = sinf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
//     D_around_Z.X = cosf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);

//     pdf = cos_theta / M_PI;
        
//     // generate a coordinate system from N
//     Vector Rx, Ry;
//     isect.gn.CoordinateSystem(&Rx, &Ry);
        
//     dir = D_around_Z.Rotate  (Rx, Ry, isect.gn);
        
//     Ray diffuse(isect.p, dir);
        
//     diffuse.pix_x = isect.pix_x;
//     diffuse.pix_y = isect.pix_y;
        
//     diffuse.FaceID = isect.FaceID;

//     diffuse.adjustOrigin(isect.gn);

//     // OK, we have the ray : trace and shade it recursively
//     bool intersected;
//     Intersection d_isect;
//     // trace ray
//     intersected = scene->trace(diffuse, &d_isect);

//     if (!d_isect.isLight) {  // if light source return 0 ; handled by direct
//         // shade this intersection

//         RGB Rcolor = shade(intersected, d_isect, depth+1);
//         color = (f->Kd * Rcolor * cos_theta) / pdf;
        
//     }
//     return color;

// }


// RGB PathTracerShader::shade(bool intersected, Intersection isect, int depth) {
//     RGB color(0.,0.,0.);
    
//     // if no intersection, return background
//     if (!intersected) {
//         return (background);
//     }
    
//     if (isect.isLight) { // intersection with a light source
//         return isect.Le;
//     }
    
//     // get the BRDF
//     Phong *f = (Phong *)isect.f;
    
//     float rnd_russian = ((float)rand()) / ((float)RAND_MAX);

//     if (depth < MAX_DEPTH|| rnd_russian < continue_p) {
//         RGB temp_color(0.,0.,0.);
//         // Montecarlo sampling 
//         float s_p = f->Ks.Y() / (f->Kd.Y() + f->Ks.Y());
//         float rnd = ((float)rand()) / ((float)RAND_MAX);
//         if (rnd<=s_p || s_p >= (1.-EPSILON)) {
//             temp_color = specularReflection(isect, f, depth)/s_p;
//         }
//         else {
//             temp_color = diffuseReflection(isect, f, depth)/(1-s_p);
//         }
//         if (depth<MAX_DEPTH) color+=temp_color;
//         else color+=temp_color/(continue_p);
//     }

//     // if there is a diffuse component do direct light
//     if (!f->Kd.isZero()) {
//         color += directLighting(isect, f);
//     }
        
//     return color;
// };

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
    int l_ndx;
    const bool RANDOM_SAMPLE_ONE=true;
    float light_pdf;

    for (auto l_iter=scene->lights.begin() ; l_iter != scene->lights.end() ; l_iter++) {
        RGB this_l_color (0.,0.,0.);
        l = (Light *) (*l_iter);
        
        // if random sampling reaasign l
        if (RANDOM_SAMPLE_ONE) {
            // randomly select one light source
            l_ndx = rand() % scene->numLights;
            l = scene->lights[l_ndx];
            light_pdf = 1.f/((float)scene->numLights);
        }
        
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
                
                // get the position and radiance of the light source
                // get 2 random number in [0,1[
                float rnd[2];
                rnd[0] = ((float)rand()) / ((float)RAND_MAX);
                rnd[1] = ((float)rand()) / ((float)RAND_MAX);
                L = al->Sample_L(rnd, &lpoint, l_pdf);
                
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                
                // now normalize Ldir
                Ldir.normalize();
                
                // compute the cosine between Ldir  and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                
                // compute the cosine between Ldir  and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                
                // shade
                if (cosL>0. and cosL_LA<=0.)  { // light is NOT behind primitive AND light normal points to the ray o
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);
                    
                    shadow.pix_x = isect.pix_x;
                    shadow.pix_y = isect.pix_y;
                    
                    shadow.FaceID = isect.FaceID;
                    
                    // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
                    shadow.adjustOrigin(isect.gn);
                    
                    if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
                        this_l_color += (Kd * L * cosL) / l_pdf;
                    }
                } // end cosL > 0.
            }
        }  // end area light
        
        // if random sampling adjust the contribution and break the for loop
        if (RANDOM_SAMPLE_ONE) {
            color = this_l_color / light_pdf;
            break;
        } else {   // not random sampling, sum the individual contributions
            color += this_l_color;
        }
    } // for loop
    return color;
}

RGB PathTracerShader::specularReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    Vector Rdir, s_dir;
    Intersection s_isect;
    
    // generate the specular ray
    
    // IDEAL SPECULAR REFLECTION
    // direction R = 2 (N.V) N - V
    float cos = isect.gn.dot(isect.wo);
    Rdir = 2.f * cos * isect.gn - isect.wo;
    
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);

    bool intersected = scene->trace(specular, &s_isect);

    RGB rcolor = shade(intersected, s_isect, depth+1);
    color = f->Ks * rcolor;
    return color;
    
}

RGB PathTracerShader::diffuseReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    Vector dir;
    
    // generate the specular ray
    
    // actual direction distributed around N
    // get 2 random number in [0,1[
    float rnd[2];
    rnd[0] = ((float)rand()) / ((float)RAND_MAX);
    rnd[1] = ((float)rand()) / ((float)RAND_MAX);
        
    Vector D_around_Z;
    // cosine sampling
    float cos_theta = D_around_Z.Z = sqrtf(rnd[1]);
    D_around_Z.Y = sinf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    D_around_Z.X = cosf(2*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    float pdf = cos_theta / M_PI;

        
    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);
        
    dir = D_around_Z.Rotate  (Rx, Ry, isect.gn);
        
    Ray diffuse(isect.p, dir);
        
    diffuse.adjustOrigin(isect.gn);

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection d_isect;
    // trace ray
    intersected = scene->trace(diffuse, &d_isect);

    if (!d_isect.isLight) {  // if light source return 0 ; handled by direct
        // shade this intersection
        RGB Rcolor = shade(intersected, d_isect, depth+1);
        color = (f->Kd *  cos_theta * Rcolor) / pdf;
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
        RGB temp_color(0.,0.,0.);
        float s_p = f->Ks.Y() / (f->Kd.Y() + f->Ks.Y());
        float rnd = ((float)rand()) / ((float)RAND_MAX);
        if (rnd<=s_p || s_p >= (1.-EPSILON)) {
            temp_color = specularReflection(isect, f, depth)/(s_p);
        }
        else {
            temp_color = diffuseReflection(isect, f, depth)/(1-s_p);
        }
        if (depth<MAX_DEPTH) color+=temp_color;
        else color+=temp_color/(continue_p);
        // color+=temp_color;
    }

    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) {
        color += directLighting(isect, f);
    }
        
    return color;
};