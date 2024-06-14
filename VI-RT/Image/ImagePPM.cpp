//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>  

void ImagePPM::ToneMap () {
    imageToSave = new PPM_pixel[W*H];
    
    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0 ; j< H ; j++) {
        for (int i = 0; i < W ; ++i) {
            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
        }
    }

}

bool ImagePPM::Save (std::string filename) {

    // Lets check if the image is empty
    if (W==0 or H==0) {
        std::cout << "ERROR: Image is empty\n";
        return false;
    }


    if (filename.substr(filename.length() - 4).compare(".ppm") != 0) {
        filename += ".ppm";
    }

    // convert from float to {0,1,..., 255}
    ToneMap();

    // Details and code on PPM files available at:
    // https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html
    
    std::ofstream ofs;
    try{
        ofs.open(filename, std::ios::out | std::ios::binary);
        if (ofs.fail()) throw "Can't open file";
        ofs << "P6\n" << W << " " << H << "\n255\n";
 
        for (int j = 0 ; j< H ; j++) {
            for (int i = 0; i < W ; ++i) {
                ofs << imageToSave[j*W+i].val[0];
                ofs << imageToSave[j*W+i].val[1];
                ofs << imageToSave[j*W+i].val[2];
            }
        }
        

        // ofs.write(reinterpret_cast<char*>(test), W*H*sizeof(PPM_pixel));
        ofs.close();
        std::cout << "Image saved to "<< filename <<"\n";
    }
    catch (std::ofstream::failure e) {
        std::cout << "Exception opening/reading file\n";
        return false;
    }

    return true;
}
