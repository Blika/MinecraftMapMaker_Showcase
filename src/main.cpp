#include "map_maker.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(){
	std::filesystem::create_directory("output");
	mapmaker::MapMaker mapmaker{};
	try{
		mapmaker.run();
	}catch(const std::exception& e){
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}