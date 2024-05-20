#! /bin/sh
bear -- g++ -std=c++17 -g -Wall src/glad.c src/imgui/*.cpp src/*.cpp -o 3dco -I include -I src/imgui -L lib -lglfw -lSDL2 -lGL -ldl
