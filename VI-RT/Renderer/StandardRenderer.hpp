//
//  StandardRenderer.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef StandardRenderer_hpp
#define StandardRenderer_hpp

#include "renderer.hpp"

class StandardRenderer: public Renderer {
private:
    int spp;
    int jitter = 0;
    int parallel = 0;
    int interactiveOutput=0;
public:
    StandardRenderer (Camera *cam, Scene * scene, Image * img, Shader *shd, int _spp, int _jitter, int _parallel, int _interactiveOutput): Renderer(cam, scene, img, shd) {
        spp = _spp;
        jitter = _jitter;
        parallel = _parallel;
        interactiveOutput = _interactiveOutput;
    }
    int Render ();
    void paralelRender(int numThreads, int W, int H, int spp, int jitter);
    
};

#endif /* StandardRenderer_hpp */
