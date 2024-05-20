#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "shader.h"

typedef struct pos_struct{
    GLfloat x = 0;
    GLfloat y = 0;
    GLfloat z = 0;
}vertex_position;

typedef struct norm_struct{
    GLfloat x = 0;
    GLfloat y = 0;
    GLfloat z = 0;
}vertex_normal;

typedef struct texcoord_struct{
    GLfloat x = 0;
    GLfloat y = 0;
}vertex_texcoord;

typedef struct vertex_struct{
    int position = 0;
    int normal = 0;
    int texcoord = 0;
}Vertex;

typedef struct material_struct {
    float ambient = 0.2f;
    float diffuse = 1.0f;
    float specular = 0.1f;
    float shininess = 32.0f;
    float color[3] = {0.3f, 0.3f, 0.3f};
    float highlight[3] = {0.0f, 1.0f, 0.0f};
}Material; 

typedef struct face_struct{
    std::vector<int> indices;
}Face;

typedef struct texture_struct{
    GLuint id = 0;
    std::string name = "";
    std::string path;
    int type = 0;
    int wrapX = 0;
    int wrapY = 0;
    float offsetX = 0;
    float offsetY = 0;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rotation = 0.0f;
    float border[4] = {0.8f, 0.8f, 0.8f, 1.0f};
}Texture;

typedef struct mesh_struct {
    float position[3] = {0.0f, 0.0f, 0.0f};
    float travel[3] = {0.0f, 0.0f, 0.0f};
    float popup_offset[3] = {0.0f, 0.0f, 0.0f};
    float popup_rotation[3] = {0.0f, 0.0f, 0.0f};
    float trigger_max = 0.0f;
    float stick_max = 0.0f;
    float touch_width = 0.0f;
    float touch_height = 0.0f;
    
    float stick_X = 0.0f;
    float stick_Y = 0.0f;
    Uint8 touch_state = 0;
    float touch_X = 0.0f;
    float touch_Y = 0.0f;
    float pull = 0.0f;
    float press = 0.0f;
    
    float highlight_value = 0.0f;
    int ring_highlight_deadzone = 10;
    bool popup = false;
    bool visible = true;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    
    GLuint elements = 0;

    Material material;
    
    std::vector<Texture> textures;
}Mesh;

typedef struct model_struct {
    std::string path;
    std::vector<Mesh> meshes;
    glm::mat4 motion_matrix = glm::mat4(1.0f);
    bool popup_bumpers = false;
    bool popup_triggers = false;
    bool popup_paddles = false;
}Model;

bool isFloat(std::string myString);

void loadModel(Model &m, std::string path);

void loadMesh(Mesh &m, std::string path);

void readInfo(Model &m, std::string path);

void writeInfo(Model &m, std::string path);

void loadTexture(GLuint &id, std::string path);

void deleteTexture(GLuint &id);

void drawModel(Model m, GLuint shader);

void drawMesh(Mesh m, glm::mat4 motion, GLuint shader);

#endif

