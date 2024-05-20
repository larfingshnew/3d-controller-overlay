#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

void write_int(std::string label, int value);

void write_float(std::string label, float value);

void write_3_floats(std::string label, float value1, float value2, float value3);

void write_string(std::string label, std::string s);

void write_line(std::string line);

void list_directory(std::string path);

void get_directory_contents(std::vector<std::filesystem::path> *files, std::string path);

void clear_directory(std::string dir);

void open_ifstream(std::filesystem::path path);

void open_ofstream(std::filesystem::path path);

void read_file(std::vector<std::string> *lines);

void close_ifstream();

void close_ofstream();

#endif