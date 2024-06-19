
#ifndef ImageJPG_hpp
#define ImageJPG_hpp
#include "image.hpp"

class ImageJPG: public Image {
    
    typedef struct {
        unsigned char val[3];  // r,g,b
    } JPG_Pixel;


    JPG_Pixel *imageToSave;
    void ToneMap ();
    void save_jpeg(JPG_Pixel* image, std::string filename, int quality);
    
public:
    ImageJPG(const int W, const int H):Image(W, H) {}
    void createTestImage();
    bool Save (std::string filename);
    RGB getPixel(int x, int y);
};

#endif /* ImagePPM_hpp */
