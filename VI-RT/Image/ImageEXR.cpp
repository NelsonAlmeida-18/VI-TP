#include "ImageEXR.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>

using namespace std;

int ImageEXR::rgba2exr(int W, int H, std::string outfilename, RGBA_pixel rgbaImage[]) {
    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = 4; // 4 channels: R, G, B, A

    std::vector<float> images[4];
    images[0].resize(W * H);
    images[1].resize(W * H);
    images[2].resize(W * H);
    images[3].resize(W * H); // Alpha channel

    // std::cout << "Here "<< W << " " << H << std::endl;
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            // std::cout << i << " " << j << std::endl;
            images[0][j * W + i] = imagePlane[j * W + i].R;
            images[1][j * W + i] = imagePlane[j * W + i].G;
            images[2][j * W + i] = imagePlane[j * W + i].B;
            images[3][j * W + i] = 255;
            // images[1][j * W + i] = rgbaImage[j * W + i].val[1];
            // images[2][j * W + i] = rgbaImage[j * W + i].val[2];
            // images[3][j * W + i] = 255;
        }
    }
    // std::cout << "Here" << std::endl;
    float* image_ptr[4];
    image_ptr[0] = &(images[2].at(0)); // B
    image_ptr[1] = &(images[1].at(0)); // G
    image_ptr[2] = &(images[0].at(0)); // R
    image_ptr[3] = &(images[3].at(0)); // A

    image.images = (unsigned char**)image_ptr;
    image.width = W;
    image.height = H;

    
    header.num_channels = 4;
    header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);

    strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
    strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
    strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';
    strncpy(header.channels[3].name, "A", 255); header.channels[3].name[strlen("A")] = '\0';

    header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // keep pixel type as float in output image
    }

    const char* err = nullptr;

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

bool ImageEXR::Save(std::string filename) {
    if (W == 0 || H == 0) {
        std::cout << "ERROR: Image is empty\n";
        return false;
    }

    return rgba2exr(W, H, filename, imageToSave) == 0;
}
