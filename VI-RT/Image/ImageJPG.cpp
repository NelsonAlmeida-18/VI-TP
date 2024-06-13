#include "ImageJPG.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "jpeglib.h" 

// Define the JPEG header structure
struct JPEGHeader {
    unsigned char signature[2];
    unsigned char app0[2];
    unsigned char length[2];
    unsigned char identifier[5];
    unsigned char version[2];
    unsigned char units;
    unsigned char xdensity[2];
    unsigned char ydensity[2];
    unsigned char xthumbnail;
    unsigned char ythumbnail;
};

void ImageJPG::ToneMap() {
    imageToSave = new JPG_Pixel[W * H];
    
    // Loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            imageToSave[j * W + i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].R) * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].G) * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].B) * 255);
        }
    }
}

void rgb_to_ycbcr(unsigned char *rgb, unsigned char *ycbcr, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        int r = rgb[i * 3 + 0];
        int g = rgb[i * 3 + 1];
        int b = rgb[i * 3 + 2];
        ycbcr[i * 3 + 0] = (unsigned char)((0.299 * r + 0.587 * g + 0.114 * b) + 0.5);
        ycbcr[i * 3 + 1] = (unsigned char)((-0.1687 * r - 0.3313 * g + 0.5 * b) + 128);
        ycbcr[i * 3 + 2] = (unsigned char)((0.5 * r - 0.4187 * g - 0.0813 * b) + 128);
    }
}

void chroma_subsampling(unsigned char *ycbcr, int width, int height) {
    for (int i = 0; i < width * height; i += 2) {
        ycbcr[i * 3 + 1] = (ycbcr[i * 3 + 1] + ycbcr[(i + 1) * 3 + 1]) / 2;
        ycbcr[i * 3 + 2] = (ycbcr[i * 3 + 2] + ycbcr[(i + 1) * 3 + 2]) / 2;
    }
}

void dct(unsigned char *block, int width, int height) {
    std::vector<double> dct_block(width * height);
    for (int u = 0; u < width; u++) {
        for (int v = 0; v < height; v++) {
            double sum = 0;
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    sum += block[x * width + y] * cos((2 * x + 1) * u * M_PI / (2 * width)) * cos((2 * y + 1) * v * M_PI / (2 * height));
                }
            }
            double cu = (u == 0) ? sqrt(1.0 / width) : sqrt(2.0 / width);
            double cv = (v == 0) ? sqrt(1.0 / height) : sqrt(2.0 / height);
            dct_block[u * width + v] = cu * cv * sum;
        }
    }
    for (int i = 0; i < width * height; i++) {
        block[i] = (unsigned char)dct_block[i];
    }
}

void quantize(unsigned char *block, int width, int height, float quantization_factor) {
    unsigned char quantization_table[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    };

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            block[i * width + j] = (unsigned char)(block[i * width + j] / (quantization_table[i][j] * quantization_factor));
        }
    }
}

void zigzag(unsigned char *block, int width, int height) {
    unsigned char temp[64];
    int index = 0;
    for (int i = 0; i < 2 * (width - 1); i++) {
        if (i % 2 == 0) {
            for (int j = 0; j <= i; j++) {
                if (j < width && (i - j) < height) {
                    temp[index++] = block[j * width + (i - j)];
                }
            }
        } else {
            for (int j = 0; j <= i; j++) {
                if (j < width && (i - j) < height) {
                    temp[index++] = block[(i - j) * width + j];
                }
            }
        }
    }
    for (int i = 0; i < width * height; i++) {
        block[i] = temp[i];
    }
}

void rle(unsigned char *block, int width, int height, unsigned char *encoded, int &encoded_length) {
    int count = 0;
    unsigned char prev = block[0];
    int encoded_index = 0;

    for (int i = 0; i < width * height; i++) {
        if (block[i] == prev) {
            count++;
        } else {
            encoded[encoded_index++] = count;
            encoded[encoded_index++] = prev;
            prev = block[i];
            count = 1;
        }
    }

    encoded[encoded_index++] = count;
    encoded[encoded_index++] = prev;
    encoded_length = encoded_index;
}

void huffman(unsigned char *encoded, int encoded_length, unsigned char *huffman_encoded, int &huffman_length) {
    for (int i = 0; i < encoded_length; i++) {
        huffman_encoded[i] = encoded[i];
    }
    huffman_length = encoded_length;
}

// void ImageJPG::save_jpeg(JPG_Pixel* image, std::string filename, float quantization_factor) {
//     std::ofstream file(filename, std::ios::binary);

//     JPEGHeader header = {
//         {0xFF, 0xD8}, // SOI marker
//         {0xFF, 0xE0}, // APP0 marker
//         {0x00, 0x10}, // Length
//         {'J', 'F', 'I', 'F', 0x00}, // Identifier
//         {0x01, 0x01}, // Version
//         0x00, // Units
//         {0x00, 0x01}, // X density
//         {0x00, 0x01}, // Y density
//         0x00, // X thumbnail
//         0x00 // Y thumbnail
//     };

//     file.write(reinterpret_cast<char*>(&header), sizeof(JPEGHeader));

//     unsigned char *ycbcr = new unsigned char[W * H * 3];
//     rgb_to_ycbcr(reinterpret_cast<unsigned char*>(image), ycbcr, W, H);
//     chroma_subsampling(ycbcr, W, H);

//     for (int i = 0; i < H; i += 8) {
//         for (int j = 0; j < W; j += 8) {
//             unsigned char block[64];
//             for (int k = 0; k < 8; k++) {
//                 for (int l = 0; l < 8; l++) {
//                     block[k * 8 + l] = ycbcr[((i + k) * W + (j + l)) * 3];
//                 }
//             }
//             dct(block, 8, 8);
//             quantize(block, 8, 8, quantization_factor);
//             zigzag(block, 8, 8);

//             unsigned char encoded[128];
//             int encoded_length;
//             rle(block, 8, 8, encoded, encoded_length);

//             unsigned char huffman_encoded[128];
//             int huffman_length;
//             huffman(encoded, encoded_length, huffman_encoded, huffman_length);

//             file.write(reinterpret_cast<char*>(huffman_encoded), huffman_length);
//         }
//     }

//     file.close();
//     delete[] ycbcr;
// }


//SRC: https://github.com/mozilla/mozjpeg
void ImageJPG::save_jpeg(JPG_Pixel* image, std::string filename, int quality=98) {

  // Open file for writing
    FILE *outfile;
    if (filename.substr(filename.length() - 4).compare(".jpg") != 0) {
        filename += ".jpg";
    }
    if ((outfile = fopen((char*)filename.c_str(), "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", (char*)filename.c_str());
    exit(1);
    }

    // Initialize JPEG compression object
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Set output file
    jpeg_stdio_dest(&cinfo, outfile);


    // Set image parameters
    cinfo.image_width = W;
    cinfo.image_height = H;
    cinfo.input_components = 3; // Assuming RGB input
    cinfo.in_color_space = JCS_RGB; // RGB color space

    jpeg_set_defaults(&cinfo);

    // Set quality
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Convert RGB data to format expected by MozJPEG
    JSAMPROW row_ptr[H];
    for (int y = 0; y < H; y++) {
        row_ptr[y] = (JSAMPROW) &image[y * W];
    }

    // Write image data to JPEG file
    jpeg_write_scanlines(&cinfo, row_ptr, H);

    // Finish compression
    jpeg_finish_compress(&cinfo);

    // Clean up
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    std::cout << "Image saved to " << filename << " with quality: " << quality << std::endl;
}


bool ImageJPG::Save(std::string filename) {
    if (W == 0 || H == 0) {
        std::cout << "ERROR: Image is empty\n";
        return false;
    }

    std::cout << "Saving image to " << filename << std::endl;
    ToneMap();

    save_jpeg(imageToSave, filename); // Use quantization factor of 1.0 as default

    delete[] imageToSave;
    return true;
}



void ImageJPG::createTestImage() {

    // Create a gradient pattern for the image
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; i++) {
            float r = (float)i / (float)(W - 1);
            float g = (float)j / (float)(H - 1);
            float b = 0.5f;

            RGB rgb(r, g, b);
            set(i, j, rgb);
        }
    }
}

RGB ImageJPG::getPixel(int x, int y) {
    if (x > W || y > H) {
        return RGB(0, 0, 0);
    }
    return imagePlane[y * W + x];
}