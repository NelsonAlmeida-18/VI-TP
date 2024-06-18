#include "ImagePFM.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>  

void ImagePFM::ToneMap () {
    imageToSave = new PFM_pixel[W*H];
    
    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0 ; j< H ; j++) {
        for (int i = 0; i < W ; ++i) {
            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
        }
    }

}

bool ImagePFM::Save (std::string filename) {

    // Lets check if the image is empty
    if (W==0 or H==0) {
        std::cout << "ERROR: Image is empty\n";
        return false;
    }


    if (filename.substr(filename.length() - 4).compare(".pfm") != 0) {
        filename += ".pfm";
    }

    std::ofstream ofs;
    try{
        ofs.open(filename, std::ios::out | std::ios::binary);
        if (ofs.fail()) throw "Can't open file";
        ofs << "PF\n" << W << " " << H << "\n-1.0\n";
 
        for (int j = H-1 ; j>= 0 ; --j) {
            for (int i = 0; i < W ; ++i) {
                RGB color = imagePlane[j*W+i];
                ofs.write(reinterpret_cast<char*>(&color.R), sizeof(float));
                ofs.write(reinterpret_cast<char*>(&color.G), sizeof(float));
                ofs.write(reinterpret_cast<char*>(&color.B), sizeof(float));
            }
        }
        
        ofs.close();
        std::cout << "Image saved to "<< filename <<"\n";
    }
    catch (std::ofstream::failure e) {
        std::cout << "Exception opening/reading file\n";
        return false;
    }

    return true;
}
