//
//  AmbientShader.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef DistributedShader_hpp
#define DistributedShader_hpp

#include "shader.hpp"
#include "Phong.hpp"

class DistributedShader: public Shader {
    RGB background;
    RGB directLighting (Intersection isect, Phong *f);
    RGB specularReflection (Intersection isect, Phong *f, int depth);
public:
    DistributedShader (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* DistributedShader_hpp */
