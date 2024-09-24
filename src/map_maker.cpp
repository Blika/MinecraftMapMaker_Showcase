#include "map_maker.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>
#include <single_include/nlohmann/json.hpp>
#include <stb_image.h>
namespace mapmaker{
    
    MapMaker::MapMaker(){
        nlohmann::json map;
        nlohmann::json blocks_src;
        nlohmann::json settings;
	    try{
            std::ifstream mf("input/map.json");
            map = nlohmann::json::parse(mf);
            std::ifstream bf("input/blocks.json");
            blocks_src = nlohmann::json::parse(bf);
            std::ifstream sf("input/settings.json");
            settings = nlohmann::json::parse(sf);
        }catch(const std::exception& e){
            throw std::runtime_error("failed to initialize data");
	    }

        threads = static_cast<int>(std::thread::hardware_concurrency());
        prefs.draw_outlines = settings["draw_outlines"].get<bool>();
        prefs.draw_shadows = settings["draw_shadows"].get<bool>();
        prefs.light_radius = settings["light_radius"].get<int>();
        prefs.light_direction = settings["light_direction"].get<int>();
        prefs.darken_value = settings["darken"].get<float>();
        prefs.shading_value = settings["shading"].get<float>();
        prefs.light_offset = settings["light_offset"].get<std::vector<int>>();

        img = std::make_unique<Image>(map["x"].get<int>(),map["z"].get<int>());

        for(auto& [k,v]: blocks_src.items()){
            int r,g,b;
            const char* hex = v.get<std::string>().c_str();
            std::sscanf(hex, "#%02x%02x%02x", &r, &g, &b);
            std::vector rgb{r,g,b};
            color_map[k] = rgb;
        }
        for(auto& [k,v]: map["data"].items()){
            int z = std::stoi(k);
            std::vector<std::string> blocks = v.get<std::vector<std::string>>();
            std::unordered_map<int, std::string> b_map{};
            std::unordered_map<int, int> h_map{};
            uint32_t vec_size = blocks.size();
            for(uint32_t x = 0; x < vec_size; x++){
                std::string bh = blocks[x];
                std::istringstream istream(bh);
                std::vector<std::string> bh_split{};
                std::string d;
                while(std::getline(istream, d, '|')){
                    bh_split.push_back(d);
                }
                b_map[x] = bh_split[0];
                int height = std::stoi(bh_split[1]);
                h_map[x] = height;
            }
            block_map[z] = b_map;
            height_map[z] = h_map;
        }
    }

    MapMaker::~MapMaker(){
    }

	float MapMaker::rnd(const float& min, const float& max){
		std::uniform_real_distribution<float> rnd_dist(min,max);
		return rnd_dist(rnd_eng);
	}

    void MapMaker::async_draw(const int& start, const int& end){
        for(int z = start; z < end; z++){
            for(auto& [x,block]: block_map[z]){
                std::vector<int> rgb{0,0,0,255};
                if(color_map.contains(block)){
                    rgb = color_map[block];
                    float darken = 1.f;
                    darken -= rnd(0.f, prefs.darken_value);
                    if(prefs.draw_outlines){
                        float outlineCurrentY = height_map[z][x];
                        float y1, y2;
                        switch(prefs.light_direction){
                            case 0:
                                if(z > 0 && x > 0){
                                    y1 = height_map[z-1][x];
                                    y2 = height_map[z][x-1];
                                    if(y1 > outlineCurrentY || y2 > outlineCurrentY){
                                        darken -= prefs.shading_value;
                                    }else if(y1 < outlineCurrentY || y2 < outlineCurrentY){
                                        darken += prefs.shading_value;
                                    }
                                }
                                break;
                            case 1:
                                if(z > 0 && x < img->get_width()){
                                    y1 = height_map[z-1][x];
                                    y2 = height_map[z][x+1];
                                    if(y1 > outlineCurrentY || y2 > outlineCurrentY){
                                        darken -= prefs.shading_value;
                                    }else if(y1 < outlineCurrentY || y2 < outlineCurrentY){
                                        darken += prefs.shading_value;
                                    }
                                }
                                break;
                            case 2:
                                if(z < img->get_height() && x > 0){
                                    y1 = height_map[z+1][x];
                                    y2 = height_map[z][x-1];
                                    if(y1 > outlineCurrentY || y2 > outlineCurrentY){
                                        darken -= prefs.shading_value;
                                    }else if(y1 < outlineCurrentY || y2 < outlineCurrentY){
                                        darken += prefs.shading_value;
                                    }
                                }
                                break;
                            case 3:
                                if(z < img->get_height() && x < img->get_width()){
                                    y1 = height_map[z+1][x];
                                    y2 = height_map[z][x+1];
                                    if(y1 > outlineCurrentY || y2 > outlineCurrentY){
                                        darken -= prefs.shading_value;
                                    }else if(y1 < outlineCurrentY || y2 < outlineCurrentY){
                                        darken += prefs.shading_value;
                                    }
                                }
                                break;
                        }
                    }
                    if(prefs.draw_shadows){
                        bool shading = true;
                        int max = prefs.light_radius * 2;
                        for(int sunX = 0; sunX <= max; sunX++){
                            for(int sunY = 0; sunY <= max; sunY++){ 
                                for(int sunZ = 0; sunZ <= max; sunZ++){
                                    int tempX, tempY, tempZ;
                                    float distX, distY, distZ, stepX, stepY, stepZ;
                                    float currentX = x;
                                    float currentY = height_map[z][x];
                                    float currentZ = z;
                                    if(prefs.light_offset[0] > 0){
                                        distX = prefs.light_offset[0] + sunX - prefs.light_radius;
                                    }else{
                                        distX = -prefs.light_offset[0] + sunX - prefs.light_radius;
                                    }
                                    if(distX < 0){
                                        distX *= -1.f;
                                    }
                                    if(prefs.light_offset[1] > 0){
                                        distY = prefs.light_offset[1] + sunY - prefs.light_radius;
                                    }else{
                                        distY = -prefs.light_offset[1] + sunY - prefs.light_radius;
                                    }
                                    if(distY < 0){
                                        distY *= -1.f;
                                    }
                                    if(prefs.light_offset[2] > 0){
                                        distZ = prefs.light_offset[2] + sunZ - prefs.light_radius;
                                    }else{
                                        distZ = -prefs.light_offset[2] + sunZ - prefs.light_radius;
                                    }
                                    if(distZ < 0){
                                        distZ *= -1.f;
                                    }
                                    if(distX > distY && distX > distZ){
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX = -1.f;
                                        }else{
                                            stepX = 1.f;
                                        }
                                        stepY = distY/distX;
                                        stepZ = distZ/distX;
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ *= -1.f;
                                        }
                                    }else if(distX == distY && distZ == distY){
                                        stepY = 1.f;
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX = -1.f;
                                        }else{
                                            stepX = 1.f;
                                        }
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ = -1.f;
                                        }else{
                                            stepZ = 1.f;
                                        }
                                    }else if(distZ == distY && distX != distY){
                                        stepY = 1.f;
                                        stepX = distX/distZ;
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX *= -1.f;
                                        }
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ = -1.f;
                                        }else{
                                            stepZ = 1.f;
                                        }
                                    }else if(distX == distZ && distY != distX){
                                        stepY = distY/distZ;
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX = -1.f;
                                        }else{
                                            stepX = 1.f;
                                        }
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ = -1.f;
                                        }else{
                                            stepZ = 1.f;
                                        }
                                    }else if(distZ > distX && distZ > distY){
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ = -1.f;
                                        }else{
                                            stepZ = 1.f;
                                        }
                                        stepY = distY/distZ;
                                        stepX = distX/distZ;
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX *= -1.f;
                                        }
                                    }else if(distY > distX && distY > distZ){
                                        stepY = 1.f;
                                        stepZ = distZ/distY;
                                        if(currentZ >= prefs.light_offset[2] + sunZ){
                                            stepZ *= -1.f;
                                        }
                                        stepX = distX/distY;
                                        if(currentX >= prefs.light_offset[0] + sunX){
                                            stepX *= -1.f;
                                        }
                                    }
                                    while(true){
                                        currentX += stepX;
                                        currentY += stepY;
                                        currentZ += stepZ;
                                        tempX = static_cast<int>(std::floor(currentX));
                                        tempY = static_cast<int>(std::floor(currentY));
                                        tempZ = static_cast<int>(std::floor(currentZ));
                                        if(tempX < 0 || tempX >= img->get_width() || tempZ < 0 || tempZ >= img->get_height() || (tempX - x >= distX && tempY - height_map[z][x] >= tempY && tempZ - z >= distZ)){
                                            sunZ = max + 1;
                                            sunX = max + 1;
                                            sunY = max + 1;
                                            shading = false;
                                            break;
                                        }
                                        if(height_map[tempZ][tempX] > tempY){
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        if(shading){
                            darken -= prefs.shading_value;
                        }
                    }
                    rgb[0] = static_cast<int>(std::floor(rgb[0] * darken));
                    rgb[1] = static_cast<int>(std::floor(rgb[1] * darken));
                    rgb[2] = static_cast<int>(std::floor(rgb[2] * darken));
                    rgb[3] = 255;
                }
                img->write_pixel(x,z,static_cast<unsigned char>(rgb[0]),static_cast<unsigned char>(rgb[1]),static_cast<unsigned char>(rgb[2]),static_cast<unsigned char>(rgb[3]));
            }
        }
    }

    void MapMaker::check_async_pool(){
        while(!async_pool.empty()){
            int size = async_pool.size();
            std::vector<int> finished{};
            for(int i = 0; i < size; i++){
                if(async_pool[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready){
                    finished.push_back(i);
                }
            }
            for(auto& i: finished){
                async_pool.erase(async_pool.begin() + i);
            }
        }
    }

    void MapMaker::run(){
        draw();
        img->save_image("output/map.png");
    }

    void MapMaker::draw(){
        int max = img->get_height();
        int left = max;
        int batch = static_cast<int>(std::ceil(max / threads));
        int start = -1;
        int end;
        while(left != 0){
            int currentBatch = batch;
            if(start == -1){
                start = 0;
            }else{
                start += batch;
            }
            if(left < currentBatch){
                currentBatch = left;
                left = 0;
            }else{
                left -= currentBatch;
            }
            end = start + currentBatch;
            std::future<void> futa = std::async(std::launch::async, std::bind(&MapMaker::async_draw, this, start, end));
            async_pool.push_back(std::move(futa));
        }
        check_async_pool();
    }
}