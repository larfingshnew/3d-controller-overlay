#! /bin/sh
bear -- g++ -std=c++17 -g -Wall src/glad.c src/imgui/*.cpp src/*.cpp -o mac_prog -I include -I src/imgui -L lib -lglfw -lSDL2 -lassimp -framework OpenGL -ldl