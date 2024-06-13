#include "ImageEXR.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>  

#include "miniz.h"
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"


#include <vector>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>

using namespace std;

int ImageEXR::rgba2exr(int W, int H, std::string outfilename, RGBA_pixel rgbaImage[])
{

  int width, height;
  int n;


  EXRHeader header;
  InitEXRHeader(&header);

  EXRImage image;
  InitEXRImage(&image);

  image.num_channels = 3;

  std::vector<float> images[3];
  images[0].resize(W * H);
  images[1].resize(W * H);
  images[2].resize(W * H);


  for (int j = 0 ; j< H ; j++) {
    for (int i = 0; i < W ; ++i) {
        images[0][j*W+i]=imageToSave[j*W+i].val[0];
        images[1][j*W+i]=imageToSave[j*W+i].val[1];
        images[2][j*W+i]=imageToSave[j*W+i].val[2];
    }
  }

  float* image_ptr[3];
  image_ptr[0] = &(images[2].at(0)); // B
  image_ptr[1] = &(images[1].at(0)); // G
  image_ptr[2] = &(images[0].at(0)); // R

  image.images = (unsigned char**)image_ptr;
  image.width = W;
  image.height = H;

  header.num_channels = 3;
  header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels); 
  // Must be BGR(A) order, since most of EXR viewers expect this channel order.
  strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
  strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
  strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

  header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels); 
  header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
  for (int i = 0; i < header.num_channels; i++) {
    header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
    header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
  }

  const char* err;

  if (outfilename.substr(outfilename.length() - 4).compare(".exr") != 0) {
    outfilename += ".exr";
  }

  int ret = SaveEXRImageToFile(&image, &header, outfilename.c_str(), &err);
  if (ret != TINYEXR_SUCCESS) {
    fprintf(stderr, "Save EXR err: %s\n", err);
    return ret;
  }
  printf("Saved exr file. [ %s ] \n", outfilename.c_str());


  free(header.channels);
  free(header.pixel_types);
  free(header.requested_pixel_types);

  return 0;
}



void ImageEXR::ToneMap () {
    imageToSave = new RGBA_pixel[W*H];
    
    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0 ; j< H ; j++) {
        for (int i = 0; i < W ; ++i) {
            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
            imageToSave[j*W+i].val[3] = 255; //Alpha channel
        }
    }

}



bool ImageEXR::Save (std::string filename) {

    // Lets check if the image is empty
    if (W==0 or H==0) {
        std::cout << "ERROR: Image is empty\n";
        return false;
    }

    std::cout << "Saving image to " << filename << std::endl;

    // convert from float to {0,1,..., 255}
    ToneMap();

    // Details and code on PPM files available at:
    // https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html

    
    rgba2exr(W, H, filename, imageToSave);


    return true;
}
