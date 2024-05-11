#pragma once

#include <stb_image.h>
#include <string>

namespace mapmaker{
    class Image{
		public:
			Image(int width, int height);
			~Image();

            Image(const Image&) = delete;
            Image &operator=(const Image&) = delete;
            Image(Image&&) = delete;
            Image &operator=(Image&&) = delete;

            int get_width(){
                return width;
            }
            int get_height(){
                return height;
            }
            int get_channels(){
                return channels;
            }
            stbi_uc* get_data(){
                return data;
            }
            std::string get_png_data();
            void get_pixel(size_t x, size_t y, stbi_uc* r, stbi_uc* g, stbi_uc* b, stbi_uc* a);
            void write_pixel(size_t x, size_t y, stbi_uc r, stbi_uc g, stbi_uc b, stbi_uc a);
            void save_image(const char* output);

        private:
            int width,height,channels;
			stbi_uc* data;
    };
}