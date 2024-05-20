#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <string>
#include <fstream>
#include <filesystem>

GLuint CompileShader(GLuint type, const char* shaderSource);

GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

std::string GetShaderSource(std::string path);

void shaderUniformBool(GLuint ID, const char* name, bool value);

void shaderUniformInt(GLuint ID, const char* name, int value);   

void shaderUniformFloat(GLuint ID, const char* name, float value);

void shaderUniformMat4(GLuint ID, const char* name, glm::mat4 mat);

void shaderUniformMat3(GLuint ID, const char* name, glm::mat3 mat);

void shaderUniformVec3(GLuint ID, const char* name, glm::vec3 vec);

void shaderUniform2f(GLuint ID, const char* name, float value1, float value2);

void shaderUniform3f(GLuint ID, const char* name, float value1, float value2, float value3);

void shaderUniform4f(GLuint ID, const char* name, float value1, float value2, float value3, float value4);

#endif
