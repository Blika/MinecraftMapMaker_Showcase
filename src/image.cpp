#include "image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace mapmaker{

	Image::Image(int iw, int ih): width{iw}, height{ih}{
        channels = 4; 
        data = (stbi_uc*)malloc(static_cast<size_t>(width) * static_cast<size_t>(height) * channels);
    }

	Image::~Image(){
        stbi_image_free(data);
    }

    std::string Image::get_png_data(){
        int len;
        unsigned char* d = stbi_write_png_to_mem((const unsigned char*) data, width * channels, width, height, channels, &len);
        std::string str_data(reinterpret_cast<const char*>(d), len);
        stbi_image_free(d);
        return str_data;
    }

    void Image::get_pixel(size_t x, size_t y, stbi_uc* r, stbi_uc* g, stbi_uc* b, stbi_uc* a){
        *r = data[4 * (y * width + x) + 0];
        *g = data[4 * (y * width + x) + 1];
        *b = data[4 * (y * width + x) + 2];
        *a = data[4 * (y * width + x) + 3];
    }

    void Image::write_pixel(size_t x, size_t y, stbi_uc r, stbi_uc g, stbi_uc b, stbi_uc a){
        data[4 * (y * width + x) + 0] = r;
        data[4 * (y * width + x) + 1] = g;
        data[4 * (y * width + x) + 2] = b;
        data[4 * (y * width + x) + 3] = a;
    }

    void Image::save_image(const char* output){
        stbi_write_png(output, width, height, channels, data, width * channels);
    }
}