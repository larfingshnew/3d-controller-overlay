#ifndef CONTROLLER_WINDOW_H
#define CONTROLLER_WINDOW_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <math.h>
#include <iostream>
#include <vector>
#include <string>

#include "settings_window.h"
#include "shader.h"
#include "model.h"

//typedefs
typedef struct direct_light_struct{
	std::string name = "Directional Light 1";
	glm::vec3 direction = glm::vec3(0.25f, -1.0f, 0.0f);
	float color[3] = {1.0f, 1.0f, 1.0f};
	float ambient = 0.4f;
	float diffuse = 0.8f;
	float specular = 1.0f;
}direct_light;

typedef struct point_light_struct{
	std::string name = "Point Light 1";
	glm::vec3 position = glm::vec3(0.0);
	float intensity = 0.5f;
	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
	float color[3] = {1.0f, 1.0f, 1.0f};
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	bool hide = false;
}point_light;

typedef struct spot_light_struct{
	std::string name = "Spot Light 1";
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 direction = glm::vec3(0.0, 0.0f, -1.0f);
	float yaw = 0.0f;
	float pitch = 0.0f;
	float cutoff = 20.0f;
	float outer_cutoff = 50.0f;
	float intensity = 0.5f;
	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
	float color[3] = {1.0f, 1.0f, 1.0f};
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	bool hide = false;
}spot_light;

typedef struct controller_window_struct{
	GLFWwindow* glfw_window;
	unsigned ID;

	SDL_GameController* sdl_controller = nullptr;
	std::string default_mapping;
				
	bool left_click = false;
	double left_click_x = 0;
	double left_click_y = 0;
	bool right_click = false;
	double right_click_x = 0;
	double right_click_y = 0;

	bool always_on_top = false;
	bool borderless = false;
	bool drag_to_move = false;
	bool scroll_to_resize = false;
	bool grid = false;
	int swap_interval = 1;
	bool wireframe = false;
	Uint8 frame_cap = 60;
	float bg_color[4] = {0.2f, 0.3f, 0.3f, 1.0f};
	float highlight_color[3] = {0.0f, 1.0f, 0.0f};
	bool freelook = false;

	double deltaTime = 0.0f;
	double lastTime = 0.0f;
	double lastFrame = 0.0f;

	float camera_distance = 3.5f;
	float camera_yaw = 0.0f;
	float camera_pitch = 89.999f;
	float camera_roll = 0.0f;
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);

	int move_speed = 5;
	int turn_speed = 5;
	int mouse_sens = 5;
	float freelook_yaw = 180.0f;
	float freelook_pitch = 0.0f;
	glm::vec3 freelook_position = glm::vec3(0.0f, 0.5f, 3.0f);
	glm::vec3 freelook_direction = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::mat4 gyro_matrix = glm::mat4(1.0f);
	bool gyro_toggled = false;
	bool gyro_enabled = false;
	float gyro_data[3] = {0.0f, 0.0f, 0.0f};
	Uint64 gyro_time = 0;
	int reset_gyro_button1 = -1;
	int reset_gyro_button2 = -1;
	int gyro_correction = 5;

	float accel_data[3] = {0.0f, 0.0f, 0.0f};
	Uint64 accel_time = 0;
	
	glm::mat4 view_matrix = glm::mat4(1.0f);
	glm::mat4 projection_matrix = glm::mat4(1.0f);
	
	GLuint grid_shader = 0;
	GLuint shader = 0;
	GLuint light_source_shader = 0;

	GLuint grid_vbo = 0;
	GLuint grid_vao = 0;
	GLuint grid_ibo = 0;
	GLuint grid_length = 0;

	std::vector<direct_light> direct_lights;
	std::vector<point_light> point_lights;
	std::vector<spot_light> spot_lights;
	
	GLuint lighting_vertex_data = 0;
	GLuint lighting_normal_data = 0;
	GLuint lighting_texture_data = 0;
	GLuint lighting_vao = 0;
	GLuint lighting_ebo = 0;

	std::string model_name = "";
	std::string mesh_name = "";
	Model model;
}controller_window;

void createControllerWindow(std::string title, std::string model_path);

void lightingSpecification(controller_window &w);

void createShader(GLuint &shader_id, const char* vs_source, const char* fs_source);

void update_camera(controller_window &w, GLuint &shader, int window_width, int window_height);

controller_window* getLastWindow();

controller_window* getControllerWindow(unsigned ID);

void controller_window_input();

void controller_sdl_events(SDL_Event* event);

void removeControllerWindow(unsigned ID);

void destroyWindows();

void make_grid(controller_window &w);

void drawControllerWindows();

void controller_framebuffer_size_callback(GLFWwindow* window, int width, int height);

void controller_window_size_callback(GLFWwindow* window, int width, int height);

void controller_window_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void controller_window_iconify_callback(GLFWwindow* window, int iconified);

#endif

