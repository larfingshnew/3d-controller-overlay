const char* vertex_shader_source1 = 
"#version 410 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "vertexColor = aColor;\n"
"}\0";

const char* fragment_shader_source1= 
"#version 410 core\n"
"in vec3 vertexColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = vec4(vertexColor, 1.0);\n"
"}\n";

const char* vertex_shader_source2 = 
"#version 410 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragment_shader_source2 = 
"#version 410 core\n"
"out vec4 color;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"    color = ourColor;\n"
"}\n";


