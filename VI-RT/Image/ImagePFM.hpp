
#ifndef ImagePFM_hpp
#define ImagePFM_hpp
#include "image.hpp"

class ImagePFM: public Image {
    
    typedef struct {
        unsigned char val[3];  // r,g,b
    } PFM_pixel;


    PFM_pixel *imageToSave;
    void ToneMap ();
public:
    ImagePFM(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename);
};

#endif
