#pragma once

#include "image.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <single_include/nlohmann/json.hpp>
#include <future>
#include <random>

#include <stb_image.h>

namespace mapmaker{
    class MapMaker{
		public:
			MapMaker();
			~MapMaker();
			MapMaker(const MapMaker&) = delete;
			MapMaker& operator = (const MapMaker&) = delete;

			void run();

        private:
			std::vector<std::future<void>> async_pool{};
			struct{
				bool draw_outlines;
				bool draw_shadows;
				int light_radius;
				int light_direction;
				float darken_value;
				float shading_value;
				std::vector<int> light_offset;
			} prefs;
			int threads;
			std::unique_ptr<Image> img;
            std::unordered_map<std::string, std::vector<int>> color_map{};
            std::unordered_map<int, std::unordered_map<int, std::string>> block_map{};
            std::unordered_map<int, std::unordered_map<int, int>> height_map{};
			std::default_random_engine rnd_eng{std::random_device{}()};

			float rnd(float min, float max);
			void async_draw(int start, int end);
			void check_async_pool();
			void draw();
    };
}