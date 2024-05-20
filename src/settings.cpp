#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <SDL2/SDL.h>
#include "settings.h"

char* base_path = SDL_GetBasePath();
std::filesystem::path file_path;
std::ofstream ofs;
std::ifstream ifs;	
	
void write_int(std::string label, int value){
    ofs << label.append("\n").c_str();
    ofs << std::to_string(value).append("\n").c_str();
}

void write_float(std::string label, float value){
    ofs << label.append("\n").c_str();
    ofs << std::to_string(value).append("\n").c_str();
}

void write_3_floats(std::string label, float value1, float value2, float value3){
    ofs << label.append("\n").c_str();
    ofs << std::to_string(value1).append("\n").c_str();
    ofs << std::to_string(value2).append("\n").c_str();
    ofs << std::to_string(value3).append("\n").c_str();
}

void write_string(std::string label, std::string value){
    label.append("\n");
    value.append("\n");
    ofs << label.c_str();
    ofs << value.c_str();
}

void write_line(std::string line){
    line.append("\n");
    ofs << line.c_str();
}

void open_ifstream(std::filesystem::path path){
    file_path = std::filesystem::path(base_path);
	std::filesystem::path sub_path(path);
    file_path /= sub_path;
	std::filesystem::create_directory(file_path.parent_path());

	ifs = std::ifstream(file_path);
    if (!ifs){
        std::cout << "Uh oh, file could not be opened for reading!" << std::endl;
    }
}

void open_ofstream(std::filesystem::path path){
    file_path = std::filesystem::path(base_path);
	std::filesystem::path sub_path(path);
    file_path /= sub_path;
	std::filesystem::create_directory(file_path.parent_path());

	ofs = std::ofstream(file_path);
    if (!ofs){
        std::cout << "Uh oh, file could not be opened for writing!" << std::endl;
    }
}

void read_file(std::vector<std::string> *lines){
    while (ifs){
        std::string line;
        std::getline(ifs, line);
        lines->push_back(line);
	}
}

void get_directory_contents(std::vector<std::filesystem::path> *files, std::string path){
    std::string dir_path = base_path;
    dir_path.append("/");
    dir_path.append(path);
    
    std::string p_str = dir_path;
    struct stat sb;
    if (stat(dir_path.c_str(), &sb) == 0){
        for (const auto & entry : std::filesystem::directory_iterator(dir_path))
            files->push_back(entry.path());
    }else{
        std::filesystem::create_directory(dir_path);
    }
}

void list_directory(std::string path){
    std::string dir_path = base_path;
    dir_path.append("/");
    dir_path.append(path);
    
    struct stat sb;
    if (stat(dir_path.c_str(), &sb) == 0){
        for (const auto & entry : std::filesystem::directory_iterator(dir_path))
            std::cout << entry.path() << std::endl;
    }else{
        std::filesystem::create_directory(dir_path);
    }
}

void clear_directory(std::string dir)
{
    std::string dir_path = base_path;
    dir_path.append("/");
    dir_path.append(dir);
    
    struct stat sb;
    if (stat(dir_path.c_str(), &sb) == 0){
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
            std::filesystem::remove_all(entry.path());
    }
}

void close_ifstream(){
    ifs.close();
}

void close_ofstream(){
    ofs.close();
}
