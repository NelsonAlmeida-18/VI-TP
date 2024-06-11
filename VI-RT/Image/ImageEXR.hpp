//
//  ImagePPM.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#ifndef ImageEXR_hpp
#define ImageEXR_hpp
#include "image.hpp"

class ImageEXR: public Image {
    
    typedef struct {
        unsigned char val[4];  // r,g,b
    } RGBA_pixel;


    RGBA_pixel *imageToSave;
    void ToneMap ();
public:
    ImageEXR(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename);
};

#endif /* ImagePPM_hpp */
