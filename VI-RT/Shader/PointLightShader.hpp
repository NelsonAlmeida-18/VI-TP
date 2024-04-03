

#ifndef PointLightShader_hpp
#define PointLightShader_hpp

#include "shader.hpp"

class PointLightShader: public Shader {
    RGB background;
    float shadowRayEpsilon = 0.0001;
public:
    PointLightShader (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* AmbientShader_hpp */
