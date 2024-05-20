#include "controller_window.h"

#include "cube_info.h"
#include "shaders.h"

std::string button_names[21] = {
    "south button",
    "east button",
    "west button",
    "north button",
    "back button",
    "guide button",
    "start button",
    "left cap",
    "right cap",
    "left bumper",
    "right bumper",
    "d-pad up",
    "d-pad down",
    "d-pad left",
    "d-pad right",
    "misc",
    "paddle 1",
    "paddle 2",
    "paddle 3",
    "paddle 4",
    "touchpad"
};

float grid_vertices[] = {
    -1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,
	   
     1.0f,  0.0f,  0.0f,  
     0.0f,  0.0f,  1.0f,  
     
	 0.0f,  0.0f,  1.0f,  
     1.0f,  0.0f,  0.0f,  
     
	-1.0f,  0.0f,  1.0f,  
     1.0f,  0.0f,  1.0f
};

unsigned int defaultWidth = 640;
unsigned int defaultHeight = 480;

std::vector<controller_window> windows;

void createControllerWindow(std::string title, std::string model_path){
    controller_window w;
	
	w.glfw_window = glfwCreateWindow(defaultWidth, defaultHeight, title.c_str(), NULL, NULL);
    if (w.glfw_window == NULL){
        std::cout << "Failed to create controller indow" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(w.glfw_window);

	GLFWimage images[1]; 
	images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, 0, 4);
	if (images[0].pixels == NULL) {
		std::cout << "couldn't load controller windown icon" << std::endl;
	}else{
		glfwSetWindowIcon(w.glfw_window, 1, images); 
	}
	stbi_image_free(images[0].pixels);

	//glfwSetFramebufferSizeCallback(w.glfw_window, controller_framebuffer_size_callback);
	//glfwSetWindowSizeCallback(w.glfw_window, controller_window_size_callback);
	glfwSetScrollCallback(w.glfw_window, controller_window_scroll_callback);
	
	//GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* vid_mode = glfwGetVideoMode(primary_monitor);
	//w.frame_cap = vid_mode->refreshRate;

	w.lastFrame = glfwGetTime();

	make_grid(w);

	lightingSpecification(w);

	//* shaders from header file
	createShader(w.shader, 
				 vertex_shader_code.c_str(),
				 fragment_shader_code.c_str());
	createShader(w.grid_shader, 
				 grid_vertex_shader_code.c_str(), 
				 grid_fragment_shader_code.c_str());
	createShader(w.light_source_shader, 
				 light_source_vertex_shader_code.c_str(),
				 light_source_fragment_shader_code.c_str());
	//*/
	/* shaders from text files
	createShader(w.shader, 
				 GetShaderSource("shaders/vertex.glsl").c_str(), 
				 GetShaderSource("shaders/fragment.glsl").c_str());
	createShader(w.grid_shader, 
				 GetShaderSource("shaders/grid_vertex.glsl").c_str(), 
				 GetShaderSource("shaders/grid_fragment.glsl").c_str());
	createShader(w.light_source_shader, 
				 GetShaderSource("shaders/light_source_vertex.glsl").c_str(),
				 GetShaderSource("shaders/light_source_fragment.glsl").c_str());
	//*/

	direct_light d;
	w.direct_lights.push_back(d);
	
	loadModel(w.model, model_path);
	w.model_name = get_top_folder(model_path);
		
	w.sdl_controller = SDL_GameControllerOpen(0);
	if(w.sdl_controller != NULL){
		w.default_mapping = SDL_GameControllerMapping(w.sdl_controller);
		if (SDL_GameControllerHasSensor(w.sdl_controller, SDL_SENSOR_GYRO)){
			SDL_GameControllerSetSensorEnabled(w.sdl_controller, SDL_SENSOR_GYRO, (SDL_bool)SDL_TRUE);
		}
		w.gyro_matrix = glm::mat4(1.0f);
	}else{
		std::cout << "couldn't open sdl controller." << std::endl;
		std::cout << SDL_GetError() << std::endl;
	}
	
	windows.push_back(w);
}

void lightingSpecification(controller_window &w){
	glGenVertexArrays(1, &w.lighting_vao);
	glGenBuffers(1, &w.lighting_vertex_data);
	glGenBuffers(1, &w.lighting_normal_data);
	glGenBuffers(1, &w.lighting_texture_data);

	glBindVertexArray(w.lighting_vao);

	glBindBuffer(GL_ARRAY_BUFFER, w.lighting_vertex_data);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(cube_vertices) * sizeof(GLfloat),
                 cube_vertices,
                 GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, w.lighting_normal_data);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(cube_normals) * sizeof(GLfloat),
                 cube_normals,
                 GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, w.lighting_texture_data);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(cube_tex_coords) * sizeof(GLfloat),
                 cube_tex_coords,
                 GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void createShader(GLuint &shader_id, const char* vs_source, const char* fs_source){
	shader_id = CreateShaderProgram(vs_source, fs_source);
}

void controller_framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void controller_window_size_callback(GLFWwindow* window, int width, int height){
	
}

void controller_window_scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	for(unsigned i = 0; i<windows.size(); ++i){
		if(windows[i].glfw_window == window && windows[i].scroll_to_resize){
			int w = 0;
			int h = 0;
			glfwGetWindowSize(window, &w, &h);
			if(yoffset > 0){
				w *= 1.05f;	h *= 1.05f;
				if (w > get_vid_mode()->width)
					w = get_vid_mode()->width;
				if (h > get_vid_mode()->height)
					h = get_vid_mode()->height;
			}else if(yoffset < 0){
				w *= 0.95f;	h *= 0.95f;
				if (w < 10)
					w = 10;
				if (h < 10)
					h = 10;
			}
			glfwSetWindowSize(window, w, h);
			break;
		}
	}
}

controller_window* getLastWindow(){
	return &windows.back();
}

controller_window* getControllerWindow(unsigned ID){
	for(unsigned i=0; i<windows.size(); ++i){
		if(windows[i].ID == ID){
			return &windows[i];
		}
	}
	return nullptr;
}

void controller_window_input(){
	SDL_PumpEvents();
	for(unsigned i = 0; i<windows.size(); ++i){
		//GYROSCOPE
		//if (SDL_GameControllerHasSensor(windows[i].sdl_controller, SDL_SENSOR_GYRO) && SDL_GameControllerIsSensorEnabled(windows[i].sdl_controller, SDL_SENSOR_GYRO)){
		if (windows[i].gyro_enabled){
			Uint64 timestamp;
			int error = SDL_GameControllerGetSensorDataWithTimestamp(windows[i].sdl_controller, SDL_SENSOR_GYRO, &timestamp, windows[i].gyro_data, 3);
			if (!error && windows[i].gyro_data[0] + windows[i].gyro_data[1] + windows[i].gyro_data[2] != 0){
				if (windows[i].gyro_toggled){
					windows[i].gyro_time = timestamp;
					windows[i].gyro_toggled = false;
				}else{
        			windows[i].gyro_matrix = glm::rotate(windows[i].gyro_matrix, windows[i].gyro_data[0] * (timestamp - windows[i].gyro_time) * 0.000001f, glm::vec3(1.0f, 0.0f, 0.0f));
					windows[i].gyro_matrix = glm::rotate(windows[i].gyro_matrix, windows[i].gyro_data[1] * (timestamp - windows[i].gyro_time) * 0.000001f, glm::vec3(0.0f, 1.0f, 0.0f));
        			windows[i].gyro_matrix = glm::rotate(windows[i].gyro_matrix, windows[i].gyro_data[2] * (timestamp - windows[i].gyro_time) * 0.000001f, glm::vec3(0.0f, 0.0f, 1.0f));
					windows[i].gyro_time = timestamp;

					//GYRO CORRECTION
					glm::vec3 controller_up = glm::vec3(0.0f, 1.0f, 0.0f) * glm::mat3(windows[i].gyro_matrix);
					glm::vec3 up_error_axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(controller_up));
					windows[i].gyro_matrix = glm::rotate(windows[i].gyro_matrix, windows[i].gyro_correction * 0.0001f, up_error_axis);
					
					glm::vec3 controller_right = glm::vec3(1.0f, 0.0f, 0.0f) * glm::mat3(windows[i].gyro_matrix);
					glm::vec3 right_error_axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), controller_right);
					windows[i].gyro_matrix = glm::rotate(windows[i].gyro_matrix, windows[i].gyro_correction * 0.0001f, right_error_axis);
					
					//RESET GYRO BUTTON COMBO
					if (windows[i].reset_gyro_button1 > -1 && windows[i].reset_gyro_button2 > -1){
						if(SDL_GameControllerGetButton(windows[i].sdl_controller, (SDL_GameControllerButton)windows[i].reset_gyro_button1) &&
						   SDL_GameControllerGetButton(windows[i].sdl_controller, (SDL_GameControllerButton)windows[i].reset_gyro_button2)){
							windows[i].gyro_matrix = glm::mat4(1.0f);
						}
					}
				}
        	}
		}
		//ACCELOROMETER
		if (SDL_GameControllerHasSensor(windows[i].sdl_controller, SDL_SENSOR_ACCEL) && SDL_GameControllerIsSensorEnabled(windows[i].sdl_controller, SDL_SENSOR_ACCEL)){
			//std::cout << "controller has accelerometer." << std::endl;
		}
		//LEFT STICK
		windows[i].model.meshes[5].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTX);
		windows[i].model.meshes[5].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTY);
		windows[i].model.meshes[7].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTX);
		windows[i].model.meshes[7].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTY);
		windows[i].model.meshes[16].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTX);
		windows[i].model.meshes[16].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_LEFTY);
		if (abs(windows[i].model.meshes[7].stick_X / 32767) > windows[i].model.meshes[7].ring_highlight_deadzone * 0.01 || 
			abs(windows[i].model.meshes[7].stick_Y / 32767) > windows[i].model.meshes[7].ring_highlight_deadzone * 0.01){
			windows[i].model.meshes[7].highlight_value = std::max(abs(windows[i].model.meshes[7].stick_X / 32767), abs(windows[i].model.meshes[7].stick_Y / 32767)) * 1.2f;
		}else{
			windows[i].model.meshes[7].highlight_value = 0.0f;
		}
		//RIGHT STICK
		windows[i].model.meshes[6].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTX);
		windows[i].model.meshes[6].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTY);
		windows[i].model.meshes[8].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTX);
		windows[i].model.meshes[8].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTY);
		windows[i].model.meshes[17].stick_X = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTX);
		windows[i].model.meshes[17].stick_Y = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_RIGHTY);
		if (abs(windows[i].model.meshes[8].stick_X / 32767) > windows[i].model.meshes[8].ring_highlight_deadzone * 0.01 || 
			abs(windows[i].model.meshes[8].stick_Y / 32767) > windows[i].model.meshes[8].ring_highlight_deadzone * 0.01){
			windows[i].model.meshes[8].highlight_value = std::max(abs(windows[i].model.meshes[8].stick_X / 32767), abs(windows[i].model.meshes[8].stick_Y / 32767)) * 1.2f;
		}else{
			windows[i].model.meshes[8].highlight_value = 0.0f;
		}
		//LEFT TRIGGER
		windows[i].model.meshes[3].pull = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		windows[i].model.meshes[3].highlight_value = windows[i].model.meshes[3].pull / 32767;
		windows[i].model.meshes[3].press = windows[i].model.meshes[3].highlight_value;
		//RIGHT TRIGGER
		windows[i].model.meshes[4].pull = SDL_GameControllerGetAxis(windows[i].sdl_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
		windows[i].model.meshes[4].highlight_value = windows[i].model.meshes[4].pull / 32767;
		windows[i].model.meshes[4].press = windows[i].model.meshes[4].highlight_value;
		//BUTTONS
		for (int b=0; b<21; b++){
			if(SDL_GameControllerGetButton(windows[i].sdl_controller, (SDL_GameControllerButton)b)){
				windows[i].model.meshes[9 + b].press = 1.0f;
				windows[i].model.meshes[9 + b].highlight_value = 1.0f;
			}else{
				windows[i].model.meshes[9 + b].press = 0.0f;
				windows[i].model.meshes[9 + b].highlight_value = 0.0f;
			}
		}
		//TOUCH POINTS
		int touch_pads = SDL_GameControllerGetNumTouchpads(windows[i].sdl_controller);
		if(touch_pads > 0){
			SDL_GameControllerGetTouchpadFinger(windows[i].sdl_controller, 
												0, 
												0, 
												&windows[i].model.meshes[30].touch_state, 
												&windows[i].model.meshes[30].touch_X, 
												&windows[i].model.meshes[30].touch_Y, 
												nullptr);
			if(windows[i].model.meshes[30].touch_state > 0){
				if(windows[i].model.meshes[29].press){
					windows[i].model.meshes[30].highlight_value = 0;
				}else{
					windows[i].model.meshes[30].highlight_value = 1;
				}
				windows[i].model.meshes[30].visible = true;
			}else{
				windows[i].model.meshes[30].highlight_value = 0;
				windows[i].model.meshes[30].visible = false;
			}
			SDL_GameControllerGetTouchpadFinger(windows[i].sdl_controller, 
												0, 
												1, 
												&windows[i].model.meshes[31].touch_state, 
												&windows[i].model.meshes[31].touch_X, 
												&windows[i].model.meshes[31].touch_Y, 
												nullptr);
			if(windows[i].model.meshes[31].touch_state > 0){
				if(windows[i].model.meshes[29].press){
					windows[i].model.meshes[31].highlight_value = 0;
				}else{
					windows[i].model.meshes[31].highlight_value = 1;
				}windows[i].model.meshes[31].visible = true;
			}else{
				windows[i].model.meshes[31].highlight_value = 0;
				windows[i].model.meshes[31].visible = false;
			}
		}
	}

	for(unsigned i = 0; i<windows.size(); ++i){
		if(glfwGetMouseButton(windows[i].glfw_window, GLFW_MOUSE_BUTTON_1)){
			if(windows[i].drag_to_move){
				int x = 0;
				int y = 0;
				glfwGetWindowPos(windows[i].glfw_window, &x, &y);

				double wx = 0;
				double wy = 0;
				glfwGetCursorPos(windows[i].glfw_window, &wx, &wy);

				if(windows[i].left_click == false){
					windows[i].left_click_x = wx;
					windows[i].left_click_y = wy;
				}
				windows[i].left_click = true;

				int cx = x + wx;
				int cy = y + wy;

				glfwSetWindowPos(windows[i].glfw_window, cx - windows[i].left_click_x, cy - windows[i].left_click_y);
			}
		}else{
			if(windows[i].left_click){
				windows[i].left_click = false;
			}
		}

		if(glfwGetMouseButton(windows[i].glfw_window, GLFW_MOUSE_BUTTON_2)){
			double x = 0;
			double y = 0;
			glfwGetCursorPos(windows[i].glfw_window, &x, &y);
			
			if(windows[i].right_click == false){
				windows[i].right_click_x = x;
				windows[i].right_click_y = y;
			}
			windows[i].right_click = true;
			
			double dx = windows[i].right_click_x - x;
			double dy = windows[i].right_click_y - y;

			windows[i].right_click_x = x;
			windows[i].right_click_y = y;

			if(windows[i].freelook){
				windows[i].freelook_yaw += (float)(dx * windows[i].turn_speed * 0.02f);
				windows[i].freelook_pitch += (float)(dy * windows[i].turn_speed * 0.02f);
				if(windows[i].freelook_pitch > 90)
					windows[i].freelook_pitch = 89.999f;
				if(windows[i].freelook_pitch < -90)
					windows[i].freelook_pitch = -89.999f;
			}
		}else{
			if(windows[i].right_click){
				windows[i].right_click = false;
			}
		}
		
		if(glfwWindowShouldClose(windows[i].glfw_window))
			close_window(windows[i].ID);
		
		if (windows[i].freelook){
			const float move_speed = windows[i].move_speed * 0.5f * windows[i].deltaTime;
			const float turn_speed = windows[i].turn_speed * 10 * windows[i].deltaTime;

			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 forward = windows[i].freelook_direction;
			forward.y = 0;
			forward = glm::normalize(forward);

			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_W) == GLFW_PRESS)
				windows[i].freelook_position += forward * move_speed;
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_S) == GLFW_PRESS)
				windows[i].freelook_position -= forward * move_speed;
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_A) == GLFW_PRESS)
				windows[i].freelook_position -= glm::normalize(glm::cross(forward, up)) * move_speed;
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_D) == GLFW_PRESS)
				windows[i].freelook_position += glm::normalize(glm::cross(forward, up)) * move_speed;
			
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_SPACE) == GLFW_PRESS)
				windows[i].freelook_position += up * move_speed;
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				windows[i].freelook_position -= up * move_speed;

			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_LEFT) == GLFW_PRESS){
				windows[i].freelook_yaw += turn_speed;
			}
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_RIGHT) == GLFW_PRESS){
				windows[i].freelook_yaw -= turn_speed;
			}

			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_UP) == GLFW_PRESS){
				windows[i].freelook_pitch += turn_speed;
				if(windows[i].freelook_pitch > 90)
					windows[i].freelook_pitch = 89.999f;
			}
			if (glfwGetKey(windows[i].glfw_window, GLFW_KEY_DOWN) == GLFW_PRESS){
				windows[i].freelook_pitch -= turn_speed;
				if(windows[i].freelook_pitch < -90)
					windows[i].freelook_pitch = -89.999f;
			}
		}
	}
}

void controller_sdl_events(SDL_Event* event){
	if(event->type == SDL_CONTROLLERDEVICEADDED){
		std::cout << "game controller added." << std::endl;
		
		int game_controllers = 0;
		for (int i = 0; i < SDL_NumJoysticks(); i++){
			if (SDL_IsGameController(i)){
				game_controllers++;
			}
		}
		if(game_controllers == 1){
			for(unsigned i=0; i<windows.size(); ++i){
				windows[i].sdl_controller = SDL_GameControllerOpen(0);
				if(windows[i].sdl_controller != NULL){
					windows[i].default_mapping = SDL_GameControllerMapping(windows[i].sdl_controller);
					if (SDL_GameControllerHasSensor(windows[i].sdl_controller, SDL_SENSOR_GYRO)){
						SDL_GameControllerSetSensorEnabled(windows[i].sdl_controller, SDL_SENSOR_GYRO, (SDL_bool)SDL_TRUE);
					}
				}else{
					std::cout << "couldn't open sdl controller" << std::endl;
					std::cout << SDL_GetError() << std::endl;
				}
			}
		}
	}
}

void removeControllerWindow(unsigned ID){
	for(unsigned i=0; i<windows.size(); ++i){
		if(windows[i].ID == ID){
			glfwDestroyWindow(windows[i].glfw_window);
			windows.erase(windows.begin() + i);
			break;
		}
	}
}

void destroyWindows(){
	for(controller_window w : windows){
		glfwDestroyWindow(w.glfw_window);
	}
}

void make_grid(controller_window &w){
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec4> indices;

	int slices = 100;

	for(int j=0; j<=slices; ++j) {
		for(int i=0; i<=slices; ++i) {
			float x = (float)i/(float)slices;
			float y = 0;
			float z = (float)j/(float)slices;
			vertices.push_back(glm::vec3(x, y, z));
		}
	}

	for(int j=0; j<slices; ++j) {
		for(int i=0; i<slices; ++i) {

			int row1 =  j    * (slices+1);
			int row2 = (j+1) * (slices+1);

			indices.push_back(glm::uvec4(row1+i, row1+i+1, row1+i+1, row2+i+1));
			indices.push_back(glm::uvec4(row2+i+1, row2+i, row2+i, row1+i));

		}
	}

	glGenVertexArrays( 1, &w.grid_vao);
	glBindVertexArray( w.grid_vao );

	glGenBuffers( 1, &w.grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, w.grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	glGenBuffers( 1, &w.grid_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, w.grid_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(glm::uvec4), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	w.grid_length = (GLuint)indices.size()*4;
}

void update_camera(controller_window &w, GLuint &shader, int window_width, int window_height){
	glUseProgram(shader);

	if(w.freelook){
		w.freelook_direction.x = cos(glm::radians(w.freelook_pitch)) * sin(glm::radians(w.freelook_yaw));
		w.freelook_direction.y = sin(glm::radians(w.freelook_pitch));
		w.freelook_direction.z = cos(glm::radians(w.freelook_pitch)) * cos(glm::radians(w.freelook_yaw));
		
		glm::vec3 front = w.freelook_position + w.freelook_direction;
		
		w.view_matrix = glm::lookAt(w.freelook_position, front, glm::vec3(0.0f, 1.0f, 0.0f));
		}else{
		w.camera_position.x = cos(glm::radians(w.camera_pitch)) * sin(glm::radians(w.camera_yaw)) * w.camera_distance;
		w.camera_position.y = sin(glm::radians(w.camera_pitch)) * w.camera_distance;
		w.camera_position.z = cos(glm::radians(w.camera_pitch)) * cos(glm::radians(w.camera_yaw)) * w.camera_distance;

		glm::vec3 front = glm::normalize(glm::vec3(0.0, 0.0, 0.0) - w.camera_position);
		glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::cross(right, front);

		glm::mat4 roll_mat = glm::mat4(1.0f);
		roll_mat = glm::rotate(roll_mat, glm::radians(w.camera_roll), front);
		up = glm::vec3(roll_mat * glm::vec4(up, 1.0));
		
		w.view_matrix = glm::lookAt(glm::vec3(w.camera_position), glm::normalize(front), up);
	}
	
	shaderUniformMat4(shader, "view", w.view_matrix);
	
	w.projection_matrix = glm::perspective(glm::radians(45.0f), (float)window_width/window_height, 0.1f, 100.0f);
	shaderUniformMat4(shader, "projection", w.projection_matrix);

	glUseProgram(0);
}

void drawControllerWindows(){
	for(controller_window &w : windows){
		glfwMakeContextCurrent(w.glfw_window);
		glfwSwapInterval(w.swap_interval);

		w.deltaTime = glfwGetTime() - w.lastTime;
		w.lastTime = glfwGetTime();

		int width = 0;
		int height = 0;
		glfwGetWindowSize(w.glfw_window, &width, &height);
		glViewport(0, 0, width, height);
		
		update_camera(w, w.shader, width, height);
		update_camera(w, w.light_source_shader, width, height);
		update_camera(w, w.grid_shader, width, height);
		
		glEnable(GL_DEPTH_TEST);
    	//glDisable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
    	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if(w.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(w.bg_color[0] * w.bg_color[3], 
					 w.bg_color[1] * w.bg_color[3], 
					 w.bg_color[2] * w.bg_color[3], 
					 1.0f * w.bg_color[3]);
    	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Draw the Grid
		if (w.grid){
			glBindVertexArray(w.grid_vao);
			glUseProgram(w.grid_shader);	
			glEnableVertexAttribArray(0);
			glm::mat4 grid_model = glm::mat4(1.0f);
			grid_model = glm::translate(grid_model, glm::vec3(-50.0f, 0.0f, -50.0f));
			grid_model = glm::scale(grid_model, glm::vec3(100.0f, 0.0f, 100.0f));
			shaderUniformMat4(w.grid_shader, "model", grid_model);
			glDrawElements(GL_LINES, w.grid_length, GL_UNSIGNED_INT, NULL);
		}

		glBindVertexArray(w.lighting_vao);
		
		glUseProgram(w.light_source_shader);
		
		//Draw Point Light Source
		for(point_light p : w.point_lights){
			if(!p.hide){
				shaderUniformVec3(w.light_source_shader, "lightColor", glm::vec3(p.color[0], p.color[1], p.color[2]));
				glm::mat4 light_source_model = glm::mat4(1.0f);
				light_source_model = glm::translate(light_source_model, p.position); 
				light_source_model = glm::scale(light_source_model, glm::vec3(0.2f)); 
				shaderUniformMat4(w.light_source_shader, "model", light_source_model);	
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		//Draw Spot Light Source
		for(spot_light s : w.spot_lights){
			if(!s.hide){
				shaderUniformVec3(w.light_source_shader, "lightColor", glm::vec3(s.color[0], s.color[1], s.color[2]));
				glm::mat4 light_source_model = glm::mat4(1.0f);
				light_source_model = glm::translate(light_source_model, s.position); 
				light_source_model = glm::rotate(light_source_model, glm::radians(s.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				light_source_model = glm::rotate(light_source_model, glm::radians(s.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
				light_source_model = glm::scale(light_source_model, glm::vec3(0.1f, 0.1f, 0.3f)); 
				shaderUniformMat4(w.light_source_shader, "model", light_source_model);	
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		//Draw Lighting Subject Cubes
		glUseProgram(w.shader);
		
		if(w.freelook)
			shaderUniformVec3(w.shader, "viewPos", w.freelook_position);
		else
			shaderUniformVec3(w.shader, "viewPos", w.camera_position);

		shaderUniformFloat(w.shader, "time", glfwGetTime());

		//direct lights
		shaderUniformInt(w.shader, "direct_lights", w.direct_lights.size());
		for (unsigned i=0; i<w.direct_lights.size(); i++){
			std::string name = "dirLights[";
			name.append(std::to_string(i));
			name.append("]");
			shaderUniformVec3(w.shader, std::string(name).append(".direction").c_str(), w.direct_lights[i].direction);
			shaderUniformVec3(w.shader, std::string(name).append(".ambient").c_str(), 
			glm::vec3(w.direct_lights[i].color[0] * w.direct_lights[i].ambient,
					  w.direct_lights[i].color[1] * w.direct_lights[i].ambient,
					  w.direct_lights[i].color[2] * w.direct_lights[i].ambient));
			shaderUniformVec3(w.shader, std::string(name).append(".diffuse").c_str(), 
			glm::vec3(w.direct_lights[i].color[0] * w.direct_lights[i].diffuse,
					  w.direct_lights[i].color[1] * w.direct_lights[i].diffuse,
					  w.direct_lights[i].color[2] * w.direct_lights[i].diffuse));
			shaderUniformVec3(w.shader, std::string(name).append(".specular").c_str(), 
			glm::vec3(w.direct_lights[i].color[0] * w.direct_lights[i].specular,
					  w.direct_lights[i].color[1] * w.direct_lights[i].specular,
					  w.direct_lights[i].color[2] * w.direct_lights[i].specular));
		}

		//point lights
		shaderUniformInt(w.shader, "point_lights", w.point_lights.size());
		for (unsigned i=0; i<w.point_lights.size(); i++){
			std::string name = "pointLights[";
			name.append(std::to_string(i));
			name.append("]");
			shaderUniformFloat(w.shader, std::string(name).append(".constant").c_str(),  w.point_lights[i].constant - w.point_lights[i].intensity);
			shaderUniformFloat(w.shader, std::string(name).append(".linear").c_str(), 	 w.point_lights[i].linear);
			shaderUniformFloat(w.shader, std::string(name).append(".quadratic").c_str(), w.point_lights[i].quadratic);	
			shaderUniformVec3(w.shader,  std::string(name).append(".position").c_str(),  w.point_lights[i].position);
			shaderUniformVec3(w.shader,  std::string(name).append(".ambient").c_str(),   w.point_lights[i].ambient);
			shaderUniformVec3(w.shader,  std::string(name).append(".diffuse").c_str(),   w.point_lights[i].diffuse);
			shaderUniformVec3(w.shader,  std::string(name).append(".specular").c_str(),  w.point_lights[i].specular);
		}
		
		//spot lights
		shaderUniformInt(w.shader, "spot_lights", w.spot_lights.size());
		for (unsigned i=0; i<w.spot_lights.size(); i++){
			std::string name = "spotLights[";
			name.append(std::to_string(i));
			name.append("]");
			shaderUniformVec3(w.shader,  std::string(name).append(".position").c_str(),     w.spot_lights[i].position);
			shaderUniformVec3(w.shader,  std::string(name).append(".direction").c_str(),    w.spot_lights[i].direction);
			shaderUniformFloat(w.shader, std::string(name).append(".cutoff").c_str(),       glm::cos(glm::radians(w.spot_lights[i].cutoff)));
			shaderUniformFloat(w.shader, std::string(name).append(".outer_cutoff").c_str(), glm::cos(glm::radians(w.spot_lights[i].cutoff + (w.spot_lights[i].outer_cutoff * 0.2f))));
			shaderUniformFloat(w.shader, std::string(name).append(".constant").c_str(),     w.spot_lights[i].constant - w.spot_lights[i].intensity);
			shaderUniformFloat(w.shader, std::string(name).append(".linear").c_str(), 	    w.spot_lights[i].linear);
			shaderUniformFloat(w.shader, std::string(name).append(".quadratic").c_str(),    w.spot_lights[i].quadratic);	
			shaderUniformVec3(w.shader,  std::string(name).append(".ambient").c_str(),      w.spot_lights[i].ambient);
			shaderUniformVec3(w.shader,  std::string(name).append(".diffuse").c_str(),      w.spot_lights[i].diffuse);
			shaderUniformVec3(w.shader,  std::string(name).append(".specular").c_str(),     w.spot_lights[i].specular);
		}
		
		w.model.motion_matrix = w.gyro_matrix;
		
		drawModel(w.model, w.shader);
		
		glUseProgram(0);
		
		glfwSwapBuffers(w.glfw_window);
	}
}
