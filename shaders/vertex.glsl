#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat3 normal_model;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float uv_rotation;
float uniformAngle = 0.5f;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = normal_model * aNormal; 
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
} 