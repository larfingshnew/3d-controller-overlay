#if defined(__linux__) 
#elif __FreeBSD__
#elif __ANDROID__
#elif __APPLE__
#elif _WIN32
#include<windows.h>
#define SDL_MAIN_HANDLED
#else //some other operating system
#endif

#include "settings_window.h"
#include "strings.h"

std::string mesh_filenames[32] = {
    "top_shell.obj",
    "bottom_shell.obj",
    "extra.obj",
    "left_trigger.obj",
    "right_trigger.obj",
    "left_stick.obj",
    "right_stick.obj",
    "left_ring.obj",
    "right_ring.obj",
    "a_button.obj",
    "b_button.obj",
    "x_button.obj",
    "y_button.obj",
    "back_button.obj",
    "guide_button.obj",
    "start_button.obj",
    "left_cap.obj",
    "right_cap.obj",
    "left_bumper.obj",
    "right_bumper.obj",
    "dpad_up.obj",
    "dpad_down.obj",
    "dpad_left.obj",
    "dpad_right.obj",
    "misc.obj",
    "paddle1.obj",
    "paddle2.obj",
    "paddle3.obj",
    "paddle4.obj",
    "touchpad.obj",
    "touch_point1.obj",
    "touch_point2.obj"
};

std::string invalid_characters = "\\/:*?\"<>|";

std::string mapping_names[27] = {
	"a",
	"b",
	"x",
	"y",
	"back",
	"guide",
	"start",
	"leftstick",
	"rightstick",
	"leftshoulder",
	"rightshoulder",
	"dpup",
	"dpdown",
	"dpleft",
	"dpright",
	"touchpad",
	"misc",
	"paddle1",
	"paddle2",
	"paddle3",
	"paddle4",
	"leftx",
	"lefty",
	"rightx",
	"righty",
	"lefttrigger",
	"righttrigger"
};

std::string input_names[27] = {
	"a button",
	"b button",
	"x button",
	"y button",
	"back button",
	"guide button",
	"start button",
	"left stick click",
	"right stick click",
	"left bumper",
	"right bumper",
	"dpad up",
	"dpad down",
	"dpad left",
	"dpad right",
	"touchpad click",
	"misc button",
	"paddle 1",
	"paddle 2",
	"paddle 3",
	"paddle 4",
	"left stick x-axis",
	"left stick y-axis",
	"right stick x-axis",
	"right stick y-axis",
	"left trigger",
	"right trigger"
};

std::string current_mapping[27];

bool remap = false;
std::string rebind_string = "";

std::string binding_names[48] = {
	"unbound",
	"b0",
	"b1",
	"b2",
	"b3",
	"b4",
	"b5",
	"b6",
	"b7",
	"b8",
	"b9",
	"b10",
	"b11",
	"b12",
	"b13",
	"b14",
	"b15",
	"b16",
	"b17",
	"b18",
	"b19",
	"b20",
	"h0.0",
	"h0.1",
	"h0.2",
	"h0.3",
	"h0.4",
	"h0.5",
	"h0.6",
	"h0.7",
	"h0.8",
	"h0.9",
	"h1.0",
	"h1.1",
	"h1.2",
	"h1.3",
	"h1.4",
	"h1.5",
	"h1.6",
	"h1.7",
	"h1.8",
	"h1.9",
	"a0",
	"a1",
	"a2",
	"a3",
	"a4",
	"a5",
};

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

unsigned int tabs_made = 0;
unsigned selected_tab = 0;
unsigned selected_mesh = 0;
unsigned material_mesh = 0;
unsigned texture_mesh = 0;
			
GLFWwindow* glfw_settings_window;
GLFWmonitor* primary_monitor;
const GLFWvidmode* vid_mode;

ImGui::FileBrowser texture_dialog;
ImGui::FileBrowser model_dialog;

std::vector<window_tab> tabs;
std::vector<Texture> textures;

ImVec4 clear_color = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
ImGuiIO* io;

void createSettingsWindow(){
    glfwInit();

    #if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#elif defined(__APPLE__)
    // GL 3.3 Core + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#else
    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

	glfw_settings_window = glfwCreateWindow(640, 480, "3D Controller Overlay", NULL, NULL);
    if (glfw_settings_window == NULL)
    {
        std::cout << "Failed to create settings window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(glfw_settings_window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(glfw_settings_window, settings_framebuffer_size_callback);

	GLFWimage images[1]; 
	images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, 0, 4);
	if (images[0].pixels == NULL) {
		std::cout << "couldn't load settings window icon" << std::endl;
	}else{
		glfwSetWindowIcon(glfw_settings_window, 1, images); 
	}
	stbi_image_free(images[0].pixels);
	
	primary_monitor = glfwGetPrimaryMonitor();
	vid_mode = glfwGetVideoMode(primary_monitor);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	//io->ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    //io->ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glfw_settings_window, true);
    if(!ImGui_ImplOpenGL3_Init(glsl_version)){
		std::cout << "failed to init imgui for opengl3." << std::endl;
	}

	texture_dialog.SetWindowSize(400, 300);
    texture_dialog.SetTitle("Select Texture File");
    texture_dialog.SetTypeFilters({ ".png", ".jpg" });

	model_dialog.SetWindowSize(400, 300);
    model_dialog.SetTitle("Select Model File");
    model_dialog.SetTypeFilters({ ".obj" });
}

GLFWwindow* getSettingsWindow(){
	return glfw_settings_window;
}

void settings_framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void close_window(unsigned ID){
	removeControllerWindow(ID);
	removeTab(ID);
}

const GLFWvidmode* get_vid_mode(){
	return vid_mode;
}

void removeTab(unsigned ID){
	for(unsigned i=0; i<tabs.size(); ++i){
		if(tabs[i].ID == ID){
			tabs.erase(tabs.begin() + i);
			selected_tab = 0;
			break;
		}
	}
}

void removeSettingsWindow(){
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(glfw_settings_window);
}

void settings_window_input(bool &quit){
	if(glfwGetKey(glfw_settings_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(glfw_settings_window, true);
	}
	if(glfwWindowShouldClose(glfw_settings_window)){
		quit = true;
	}	
}

void settings_sdl_events(SDL_Event* event){
	//Maybe used later
}

bool check_tab_title_exists(std::string title){
	bool exists = false;
	for (my_tab t : tabs){
		if (title == t.title){
			exists = true;
			break;
		}
	}
	return exists;
}

void drawSettingsWindow(){
    glfwMakeContextCurrent(glfw_settings_window);
	glfwSwapInterval(1);
	
	ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGuiWindowFlags window_flags =
	ImGuiWindowFlags_NoDecoration |
	ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove;

	#ifdef IMGUI_HAS_VIEWPORT	
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	#else
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	#endif
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::Begin("Settings Window", nullptr, window_flags);
	
	bool new_controller_window = false;
	int new_tab_number = 1;
	std::string new_tab_title = "Controller ";
	while(check_tab_title_exists(std::string("Controller ").append(std::to_string(new_tab_number)))){
		new_tab_number++;
	}
	new_tab_title.append(std::to_string(new_tab_number));

	static ImGuiTabBarFlags tab_bar_flags = 
		ImGuiTabBarFlags_AutoSelectNewTabs | 
		ImGuiTabBarFlags_Reorderable | 
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)){
		if (ImGui::TabItemButton("New", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)){
			window_tab new_tab;
			tabs_made++;
			//new_tab_title.append(std::to_string(tabs_made));
			new_tab.title = new_tab_title;
			tabs.push_back(new_tab);

			new_controller_window = true;
		}
		for (unsigned i = 0; i<tabs.size(); ++i){
			bool open = true;
			if (ImGui::BeginTabItem(tabs[i].title.c_str(), &open, ImGuiTabItemFlags_None)){
				selected_tab = i;
				ImGui::EndTabItem();
			}
			if (!open){
				glfwSetWindowShouldClose(getControllerWindow(tabs[i].ID)->glfw_window, true);
			}
		}
		ImGui::EndTabBar();
	}

	if (tabs.size() > 0 && new_controller_window == false){
		controller_window* current_window = getControllerWindow(tabs[selected_tab].ID);
		if (ImGui::CollapsingHeader("Window")){
			char title[20] = {};
            if (ImGui::InputTextWithHint("Title", tabs[selected_tab].title.c_str(), title, IM_ARRAYSIZE(title), ImGuiInputTextFlags_EnterReturnsTrue)){
                glfwSetWindowTitle(current_window->glfw_window, title);
                tabs[selected_tab].title = std::string(title);
            }
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Set title of window.");
			ImGui::NewLine();
			//std::cout << "*****************" << std::endl;
			if(ImGui::Checkbox("Always on Top", &current_window->always_on_top)){
				glfwSetWindowAttrib(current_window->glfw_window, GLFW_FLOATING, 
				current_window->always_on_top);
			}
			//std::cout << ">>>>>>>>>>>>>>>>>" << std::endl;
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle if the window should be always on top of all other windows.");
			if(ImGui::Checkbox("Borderless", &current_window->borderless)){
				glfwSetWindowAttrib(current_window->glfw_window, GLFW_DECORATED, 
				!current_window->borderless);
			}
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle window title bar and borders.");
			ImGui::Checkbox("Drag to Move", &current_window->drag_to_move);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Left-click and drag inside window to move.");
			ImGui::Checkbox("Scroll to Resize", &current_window->scroll_to_resize);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Scroll mouse wheel inside window to resize.");
			ImGui::Checkbox("Show Grid", &current_window->grid);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Show the horizontal grid.");
			ImGui::Checkbox("Wireframe Mode", &current_window->wireframe);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle wireframe mode.");
			ImGui::NewLine();
			int w = 0;
            int h = 0;
			glfwGetWindowSize(current_window->glfw_window, &w, &h);
            if (ImGui::InputInt("Width", &w, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (w < 10)
                    w = 10;
				if (w > vid_mode->width)
					w = vid_mode->width;
                glfwSetWindowSize(current_window->glfw_window, w, h);
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Set width of window in pexels.");
			if (ImGui::InputInt("Height", &h, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (h < 10)
                    h = 10;
				if (h > vid_mode->height)
					h = vid_mode->height;
                glfwSetWindowSize(current_window->glfw_window, w, h);
            }
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Set height of window in pexels.");
			ImGui::NewLine();
			ImGui::SliderInt("Swap Interval", &current_window->swap_interval, 0, 2);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Set the swap interval (v-sync) for the controller window.");
			/*
			int fps_cap = current_window->frame_cap;
			if (ImGui::InputInt("Frame Cap", &fps_cap, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)){
				if (fps_cap > 300)
					fps_cap = 300;
				if (fps_cap < 10)
					fps_cap = 10;
				current_window->frame_cap = fps_cap;
			}
			if (ImGui::IsItemHovered())
               	ImGui::SetTooltip("Set the maximum frame rate of controller window.");
            //*/
			ImGui::NewLine();
			ImGui::ColorEdit4("Background Color", current_window->bg_color);
			if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Adjust red, green, blue and alpha (opacity) values for window background color.");
		}
		if (ImGui::CollapsingHeader("Camera")){
			ImGui::Checkbox("Freelook", &current_window->freelook);
			if(current_window->freelook){
				ImGui::TextUnformatted("Note: Controller window must be in focus.");
                ImGui::TextUnformatted("W,A,S,D = move");
                ImGui::TextUnformatted("Space / Ctrl = up / down");
                ImGui::TextUnformatted("Arrow Keys = look");
				ImGui::TextUnformatted("Right-Click + drag = look");
                ImGui::SliderInt("Move Speed", &current_window->move_speed, 0, 10);
				ImGui::SliderInt("Turn Speed", &current_window->turn_speed, 0, 10);
				if (ImGui::Button("Reset")){
					current_window->move_speed = 5.0f;
					current_window->turn_speed = 5.0f;
					current_window->freelook_yaw = 180.0f;
					current_window->freelook_pitch = 0.0f;
					current_window->freelook_position = glm::vec3(0.0f, 0.5f, 3.0f);
					current_window->freelook_direction = glm::vec3(0.0f, 0.0f, -1.0f);
				}
			}else{
				ImGui::SliderFloat("Distance", &current_window->camera_distance, 1, 10);
				ImGui::SliderFloat("Yaw", &current_window->camera_yaw, -180, 180);
				ImGui::SliderFloat("Pitch", &current_window->camera_pitch, -89.999, 89.999);
				ImGui::SliderFloat("Roll", &current_window->camera_roll, -180, 180);
				if (ImGui::Button("Reset")){
					current_window->camera_distance = 3.3f;
					current_window->camera_yaw = 0.0f;
					current_window->camera_pitch = 89.999f;
					current_window->camera_roll = 0.0f;
				}
			}
		}
		if (ImGui::CollapsingHeader("Controller")){
			std::vector<int> devices;
            for (int i = 0; i < SDL_NumJoysticks(); i++){
                if (SDL_IsGameController(i)){
                    devices.push_back(i);
				}
            }
			std::string device_name = "";
			if(current_window->sdl_controller != nullptr)
				device_name = SDL_GameControllerName(current_window->sdl_controller);
			if (ImGui::BeginCombo("Controllers", device_name.c_str(), 0)){
                for (size_t i = 0; i < devices.size(); i++){
                    if (ImGui::Selectable(SDL_GameControllerNameForIndex(devices[i]))){
						SDL_GameControllerAddMapping(current_window->default_mapping.c_str());
                        current_window->sdl_controller = SDL_GameControllerOpen(i);
						if(current_window->sdl_controller != NULL){
							SDL_GameControllerSetSensorEnabled(current_window->sdl_controller, SDL_SENSOR_GYRO, (SDL_bool)current_window->gyro_enabled);
							current_window->default_mapping = SDL_GameControllerMapping(current_window->sdl_controller);
						}else{
							std::cout << "couldn't open sdl controller " << i << std::endl;
							std::cout << SDL_GetError() << std::endl;
						}
					}
                }
            	ImGui::EndCombo();
			}
			if(ImGui::TreeNode("Settings")){
				ImGui::Checkbox("Popup Bumpers", &current_window->model.popup_bumpers);
				ImGui::SameLine();
				ImGui::Checkbox("Popup Triggers", &current_window->model.popup_triggers);
				ImGui::SameLine();
				ImGui::Checkbox("Popup Paddles", &current_window->model.popup_paddles);
				ImGui::NewLine();
				ImGui::SliderInt("L-Stick Highlight Deadzone", &current_window->model.meshes[7].ring_highlight_deadzone, 0, 100);
				ImGui::SliderInt("R-Stick Highlight Deadzone", &current_window->model.meshes[8].ring_highlight_deadzone, 0, 100);
				ImGui::ColorEdit3("Hightlight Color", current_window->highlight_color);
				for(int i = 3; i<32; i++){
					if(i != 5 && i != 6){
						current_window->model.meshes[i].material.highlight[0] = current_window->highlight_color[0];
						current_window->model.meshes[i].material.highlight[1] = current_window->highlight_color[1];
						current_window->model.meshes[i].material.highlight[2] = current_window->highlight_color[2];
					}
				}
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Materials")){
				static std::string mesh_name = mesh_names[selected_mesh].c_str();
				if (ImGui::BeginCombo("Meshes", mesh_name.c_str(), 0)){
					for(int i=0; i<IM_ARRAYSIZE(mesh_names); i++){
						if (ImGui::Selectable(mesh_names[i].c_str())){
							mesh_name = mesh_names[i].c_str();
							material_mesh = i;
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NewLine();
				ImGui::SliderFloat("Ambient", &current_window->model.meshes[material_mesh].material.ambient, 0, 1);
				ImGui::SliderFloat("Diffuse", &current_window->model.meshes[material_mesh].material.diffuse, 0, 1);
				ImGui::SliderFloat("Specular", &current_window->model.meshes[material_mesh].material.specular, 0, 1);
				ImGui::SliderFloat("Shininess", &current_window->model.meshes[material_mesh].material.shininess, 1, 256);
				ImGui::ColorEdit3("Color", current_window->model.meshes[material_mesh].material.color);
				/*
				if(material_mesh > 2 && material_mesh != 5 && material_mesh != 6){
					ImGui::ColorEdit3("Highlight", current_window->model.meshes[material_mesh].material.highlight);
				}
				//*/
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Textures")){
				static std::string mesh_name = mesh_names[selected_mesh].c_str();
				if (ImGui::BeginCombo("Meshes", mesh_name.c_str(), 0)){
					for(int i=0; i<IM_ARRAYSIZE(mesh_names); i++){
						if (ImGui::Selectable(mesh_names[i].c_str())){
							mesh_name = mesh_names[i].c_str();
							texture_mesh = i;
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NewLine();
				static size_t current_texture = 0;
				if (ImGui::BeginListBox("Textures")){
					for (size_t n = 0; n < current_window->model.meshes[texture_mesh].textures.size(); n++){
						const bool is_selected = (current_texture == n);
						if (ImGui::Selectable(current_window->model.meshes[texture_mesh].textures[n].name.c_str(), is_selected)){
							current_texture = n;
						}
					}
					ImGui::EndListBox();
				}
				if(current_window->model.meshes[texture_mesh].textures.size() < 16){
					if (ImGui::Button("New Texture")){
						texture_dialog.Open();
						current_texture = current_window->model.meshes[texture_mesh].textures.size();
					}
				}
				if(current_window->model.meshes[texture_mesh].textures.size() > 0){
					if(current_window->model.meshes[texture_mesh].textures.size() < 16){
						ImGui::SameLine();
					}
					if (ImGui::Button("Delete Texture")){
						glfwMakeContextCurrent(current_window->glfw_window);
						deleteTexture(current_window->model.meshes[texture_mesh].textures[current_texture].id);
						current_window->model.meshes[texture_mesh].textures.erase(current_window->model.meshes[texture_mesh].textures.begin() + current_texture);
						glfwMakeContextCurrent(glfw_settings_window);
						current_texture = 0;
						for (size_t i = 0; i < current_window->model.meshes[texture_mesh].textures.size(); i++){
							Texture* t = &current_window->model.meshes[texture_mesh].textures[i];
							t->name = std::to_string(i + 1) + ": " + t->path;
						}
					}
					ImGui::SameLine();
					if (ImGui::ArrowButton("##up", ImGuiDir_Up)){
						std::cout << "move texture up" << std::endl;
						if(current_texture > 0){
							Texture temp = current_window->model.meshes[texture_mesh].textures[current_texture - 1];
							current_window->model.meshes[texture_mesh].textures[current_texture - 1] = current_window->model.meshes[texture_mesh].textures[current_texture];
							current_window->model.meshes[texture_mesh].textures[current_texture] = temp;
							current_texture--;
						}
						for (size_t i = 0; i < current_window->model.meshes[texture_mesh].textures.size(); i++){
							Texture* t = &current_window->model.meshes[texture_mesh].textures[i];
							t->name = std::to_string(i + 1) + ": " + t->path;
						}
					}
					ImGui::SameLine();
					if (ImGui::ArrowButton("##down", ImGuiDir_Down)){
						std::cout << "move texture down" << std::endl;
						if(current_texture < current_window->model.meshes[texture_mesh].textures.size() - 1){
							Texture temp = current_window->model.meshes[texture_mesh].textures[current_texture + 1];
							current_window->model.meshes[texture_mesh].textures[current_texture + 1] = current_window->model.meshes[texture_mesh].textures[current_texture];
							current_window->model.meshes[texture_mesh].textures[current_texture] = temp;
							current_texture++;
						}
						for (size_t i = 0; i < current_window->model.meshes[texture_mesh].textures.size(); i++){
							Texture* t = &current_window->model.meshes[texture_mesh].textures[i];
							t->name = std::to_string(i + 1) + ": " + t->path;
						}
					}
					Texture* t = &current_window->model.meshes[texture_mesh].textures[current_texture];
					ImGui::NewLine();
					enum Type { diffuse, specular, emission, type_count };
					const char* type_names[type_count] = { "Diffuse", "Specular", "Emissive" };
					const char* type_name = (t->type >= 0 && t->type < type_count) ? type_names[t->type] : "Unknown";
					ImGui::SliderInt("Type", &t->type, 0, type_count - 1, type_name);
					enum Wrap { repeat, mirror_repeat, clamp_edge, clamp_border, wrap_count };
					const char* wrap_names[wrap_count] = { "Repeat", "Mirrored Repeat", "Clamp to Edge", "Clamp to Border" };
					const char* wrap_name_x = (t->wrapX >= 0 && t->wrapX < wrap_count) ? wrap_names[t->wrapX] : "Unknown";
					if(ImGui::SliderInt("X Wrap", &t->wrapX, 0, wrap_count - 1, wrap_name_x)){
						glfwMakeContextCurrent(current_window->glfw_window);
						glBindTexture(GL_TEXTURE_2D, current_window->model.meshes[texture_mesh].textures[current_texture].id);
						switch(t->wrapX){
							case repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
								break;
							case mirror_repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
								break;
							case clamp_edge : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
								break;
							case clamp_border : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
								glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, current_window->model.meshes[texture_mesh].textures[current_texture].border);
								break;
						}
						glfwMakeContextCurrent(glfw_settings_window);
					}
					const char* wrap_name_y = (t->wrapY >= 0 && t->wrapY < wrap_count) ? wrap_names[t->wrapY] : "Unknown";
					if(ImGui::SliderInt("Y Wrap", &t->wrapY, 0, wrap_count - 1, wrap_name_y)){
						glfwMakeContextCurrent(current_window->glfw_window);
						glBindTexture(GL_TEXTURE_2D, current_window->model.meshes[texture_mesh].textures[current_texture].id);
						switch(t->wrapY){
							case repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
								break;
							case mirror_repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
								break;
							case clamp_edge : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
								break;
							case clamp_border : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
								glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, current_window->model.meshes[texture_mesh].textures[current_texture].border);
								break;
						}
						glfwMakeContextCurrent(glfw_settings_window);
					}
					if(ImGui::ColorEdit3("Border Color", current_window->model.meshes[texture_mesh].textures[current_texture].border)){
						glfwMakeContextCurrent(current_window->glfw_window);
						glBindTexture(GL_TEXTURE_2D, current_window->model.meshes[texture_mesh].textures[current_texture].id);
						glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, current_window->model.meshes[texture_mesh].textures[current_texture].border);
						glfwMakeContextCurrent(glfw_settings_window);
					}
					ImGui::InputFloat("Offset X", &current_window->model.meshes[texture_mesh].textures[current_texture].offsetX, 0.01f, 1.0f, "%.3f");
					ImGui::InputFloat("Offset Y", &current_window->model.meshes[texture_mesh].textures[current_texture].offsetY, 0.01f, 1.0f, "%.3f");
					ImGui::InputFloat("Scale X", &current_window->model.meshes[texture_mesh].textures[current_texture].scaleX, 0.01f, 1.0f, "%.3f");
					ImGui::InputFloat("Scale Y", &current_window->model.meshes[texture_mesh].textures[current_texture].scaleY, 0.01f, 1.0f, "%.3f");
					ImGui::SliderAngle("Rotation", &current_window->model.meshes[texture_mesh].textures[current_texture].rotation, -180.0f, 180.0f);
				}
				ImGui::TreePop();
			}	
		}
		if (ImGui::CollapsingHeader("Model")){
			if (ImGui::BeginCombo("Models", current_window->model_name.c_str(), 0)){
				std::string dir_path = SDL_GetBasePath();
				dir_path.append("models/");
				struct stat sb;
				for (const auto & entry : std::filesystem::directory_iterator(dir_path)){
					std::string model_dir = entry.path().string();
					std::string delimiter = "/";
					if (stat(model_dir.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)){
						size_t pos = 0;
						while ((pos = model_dir.find(delimiter)) != std::string::npos){
							model_dir.erase(0, pos + delimiter.length());
						}
						if (ImGui::Selectable(model_dir.c_str())){
							current_window->model_name = model_dir;
							std::string model_path("models/");
							model_path.append(model_dir);
							writeInfo(current_window->model, current_window->model.path);
							glfwMakeContextCurrent(current_window->glfw_window);
							loadModel(current_window->model, model_path);
							glfwMakeContextCurrent(glfw_settings_window);
						}
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("New Model")){
                ImGui::OpenPopup("new");
			}
            if (ImGui::BeginPopup("new")){
                char name[32] = {};
                static bool name_valid = true;
                if (ImGui::InputText("Model Name", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue)){
                    bool valid = check_filename_valid(name);
                    name_valid = valid;

                    if (valid){
                        std::string new_model_path = "models/";
                        new_model_path.append(name);

						std::filesystem::path path(SDL_GetBasePath());
						std::filesystem::path new_path(new_model_path);
						path /= new_path;
						std::filesystem::create_directory(path);
						writeInfo(current_window->model, current_window->model.path);
						glfwMakeContextCurrent(current_window->glfw_window);
						loadModel(current_window->model, new_model_path);
						glfwMakeContextCurrent(glfw_settings_window);
						current_window->model_name = get_top_folder(current_window->model.path);
						ImGui::CloseCurrentPopup();
					}else{
                        std::cout << "Name contains invalid characters " << invalid_characters << std::endl;
					}
                }
                if (!name_valid){
                    ImGui::Text("Name cannot include characters \\/:*?\"<>|");
				}
                ImGui::EndPopup();
            }
			ImGui::SameLine();
			if (ImGui::Button("Delete Model")){
				ImGui::OpenPopup("delete");
			}
			if (ImGui::BeginPopup("delete")){
                ImGui::Text("Delete this model?");
                if (ImGui::Button("Confirm")){
                    std::filesystem::remove_all(current_window->model.path);
					std::string dir_path = SDL_GetBasePath();
					dir_path.append("models/");
					std::vector<std::string> model_folders;
					struct stat sb;
					for (const auto & entry : std::filesystem::directory_iterator(dir_path)){
						if (stat(entry.path().string().c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)){
							model_folders.push_back(entry.path().string());
						}
					}
					if (model_folders.size() > 0){
						glfwMakeContextCurrent(current_window->glfw_window);
						loadModel(current_window->model, model_folders.front().c_str());
						glfwMakeContextCurrent(glfw_settings_window);
						current_window->model_name = get_top_folder(model_folders.front());
					}else{
						current_window->model_name = "";
					}
					ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
			ImGui::NewLine();
			current_window->mesh_name = mesh_names[selected_mesh].c_str();
			if (ImGui::BeginCombo("Meshes", current_window->mesh_name.c_str(), 0)){
				for(int i=0; i<IM_ARRAYSIZE(mesh_names); i++){
					if (ImGui::Selectable(mesh_names[i].c_str())){
						current_window->mesh_name = mesh_names[i].c_str();
						selected_mesh = i;
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Import Mesh")){
				model_dialog.Open();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Mesh")){
				ImGui::OpenPopup("delete_mesh");
			}
			if (ImGui::BeginPopup("delete_mesh")){
                ImGui::Text("Delete this mesh?");
                if (ImGui::Button("Confirm")){
					std::string mesh_path = current_window->model.path;
					mesh_path.append("/");
					mesh_path.append(mesh_filenames[selected_mesh]);
					std::cout << "mesh path : " << mesh_path.c_str() << std::endl;
					if(std::remove(mesh_path.c_str()) == 0){
						std::cout << "file deleted successfully." << std::endl;
					}else{
						std::cout << "unable to delete file." << std::endl;
					}
					writeInfo(current_window->model, current_window->model.path);
					glfwMakeContextCurrent(current_window->glfw_window);
					loadModel(current_window->model, current_window->model.path);
					glfwMakeContextCurrent(glfw_settings_window);
					ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
			ImGui::NewLine();
			ImGui::InputFloat("X Position", &current_window->model.meshes[selected_mesh].position[0], 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Y Position", &current_window->model.meshes[selected_mesh].position[1], 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Z Position", &current_window->model.meshes[selected_mesh].position[2], 0.01f, 1.0f, "%.3f");
			//LEFT STICK
			if(selected_mesh == 5){
				ImGui::NewLine();
				if(ImGui::SliderAngle("Max Angle", &current_window->model.meshes[selected_mesh].stick_max, 0.0f, 45.0f)){
					current_window->model.meshes[7].stick_max = current_window->model.meshes[5].stick_max;
					current_window->model.meshes[16].stick_max = current_window->model.meshes[5].stick_max;
				}
			}
			//RIGHT STICK
			if(selected_mesh == 6){
				ImGui::NewLine();
				if(ImGui::SliderAngle("Max Angle", &current_window->model.meshes[selected_mesh].stick_max, 0.0f, 45.0f)){
					current_window->model.meshes[8].stick_max = current_window->model.meshes[6].stick_max;
					current_window->model.meshes[17].stick_max = current_window->model.meshes[6].stick_max;
				}
			}
			//TRIGGERS
			if(selected_mesh == 3 || selected_mesh == 4){
				ImGui::NewLine();
				ImGui::SliderAngle("Max Angle", &current_window->model.meshes[selected_mesh].trigger_max, 0.0f, 90.0f);
				ImGui::NewLine();
				ImGui::InputFloat("X Travel", &current_window->model.meshes[selected_mesh].travel[0], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Y Travel", &current_window->model.meshes[selected_mesh].travel[1], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Z Travel", &current_window->model.meshes[selected_mesh].travel[2], 0.01f, 1.0f, "%.3f");
				ImGui::NewLine();
				ImGui::InputFloat("Popup Offset X", &current_window->model.meshes[selected_mesh].popup_offset[0], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Popup Offset Y", &current_window->model.meshes[selected_mesh].popup_offset[1], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Popup Offset Z", &current_window->model.meshes[selected_mesh].popup_offset[2], 0.01f, 1.0f, "%.3f");
				ImGui::NewLine();
				ImGui::SliderAngle("Popup Yaw", &current_window->model.meshes[selected_mesh].popup_rotation[1], -180, 180);
				ImGui::SliderAngle("Popup Pitch", &current_window->model.meshes[selected_mesh].popup_rotation[0], -180, 180);
				ImGui::SliderAngle("Popup Roll", &current_window->model.meshes[selected_mesh].popup_rotation[2], -180, 180);
			}
			//BUTTONS
			if(selected_mesh > 8 && selected_mesh < 30){
				ImGui::NewLine();
				ImGui::InputFloat("X Travel", &current_window->model.meshes[selected_mesh].travel[0], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Y Travel", &current_window->model.meshes[selected_mesh].travel[1], 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Z Travel", &current_window->model.meshes[selected_mesh].travel[2], 0.01f, 1.0f, "%.3f");
				//BUMPERS AN PADDLES
				if((selected_mesh == 18 || selected_mesh == 19) || (selected_mesh > 24 && selected_mesh < 29)){
					ImGui::NewLine();
					ImGui::InputFloat("Popup Offset X", &current_window->model.meshes[selected_mesh].popup_offset[0], 0.01f, 1.0f, "%.3f");
					ImGui::InputFloat("Popup Offset Y", &current_window->model.meshes[selected_mesh].popup_offset[1], 0.01f, 1.0f, "%.3f");
					ImGui::InputFloat("Popup Offset Z", &current_window->model.meshes[selected_mesh].popup_offset[2], 0.01f, 1.0f, "%.3f");
					ImGui::NewLine();
					ImGui::SliderAngle("Popup Yaw", &current_window->model.meshes[selected_mesh].popup_rotation[1], -180, 180);
					ImGui::SliderAngle("Popup Pitch", &current_window->model.meshes[selected_mesh].popup_rotation[0], -180, 180);
					ImGui::SliderAngle("Popup Roll", &current_window->model.meshes[selected_mesh].popup_rotation[2], -180, 180);
				}
			}
			//TOUCHPAD
			if(selected_mesh == 29){
				ImGui::NewLine();
				if(ImGui::InputFloat("Touch Area Width",  &current_window->model.meshes[30].touch_width, 0.01f, 1.0f, "%.3f")){
					current_window->model.meshes[31].touch_width = current_window->model.meshes[30].touch_width;
				}
				if(ImGui::InputFloat("Touch Area Height",  &current_window->model.meshes[30].touch_height, 0.01f, 1.0f, "%.3f")){
					current_window->model.meshes[31].touch_height = current_window->model.meshes[30].touch_height;
				}
			}
		}
		if (ImGui::CollapsingHeader("Gyro")){
			if (SDL_GameControllerHasSensor(current_window->sdl_controller, SDL_SENSOR_GYRO)){
				if (ImGui::Checkbox("Enable Gyro", &current_window->gyro_enabled)){
					if (current_window->gyro_enabled){
						current_window->gyro_toggled = true;
					}
				}
				ImGui::SliderInt("Gyro Correction", &current_window->gyro_correction, 0, 10);
				if (ImGui::Button("Reset Gyro")){
					current_window->gyro_matrix = glm::mat4(1.0f);
				}
				ImGui::NewLine();
				ImGui::Text("Reset Gyro button combo");
				std::string button1_name = "";
				if(current_window->reset_gyro_button1 > -1){
					//button1_name = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)current_window->reset_gyro_button1);
					button1_name = input_names[current_window->reset_gyro_button1].c_str();
				}else{
					button1_name = "none";
				}
				if (ImGui::BeginCombo("Button 1", button1_name.c_str(), 0)){
					for (unsigned i = 0; i < 22; i++){
						if (i>0){
							//if (ImGui::Selectable(SDL_GameControllerGetStringForButton((SDL_GameControllerButton)(i-1)))){
							if (ImGui::Selectable(input_names[i-1].c_str())){
								current_window->reset_gyro_button1 = i-1;
							}
						}else{
							if (ImGui::Selectable("none")){
								current_window->reset_gyro_button1 = -1;
							}
						}
					}
					ImGui::EndCombo();
				}
				std::string button2_name = "";
				if(current_window->reset_gyro_button2 > -1){
					//button2_name = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)current_window->reset_gyro_button2);
					button2_name = input_names[current_window->reset_gyro_button2].c_str();
				}else{
					button2_name = "none";
				}
				if (ImGui::BeginCombo("Button 2", button2_name.c_str(), 0)){
					for (unsigned i = 0; i < 22; i++){
						if (i>0){
							//if (ImGui::Selectable(SDL_GameControllerGetStringForButton((SDL_GameControllerButton)(i-1)))){
							if (ImGui::Selectable(input_names[i-1].c_str())){
								current_window->reset_gyro_button2 = i-1;
							}
						}else{
							if (ImGui::Selectable("none")){
								current_window->reset_gyro_button2 = -1;
							}
						}
					}
					ImGui::EndCombo();
				}
			}else{
				ImGui::Text("No Gyroscope detected for selected controller.");
			}
		}
		if (ImGui::CollapsingHeader("Lighting")){
			if (ImGui::TreeNode("Directional Lights")){
				static unsigned current_dir_light = 0;
				std::string preview_name = "";
				if (current_window->direct_lights.size() > 0)
					preview_name = current_window->direct_lights[current_dir_light].name.c_str();
				if (ImGui::BeginCombo("Lights", preview_name.c_str(), 0)){
					for (unsigned i = 0; i < current_window->direct_lights.size(); i++){
						if (ImGui::Selectable(current_window->direct_lights[i].name.c_str())){
							current_dir_light = i;
						}
					}
					ImGui::EndCombo();
				}
				if(current_window->direct_lights.size() < 16){
					if (ImGui::Button("New Light")){
						direct_light new_dir_light;
						std::string new_light_name = "Directional Light ";
						static unsigned count = current_window->direct_lights.size() + 1;
						bool name_exists = false;
						while(true){
							name_exists = false;
							std::string test_name = new_light_name;
							test_name.append(std::to_string(count));
							for(direct_light d : current_window->direct_lights){
								if(d.name == test_name.c_str()){
									name_exists = true;
									count++;
									break;
								}
							}
							if(!name_exists)
								break;
						}
						new_dir_light.name = new_light_name.append(std::to_string(count)).c_str();
						count++;
						current_window->direct_lights.push_back(new_dir_light);
						current_dir_light = current_window->direct_lights.size() -1;
					}
				}
				if(current_window->direct_lights.size() > 0 && current_window->direct_lights.size() < 16)
					ImGui::SameLine();
				if (current_window->direct_lights.size() > 0){
					if (ImGui::Button("Delete Light")){
						current_window->direct_lights.erase(current_window->direct_lights.begin() + current_dir_light);
						current_dir_light = 0;
					}
					ImGui::NewLine();
					direct_light* d = &current_window->direct_lights[current_dir_light];
					char name[64] = {};
					if (ImGui::InputTextWithHint("Name", d->name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue)){
						bool exists = false;
						for(direct_light d : current_window->direct_lights){
							if(d.name == name){
								exists = true;
								break;
							}
						}
						if(!exists)
							d->name = std::string(name);
					}
					ImGui::SliderFloat("X Direction", &d->direction.x, -1, 1);
					ImGui::SliderFloat("Y Direction", &d->direction.y, -1, 1);
					ImGui::SliderFloat("Z Direction", &d->direction.z, -1, 1);
					ImGui::ColorEdit3("Color", d->color);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Point Lights")){
				static unsigned current_point_light = 0;
				std::string preview_name = "";
				if (current_window->point_lights.size() > 0)
					preview_name = current_window->point_lights[current_point_light].name.c_str();
				if (ImGui::BeginCombo("Lights", preview_name.c_str(), 0)){
					for (unsigned i = 0; i < current_window->point_lights.size(); i++){
						if (ImGui::Selectable(current_window->point_lights[i].name.c_str())){
							current_point_light = i;
						}
					}
					ImGui::EndCombo();
				}
				if(current_window->point_lights.size() < 16){
					if (ImGui::Button("New Light")){
						point_light new_point_light;
						std::string new_light_name = "Point Light ";
						static unsigned count = current_window->point_lights.size() + 1;
						bool name_exists = false;
						while(true){
							name_exists = false;
							std::string test_name = new_light_name;
							test_name.append(std::to_string(count));
							for(point_light p : current_window->point_lights){
								if(p.name == test_name.c_str()){
									name_exists = true;
									count++;
									break;
								}
							}
							if(!name_exists)
								break;
						}
						new_point_light.name = new_light_name.append(std::to_string(count)).c_str();
						count++;
						new_point_light.position.x = 2.0f;
						new_point_light.position.y = 2.0f;
						new_point_light.position.z = 2.0f;
						current_window->point_lights.push_back(new_point_light);
						current_point_light = current_window->point_lights.size() -1;
					}
				}
				if(current_window->point_lights.size() > 0 && current_window->point_lights.size() < 16)
					ImGui::SameLine();
				if (current_window->point_lights.size() > 0){
					if (ImGui::Button("Delete Light")){
						current_window->point_lights.erase(current_window->point_lights.begin() + current_point_light);
						current_point_light = 0;
					}
					ImGui::NewLine();
					point_light* p = &current_window->point_lights[current_point_light];
					char name[64] = {};
					if (ImGui::InputTextWithHint("Name", p->name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue)){
						bool exists = false;
						for(point_light p : current_window->point_lights){
							if(p.name == name){
								exists = true;
								break;
							}
						}
						if(!exists)
							p->name = std::string(name);
					}
					ImGui::Checkbox("Hide Source", &p->hide);
					ImGui::SliderFloat("X Position", &p->position.x, -10, 10);
					ImGui::SliderFloat("Y Position", &p->position.y, -10, 10);
					ImGui::SliderFloat("Z Position", &p->position.z, -10, 10);
					ImGui::SliderFloat("Brightness", &p->intensity, 0, 1);
					if(ImGui::ColorEdit3("Color", p->color)){
						p->ambient.r = p->color[0] * 0.05f;
						p->ambient.g = p->color[1] * 0.05f;
						p->ambient.b = p->color[2] * 0.05f;

						p->diffuse.r = p->color[0] * 0.8f;
						p->diffuse.g = p->color[1] * 0.8f;
						p->diffuse.b = p->color[2] * 0.8f;
						
						p->specular.r = p->color[0];
						p->specular.g = p->color[1];
						p->specular.b = p->color[2];
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Spot Lights")){
				static unsigned current_spot_light = 0;
				std::string preview_name = "";
				if (current_window->spot_lights.size() > 0)
					preview_name = current_window->spot_lights[current_spot_light].name.c_str();
				if (ImGui::BeginCombo("Lights", preview_name.c_str(), 0)){
					for (unsigned i = 0; i < current_window->spot_lights.size(); i++){
						if (ImGui::Selectable(current_window->spot_lights[i].name.c_str())){
							current_spot_light = i;
						}
					}
					ImGui::EndCombo();
				}
				if(current_window->spot_lights.size() < 16){
					if (ImGui::Button("New Light")){
						spot_light new_spot_light;
						std::string new_light_name = "Spot Light ";
						static unsigned count = current_window->spot_lights.size() + 1;
						bool name_exists = false;
						while(true){
							name_exists = false;
							std::string test_name = new_light_name;
							test_name.append(std::to_string(count));
							for(spot_light s : current_window->spot_lights){
								if(s.name == test_name.c_str()){
									name_exists = true;
									count++;
									break;
								}
							}
							if(!name_exists)
								break;
						}
						new_spot_light.name = new_light_name.append(std::to_string(count)).c_str();
						count++;
						new_spot_light.position.x = 0.0f;
						new_spot_light.position.y = 0.0f;
						new_spot_light.position.z = 2.0f;
						current_window->spot_lights.push_back(new_spot_light);
						current_spot_light = current_window->spot_lights.size() -1;
					}
				}
				if(current_window->spot_lights.size() > 0 && current_window->spot_lights.size() < 16)
					ImGui::SameLine();
				if (current_window->spot_lights.size() > 0){
					if (ImGui::Button("Delete Light")){
						current_window->spot_lights.erase(current_window->spot_lights.begin() + current_spot_light);
						current_spot_light = 0;
					}
					ImGui::NewLine();
					spot_light* s = &current_window->spot_lights[current_spot_light];
					char name[64] = {};
					if (ImGui::InputTextWithHint("Name", s->name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue)){
						bool exists = false;
						for(spot_light s : current_window->spot_lights){
							if(s.name == name){
								exists = true;
								break;
							}
						}
						if(!exists)
							s->name = std::string(name);
					}
					ImGui::Checkbox("Hide Source", &s->hide);
					ImGui::SliderFloat("X Position", &s->position.x, -10, 10);
					ImGui::SliderFloat("Y Position", &s->position.y, -10, 10);
					ImGui::SliderFloat("Z Position", &s->position.z, -10, 10);
					ImGui::SliderFloat("Brightness",  &s->intensity, 0, 1);
					if(ImGui::ColorEdit3("Color", s->color)){
						s->ambient.r = s->color[0] * 0.05f;
						s->ambient.g = s->color[1] * 0.05f;
						s->ambient.b = s->color[2] * 0.05f;

						s->diffuse.r = s->color[0] * 0.8f;
						s->diffuse.g = s->color[1] * 0.8f;
						s->diffuse.b = s->color[2] * 0.8f;
						
						s->specular.r = s->color[0];
						s->specular.g = s->color[1];
						s->specular.b = s->color[2];
					}
					if(ImGui::SliderFloat("Yaw", &s->yaw, -180, 180)){
						s->direction.x = cos(glm::radians(s->pitch)) * sin(glm::radians(s->yaw + 180));
						s->direction.y = sin(glm::radians(s->pitch));
						s->direction.z = cos(glm::radians(s->pitch)) * cos(glm::radians(s->yaw + 180));
					}
					if(ImGui::SliderFloat("Pitch", &s->pitch, -90, 90)){
						s->direction.x = cos(glm::radians(s->pitch)) * sin(glm::radians(s->yaw + 180));
						s->direction.y = sin(glm::radians(s->pitch));
						s->direction.z = cos(glm::radians(s->pitch)) * cos(glm::radians(s->yaw + 180));
					}
					//ImGui::SliderFloat("X Direction", &s->direction.x, -1, 1);
					//ImGui::SliderFloat("Y Direction", &s->direction.y, -1, 1);
					//ImGui::SliderFloat("Z Direction", &s->direction.z, -1, 1);
					ImGui::SliderFloat("Beam Angle", &s->cutoff, 0, 90);
					ImGui::SliderFloat("Edge Blur", &s->outer_cutoff, 0, 100);
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::CollapsingHeader("Mapping")){
			std::vector<std::string> mapping = get_current_mapping(current_window->sdl_controller);
			for(std::string s : mapping){
				std::vector<std::string> binding = get_binding(s);
				for (int i = 0; i < 27; i++){
					if(binding[0] == mapping_names[i]){
						current_mapping[i] = binding[1];
					}
				}
			}
			static unsigned current_input = 0;
			std::string input_name = input_names[current_input];
			if (ImGui::BeginCombo("Input", input_name.c_str(), 0)){
				for (unsigned i = 0; i < 27; i++){
					if (ImGui::Selectable(input_names[i].c_str())){
						current_input = i;
					}
				}
				ImGui::EndCombo();
			}
			int current_binding = -1;
			for (unsigned i = 0; i < IM_ARRAYSIZE(binding_names); i++){
				if(binding_names[i] == current_mapping[current_input]){
					current_binding = i;
				}
			}
			std::string binding_name = "";
			if(current_binding > -1){
				binding_name = binding_names[current_binding];
			}
			if (ImGui::BeginCombo("Binding", binding_name.c_str(), 0)){
				for (unsigned i = 0; i < IM_ARRAYSIZE(binding_names); i++){
					if (ImGui::Selectable(binding_names[i].c_str())){
						//current_binding = i;
						if(i == 0){
							current_mapping[current_input] = "";
						}else{
							current_mapping[current_input] = binding_names[i];
						}

						SDL_Joystick* joystick = SDL_GameControllerGetJoystick(getControllerWindow(tabs[selected_tab].ID)->sdl_controller);
						SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
						char guid_string[100] = {};
						SDL_JoystickGetGUIDString(guid, guid_string, 100);
						////std::cout << "GUID for controller is : " << guid_string << std::endl;
						std::string new_mapping = guid_string;
						new_mapping.append(",");
						new_mapping.append(SDL_GameControllerName(current_window->sdl_controller));
						new_mapping.append(",");
						//new_mapping.append(",controller,");
						for(int i = 0; i < 27; i++){
							if (current_mapping[i] != ""){
								new_mapping.append(mapping_names[i]);
								new_mapping.append(":");
								new_mapping.append(current_mapping[i]);
								new_mapping.append(",");
							}
						}
						std::cout << new_mapping << std::endl;
						SDL_GameControllerAddMapping(new_mapping.c_str());
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Save")){
                ImGui::OpenPopup("save");
			}
            if (ImGui::BeginPopup("save")){
                char name[32] = {};
                static bool name_valid = true;
                if (ImGui::InputText("Mapping Name", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue)){
                    bool valid = check_filename_valid(name);
                    name_valid = valid;

                    if (valid){
						std::filesystem::path path(SDL_GetBasePath());
						path.append("mapping/");
						std::filesystem::create_directory(path);
                        path.append(name);
						open_ofstream(path);
						//char* mapping = SDL_GameControllerMapping(getControllerWindow(tabs[selected_tab].ID)->sdl_controller);
						std::string mapping = "";
						for(int i = 0; i < 27; i++){
							if (current_mapping[i] != ""){
								mapping.append(mapping_names[i]);
								mapping.append(":");
								mapping.append(current_mapping[i]);
								mapping.append(",");
							}
						}
						write_line(mapping);
						close_ofstream();
						get_current_mapping(current_window->sdl_controller);
						ImGui::CloseCurrentPopup();
					}else{
                        std::cout << "Name contains invalid characters " << invalid_characters << std::endl;
					}
                }
                if (!name_valid){
                    ImGui::Text("Name cannot include characters \\/:*?\"<>|");
				}
                ImGui::EndPopup();
            }
			ImGui::SameLine();
			if (ImGui::Button("Load")){
				ImGui::OpenPopup("load");
			}
			if (ImGui::BeginPopup("load")){
                if(ImGui::BeginListBox("Mappings")){
					std::string dir_path = SDL_GetBasePath();
					dir_path.append("mapping/");
					std::filesystem::create_directory(dir_path);
                    struct stat sb;
					for (const auto & entry : std::filesystem::directory_iterator(dir_path)){
						std::string mapping_path = entry.path().string();
						std::string mapping_name = mapping_path;
						std::string delimiter = "/";
						if (stat(mapping_path.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR)){
							size_t pos = 0;
							while ((pos = mapping_name.find(delimiter)) != std::string::npos){
								mapping_name.erase(0, pos + delimiter.length());
							}
							if (ImGui::Selectable(mapping_name.c_str())){
								for (int i = 0; i < 27; i++){
									current_mapping[i] = "";
								}
								open_ifstream(mapping_path);
								std::vector<std::string> mapping;
								read_file(&mapping);
								std::cout << "mapping file : " << mapping[0] << std::endl;
								SDL_Joystick* joystick = SDL_GameControllerGetJoystick(getControllerWindow(tabs[selected_tab].ID)->sdl_controller);
								SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
								char guid_string[100] = {};
								SDL_JoystickGetGUIDString(guid, guid_string, 100);
								//std::cout << "GUID for controller is : " << guid_string << std::endl;
								std::string mapping_string = guid_string;
								mapping_string.append(",");
								mapping_string.append(SDL_GameControllerName(current_window->sdl_controller));
								mapping_string.append(",");
								mapping_string.append(mapping[0]);
								std::cout << "mapping_string : " << mapping_string << std::endl;
								SDL_GameControllerAddMapping(mapping_string.c_str());
								close_ifstream();
								ImGui::CloseCurrentPopup();
							}
						}
					}
					ImGui::EndListBox();
				}
                ImGui::EndPopup();
            }
			ImGui::SameLine();
			if (ImGui::Button("Default")){
                SDL_GameControllerAddMapping(current_window->default_mapping.c_str());
			}
		}
		if (ImGui::CollapsingHeader("Help")){
			ImGui::Text("3D Controller Overlay version 1.12");
			ImGui::NewLine();
			ImGui::Text("https://github.com/larfingshnew/3d-controller-overlay");
			if (ImGui::Button("Open Github Page")){
				OsOpenInShell("https://github.com/larfingshnew/3d-controller-overlay");
			}
			ImGui::NewLine();
			ImGui::Text("https://discord.gg/aKwHHvCMnS");
			if (ImGui::Button("Join Discord Server")){
				OsOpenInShell("https://discord.gg/aKwHHvCMnS");
			}
		}
	}
	
	ImGui::End();
	ImGui::PopStyleVar();

	texture_dialog.Display();
	model_dialog.Display();
	
    if (texture_dialog.HasSelected()){
		std::cout << "Selected filename : " << texture_dialog.GetSelected().string() << std::endl;
		glfwMakeContextCurrent(getControllerWindow(tabs[selected_tab].ID)->glfw_window);
		Texture t;
		loadTexture(t.id, texture_dialog.GetSelected().string());
		t.path = texture_dialog.GetSelected().string();
		t.name = std::to_string(getControllerWindow(tabs[selected_tab].ID)->model.meshes[texture_mesh].textures.size() + 1) + ": " + t.path;
		getControllerWindow(tabs[selected_tab].ID)->model.meshes[texture_mesh].textures.push_back(t);
		glfwMakeContextCurrent(glfw_settings_window);
		texture_dialog.ClearSelected();
	}

	if (model_dialog.HasSelected()){
		std::cout << "Selected filename : " << model_dialog.GetSelected().string() << std::endl;
		const auto copy_options = std::filesystem::copy_options::overwrite_existing;
        std::filesystem::path from_path = model_dialog.GetSelected();
        std::cout << "from_path : " << from_path.string() << std::endl;
		std::filesystem::path to_path = SDL_GetBasePath();
		to_path.append(getControllerWindow(tabs[selected_tab].ID)->model.path);
		to_path.append(mesh_filenames[selected_mesh]);
		std::cout << "to_path : " << to_path.string() << std::endl;
        std::filesystem::copy(from_path, to_path, copy_options);
		writeInfo(getControllerWindow(tabs[selected_tab].ID)->model, getControllerWindow(tabs[selected_tab].ID)->model.path);
		glfwMakeContextCurrent(getControllerWindow(tabs[selected_tab].ID)->glfw_window);
		loadModel(getControllerWindow(tabs[selected_tab].ID)->model, getControllerWindow(tabs[selected_tab].ID)->model.path);
		glfwMakeContextCurrent(glfw_settings_window);
		model_dialog.ClearSelected();
	}

	ImGui::Render();
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(glfw_settings_window);
	
	if(new_controller_window){
		createControllerWindow(new_tab_title, get_first_model());
		tabs.back().ID = tabs_made;
		getLastWindow()->ID = tabs_made;
	}
}

void saveTabs(){
	clear_directory("settings");
	for(window_tab t : tabs){
		writeInfo(getControllerWindow(t.ID)->model, getControllerWindow(t.ID)->model.path);

		std::string path = "settings/";
		path.append(t.title);
		open_ofstream(path);
		//Model
		write_string(std::string("model path"), getControllerWindow(t.ID)->model.path);
		//Window Settings
		write_string(std::string("title"), t.title);
		write_int(std::string("always on top"), getControllerWindow(t.ID)->always_on_top);
		write_int(std::string("borderless"), getControllerWindow(t.ID)->borderless);
		write_int(std::string("drag to move"), getControllerWindow(t.ID)->drag_to_move);
		write_int(std::string("scroll to resize"), getControllerWindow(t.ID)->scroll_to_resize);
		write_int(std::string("show grid"), getControllerWindow(t.ID)->grid);
		write_int(std::string("wireframe"), getControllerWindow(t.ID)->wireframe);
		int w = 640;
		int h = 480;
		if(!glfwGetWindowAttrib(getControllerWindow(t.ID)->glfw_window, GLFW_ICONIFIED)){
			glfwGetWindowSize(getControllerWindow(t.ID)->glfw_window, &w, &h);
		}
		write_int(std::string("width"), w);
		write_int(std::string("height"), h);
		int x = 100;
		int y = 100;
		if(!glfwGetWindowAttrib(getControllerWindow(t.ID)->glfw_window, GLFW_ICONIFIED)){
			glfwGetWindowPos(getControllerWindow(t.ID)->glfw_window, &x, &y);
		}
		write_int(std::string("x pos"), x);
		write_int(std::string("y pos"), y);
		write_int(std::string("swap interval"), getControllerWindow(t.ID)->swap_interval);
		write_int(std::string("frame cap"), getControllerWindow(t.ID)->frame_cap);
		write_float(std::string("bg red"), getControllerWindow(t.ID)->bg_color[0]);
		write_float(std::string("bg green"), getControllerWindow(t.ID)->bg_color[1]);
		write_float(std::string("bg blue"), getControllerWindow(t.ID)->bg_color[2]);
		write_float(std::string("bg alpha"), getControllerWindow(t.ID)->bg_color[3]);
		//Camera Settings
		write_int(std::string("freelook"), getControllerWindow(t.ID)->freelook);
		write_float(std::string("camera distance"), getControllerWindow(t.ID)->camera_distance);
		write_float(std::string("camera yaw"), getControllerWindow(t.ID)->camera_yaw);
		write_float(std::string("camera pitch"), getControllerWindow(t.ID)->camera_pitch);
		write_float(std::string("camera roll"), getControllerWindow(t.ID)->camera_roll);
		write_int(std::string("move speed"), getControllerWindow(t.ID)->move_speed);
		write_int(std::string("turn speed"), getControllerWindow(t.ID)->turn_speed);
		write_float(std::string("freelook yaw"), getControllerWindow(t.ID)->freelook_yaw);
		write_float(std::string("freelook pitch"), getControllerWindow(t.ID)->freelook_pitch);
		write_3_floats(std::string("freelook position"), getControllerWindow(t.ID)->freelook_position.x,
													     getControllerWindow(t.ID)->freelook_position.y,
														 getControllerWindow(t.ID)->freelook_position.z);
		//Controller Settings
		write_int(std::string("popup bumbers"), getControllerWindow(t.ID)->model.popup_bumpers);
		write_int(std::string("popup triggers"), getControllerWindow(t.ID)->model.popup_triggers);
		write_int(std::string("popup paddles"), getControllerWindow(t.ID)->model.popup_paddles);
		write_int(std::string("left stick highlight deadzone"), getControllerWindow(t.ID)->model.meshes[7].ring_highlight_deadzone);
		write_int(std::string("right stick highlight deadzone"), getControllerWindow(t.ID)->model.meshes[8].ring_highlight_deadzone);
		write_float(std::string("highlight red"), getControllerWindow(t.ID)->highlight_color[0]);
		write_float(std::string("highlight green"), getControllerWindow(t.ID)->highlight_color[1]);
		write_float(std::string("highlight blue"), getControllerWindow(t.ID)->highlight_color[2]);
		//Materials
		write_int(std::string("model meshes"), getControllerWindow(t.ID)->model.meshes.size());
		write_line(std::string("materials"));
		for(int i=0; i<(int)getControllerWindow(t.ID)->model.meshes.size(); i++){
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.ambient));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.diffuse));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.specular));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.shininess));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.color[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.color[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.color[2]));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.highlight[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.highlight[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].material.highlight[2]));
		}
		//Textures
		write_line(std::string("textures"));
		for(int i=0; i<(int)getControllerWindow(t.ID)->model.meshes.size(); i++){
			write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures.size()));
		}
		for(int i=0; i<(int)getControllerWindow(t.ID)->model.meshes.size(); i++){
			for(int j=0; j<(int)getControllerWindow(t.ID)->model.meshes[i].textures.size(); j++){
				write_line(getControllerWindow(t.ID)->model.meshes[i].textures[j].path);
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].type));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].wrapX));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].wrapY));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].offsetX));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].offsetY));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].scaleX));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].scaleY));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].rotation));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].border[0]));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].border[1]));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].border[2]));
				write_line(std::to_string(getControllerWindow(t.ID)->model.meshes[i].textures[j].border[3]));
			}
		}
		//Gyro
		write_int(std::string("gyro enabled"), getControllerWindow(t.ID)->gyro_enabled);
		write_int(std::string("reset gyro button 1"), getControllerWindow(t.ID)->reset_gyro_button1);
		write_int(std::string("reset gyro button 2"), getControllerWindow(t.ID)->reset_gyro_button2);
		write_int(std::string("gyro correction"), getControllerWindow(t.ID)->gyro_correction);
		//Lighting
		write_int(std::string("direct lights"), getControllerWindow(t.ID)->direct_lights.size());
		for(int i=0; i<(int)getControllerWindow(t.ID)->direct_lights.size(); i++){
			write_line(getControllerWindow(t.ID)->direct_lights[i].name);
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].direction[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].direction[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].direction[2]));
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].color[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].color[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->direct_lights[i].color[2]));
		}
		write_int(std::string("point lights"), getControllerWindow(t.ID)->point_lights.size());
		for(int i=0; i<(int)getControllerWindow(t.ID)->point_lights.size(); i++){
			write_line(getControllerWindow(t.ID)->point_lights[i].name);
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].hide));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].position[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].position[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].position[2]));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].intensity));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].color[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].color[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->point_lights[i].color[2]));
		}
		write_int(std::string("spot lights"), getControllerWindow(t.ID)->spot_lights.size());
		for(int i=0; i<(int)getControllerWindow(t.ID)->spot_lights.size(); i++){
			write_line(getControllerWindow(t.ID)->spot_lights[i].name);
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].hide));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].position[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].position[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].position[2]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].intensity));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].color[0]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].color[1]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].color[2]));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].yaw));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].pitch));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].cutoff));
			write_line(std::to_string(getControllerWindow(t.ID)->spot_lights[i].outer_cutoff));
		}
		close_ofstream();
	}
}

void loadTabs(){
	std::vector<std::filesystem::path> files;
	get_directory_contents(&files, "settings");
	for (std::filesystem::path path : files){
		std::string new_tab_title = "Controller ";
		window_tab new_tab;
		tabs_made++;
		new_tab_title.append(std::to_string(tabs_made));
		new_tab.title = new_tab_title;
		tabs.push_back(new_tab);
		tabs.back().ID = tabs_made;
		
		std::vector<std::string> lines;
		open_ifstream(path.c_str());
		read_file(&lines);
		close_ifstream();
		unsigned line_index = 0;

		for (std::string line : lines){
			//Model
			if (line == "model path"){
				createControllerWindow(new_tab_title, lines[line_index + 1]);
				getLastWindow()->ID = tabs_made;
				getControllerWindow(tabs.back().ID)->model_name = get_top_folder(getControllerWindow(tabs.back().ID)->model.path);
			}
			//Window Settings
			if (line == "title"){
				tabs.back().title = lines[line_index + 1];
				glfwSetWindowTitle(getControllerWindow(tabs.back().ID)->glfw_window, lines[line_index + 1].c_str());
			}
			if (line == "always on top"){
				getControllerWindow(tabs.back().ID)->always_on_top = std::stoi(lines[line_index + 1]);
				glfwSetWindowAttrib(getControllerWindow(tabs.back().ID)->glfw_window, GLFW_FLOATING, std::stoi(lines[line_index + 1]));
			}
			if (line == "borderless"){
				getControllerWindow(tabs.back().ID)->borderless = std::stoi(lines[line_index + 1]);
				glfwSetWindowAttrib(getControllerWindow(tabs.back().ID)->glfw_window, GLFW_DECORATED, !std::stoi(lines[line_index + 1]));
			}
			if (line == "drag to move")
				getControllerWindow(tabs.back().ID)->drag_to_move = std::stoi(lines[line_index + 1]);
			if (line == "scroll to resize")
				getControllerWindow(tabs.back().ID)->scroll_to_resize = std::stoi(lines[line_index + 1]);
			if (line == "show grid")
				getControllerWindow(tabs.back().ID)->grid = std::stoi(lines[line_index + 1]);
			if (line == "wireframe")
				getControllerWindow(tabs.back().ID)->wireframe = std::stoi(lines[line_index + 1]);
			if (line == "width")
				glfwSetWindowSize(getControllerWindow(tabs.back().ID)->glfw_window, 
													  std::stoi(lines[line_index + 1]), 
													  std::stoi(lines[line_index + 3]));
			if (line == "x pos")
				glfwSetWindowPos(getControllerWindow(tabs.back().ID)->glfw_window, 
													  std::stoi(lines[line_index + 1]), 
													  std::stoi(lines[line_index + 3]));
			if (line == "swap interval")
				getControllerWindow(tabs.back().ID)->swap_interval = std::stoi(lines[line_index + 1]);
			if (line == "frame cap")
				getControllerWindow(tabs.back().ID)->frame_cap = std::stoi(lines[line_index + 1]);
			if (line == "bg red")
				getControllerWindow(tabs.back().ID)->bg_color[0] = std::stof(lines[line_index + 1]);
			if (line == "bg green")
				getControllerWindow(tabs.back().ID)->bg_color[1] = std::stof(lines[line_index + 1]);
			if (line == "bg blue")
				getControllerWindow(tabs.back().ID)->bg_color[2] = std::stof(lines[line_index + 1]);
			if (line == "bg alpha")
				getControllerWindow(tabs.back().ID)->bg_color[3] = std::stof(lines[line_index + 1]);
			//Camera Settings
			if (line == "freelook")
				getControllerWindow(tabs.back().ID)->freelook = std::stoi(lines[line_index + 1]);
			if (line == "camera distance")
				getControllerWindow(tabs.back().ID)->camera_distance = std::stof(lines[line_index + 1]);
			if (line == "camera yaw")
				getControllerWindow(tabs.back().ID)->camera_yaw = std::stof(lines[line_index + 1]);
			if (line == "camera pitch")
				getControllerWindow(tabs.back().ID)->camera_pitch = std::stof(lines[line_index + 1]);
			if (line == "camera roll")
				getControllerWindow(tabs.back().ID)->camera_roll = std::stof(lines[line_index + 1]);
			if (line == "move speed")
				getControllerWindow(tabs.back().ID)->move_speed = std::stoi(lines[line_index + 1]);
			if (line == "turn speed")
				getControllerWindow(tabs.back().ID)->turn_speed = std::stoi(lines[line_index + 1]);
			if (line == "freelook yaw")
				getControllerWindow(tabs.back().ID)->freelook_yaw = std::stof(lines[line_index + 1]);
			if (line == "freelook pitch")
				getControllerWindow(tabs.back().ID)->freelook_pitch = std::stof(lines[line_index + 1]);
			if (line == "freelook position"){
				getControllerWindow(tabs.back().ID)->freelook_position.x = std::stof(lines[line_index + 1]);
				getControllerWindow(tabs.back().ID)->freelook_position.y = std::stof(lines[line_index + 2]);
				getControllerWindow(tabs.back().ID)->freelook_position.z = std::stof(lines[line_index + 3]);
			}
			//Controller Settings
			if (line == "popup bumbers")
				getControllerWindow(tabs.back().ID)->model.popup_bumpers = std::stoi(lines[line_index + 1]);
			if (line == "popup triggers")
				getControllerWindow(tabs.back().ID)->model.popup_triggers = std::stoi(lines[line_index + 1]);
			if (line == "popup paddles")
				getControllerWindow(tabs.back().ID)->model.popup_paddles = std::stoi(lines[line_index + 1]);
			if (line == "left stick highlight deadzone")
				getControllerWindow(tabs.back().ID)->model.meshes[7].ring_highlight_deadzone = std::stoi(lines[line_index + 1]);
			if (line == "right stick highlight deadzone")
				getControllerWindow(tabs.back().ID)->model.meshes[8].ring_highlight_deadzone = std::stoi(lines[line_index + 1]);
			if (line == "highlight red")
				getControllerWindow(tabs.back().ID)->highlight_color[0] = std::stof(lines[line_index + 1]);
			if (line == "highlight green")
				getControllerWindow(tabs.back().ID)->highlight_color[1] = std::stof(lines[line_index + 1]);
			if (line == "highlight blue")
				getControllerWindow(tabs.back().ID)->highlight_color[2] = std::stof(lines[line_index + 1]);
			for(int i = 3; i<32; i++){
				if(i != 5 && i != 6){
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[0] = getControllerWindow(tabs.back().ID)->highlight_color[0];
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[1] = getControllerWindow(tabs.back().ID)->highlight_color[1];
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[2] = getControllerWindow(tabs.back().ID)->highlight_color[2];
				}
			}	
			//Materials
			if (line == "materials"){
				for(int i=0; i<(int)getControllerWindow(tabs.back().ID)->model.meshes.size(); i++){
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.ambient = std::stof(lines[line_index + 1 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.diffuse = std::stof(lines[line_index + 2 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.specular = std::stof(lines[line_index + 3 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.shininess = std::stof(lines[line_index + 4 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.color[0] = std::stof(lines[line_index + 5 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.color[1] = std::stof(lines[line_index + 6 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.color[2] = std::stof(lines[line_index + 7 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[0] = std::stof(lines[line_index + 8 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[1] = std::stof(lines[line_index + 9 + (i*10)]);
					getControllerWindow(tabs.back().ID)->model.meshes[i].material.highlight[2] = std::stof(lines[line_index + 10 + (i*10)]);
				}
			}
			//Textures
			if (line == "textures"){
				unsigned texture_count = 0;
				for(int mesh=0; mesh<(int)getControllerWindow(tabs.back().ID)->model.meshes.size(); mesh++){
					int mesh_textures = std::stoi(lines[line_index + 1 + mesh]);
					for(int i=0; i<mesh_textures; i++){
						unsigned texture_line = line_index + (int)getControllerWindow(tabs.back().ID)->model.meshes.size() + 1 + texture_count*13;
						Texture t;
						loadTexture(t.id, lines[texture_line]);
						t.path = lines[texture_line];
						t.name = lines[texture_line];
						t.type = std::stoi(lines[texture_line + 1]);
						t.wrapX = std::stoi(lines[texture_line + 2]);
						t.wrapY = std::stoi(lines[texture_line + 3]);
						t.offsetX = std::stof(lines[texture_line + 4]);
						t.offsetY = std::stof(lines[texture_line + 5]);
						t.scaleX = std::stof(lines[texture_line + 6]);
						t.scaleY = std::stof(lines[texture_line + 7]);
						t.rotation = std::stof(lines[texture_line + 8]);
						t.border[0] = std::stof(lines[texture_line + 9]);
						t.border[1] = std::stof(lines[texture_line + 10]);
						t.border[2] = std::stof(lines[texture_line + 11]);
						t.border[3] = std::stof(lines[texture_line + 12]);
						texture_count++;
						glBindTexture(GL_TEXTURE_2D, t.id);
						enum Wrap { repeat, mirror_repeat, clamp_edge, clamp_border, wrap_count };
						switch(t.wrapX){
							case repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
								break;
							case mirror_repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
								break;
							case clamp_edge : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
								break;
							case clamp_border : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
								break;
						}
						switch(t.wrapY){
							case repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
								break;
							case mirror_repeat : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
								break;
							case clamp_edge : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
								break;
							case clamp_border : 
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
								break;
						}
						glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, t.border);
						getControllerWindow(tabs.back().ID)->model.meshes[mesh].textures.push_back(t);
					}
				}
			}
			//Motion Settings
			if (line == "gyro enabled"){
				getControllerWindow(tabs.back().ID)->gyro_enabled = std::stoi(lines[line_index + 1]);
				SDL_GameControllerSetSensorEnabled(getControllerWindow(tabs.back().ID)->sdl_controller, SDL_SENSOR_GYRO, (SDL_bool)getControllerWindow(tabs.back().ID)->gyro_enabled);
			}
			if (line == "reset gyro button 1")
				getControllerWindow(tabs.back().ID)->reset_gyro_button1 = std::stoi(lines[line_index + 1]);
			if (line == "reset gyro button 2")
				getControllerWindow(tabs.back().ID)->reset_gyro_button2 = std::stoi(lines[line_index + 1]);	
			if (line == "gyro correction")
				getControllerWindow(tabs.back().ID)->gyro_correction = std::stoi(lines[line_index + 1]);
			line_index++;
			//Direct Lights
			if (line == "direct lights"){
				getControllerWindow(tabs.back().ID)->direct_lights.clear();
				int lights = std::stoi(lines[line_index]);
				for (int i = 0; i<lights; i++){
					direct_light new_light;
					new_light.name = lines[line_index + (i*7) + 1];
					new_light.direction[0] = std::stof(lines[line_index + (i*7) + 2]);
					new_light.direction[1] = std::stof(lines[line_index + (i*7) + 3]);
					new_light.direction[2] = std::stof(lines[line_index + (i*7) + 4]);
					new_light.color[0] = std::stof(lines[line_index + (i*7) + 5]);
					new_light.color[1] = std::stof(lines[line_index + (i*7) + 6]);
					new_light.color[2] = std::stof(lines[line_index + (i*7) + 7]);
					getControllerWindow(tabs.back().ID)->direct_lights.push_back(new_light);
				}
			}
			//Point Lights
			if (line == "point lights"){
				getControllerWindow(tabs.back().ID)->point_lights.clear();
				int lights = std::stoi(lines[line_index]);
				for (int i = 0; i<lights; i++){
					point_light new_light;
					new_light.name = lines[line_index + (i*9) + 1];
					new_light.hide = (bool)std::stoi(lines[line_index + (i*9) + 2]);
					new_light.position[0] = std::stof(lines[line_index + (i*9) + 3]);
					new_light.position[1] = std::stof(lines[line_index + (i*9) + 4]);
					new_light.position[2] = std::stof(lines[line_index + (i*9) + 5]);
					new_light.intensity = std::stof(lines[line_index + (i*9) + 6]);
					new_light.color[0] = std::stof(lines[line_index + (i*9) + 7]);
					new_light.color[1] = std::stof(lines[line_index + (i*9) + 8]);
					new_light.color[2] = std::stof(lines[line_index + (i*9) + 9]);
					new_light.ambient.r = new_light.color[0] * 0.05f;
					new_light.ambient.g = new_light.color[1] * 0.05f;
					new_light.ambient.b = new_light.color[2] * 0.05f;
					new_light.diffuse.r = new_light.color[0] * 0.8f;
					new_light.diffuse.g = new_light.color[1] * 0.8f;
					new_light.diffuse.b = new_light.color[2] * 0.8f;
					new_light.specular.r = new_light.color[0];
					new_light.specular.g = new_light.color[1];
					new_light.specular.b = new_light.color[2];
					getControllerWindow(tabs.back().ID)->point_lights.push_back(new_light);
				}
			}
			//Spot Lights
			if (line == "spot lights"){
				getControllerWindow(tabs.back().ID)->spot_lights.clear();
				int lights = std::stoi(lines[line_index]);
				for (int i = 0; i<lights; i++){
					spot_light new_light;
					new_light.name = lines[line_index + (i*13) + 1];
					new_light.hide = (bool)std::stoi(lines[line_index + (i*13) + 2]);
					new_light.position[0] = std::stof(lines[line_index + (i*13) + 3]);
					new_light.position[1] = std::stof(lines[line_index + (i*13) + 4]);
					new_light.position[2] = std::stof(lines[line_index + (i*13) + 5]);
					new_light.intensity = std::stof(lines[line_index + (i*13) + 6]);
					new_light.color[0] = std::stof(lines[line_index + (i*13) + 7]);
					new_light.color[1] = std::stof(lines[line_index + (i*13) + 8]);
					new_light.color[2] = std::stof(lines[line_index + (i*13) + 9]);
					new_light.ambient.r = new_light.color[0] * 0.05f;
					new_light.ambient.g = new_light.color[1] * 0.05f;
					new_light.ambient.b = new_light.color[2] * 0.05f;
					new_light.diffuse.r = new_light.color[0] * 0.8f;
					new_light.diffuse.g = new_light.color[1] * 0.8f;
					new_light.diffuse.b = new_light.color[2] * 0.8f;
					new_light.specular.r = new_light.color[0];
					new_light.specular.g = new_light.color[1];
					new_light.specular.b = new_light.color[2];
					new_light.yaw = std::stof(lines[line_index + (i*13) + 10]);
					new_light.pitch = std::stof(lines[line_index + (i*13) + 11]);
					new_light.direction.x = cos(glm::radians(new_light.pitch)) * sin(glm::radians(new_light.yaw + 180));
					new_light.direction.y = sin(glm::radians(new_light.pitch));
					new_light.direction.z = cos(glm::radians(new_light.pitch)) * cos(glm::radians(new_light.yaw + 180));
					new_light.cutoff = std::stof(lines[line_index + (i*13) + 12]);
					new_light.outer_cutoff = std::stof(lines[line_index + (i*13) + 13]);
					getControllerWindow(tabs.back().ID)->spot_lights.push_back(new_light);
				}
			}
		}
	}
}

bool check_filename_valid(const char* name)
{
    bool valid = true;
    for (int i = 0; i < 32; i++)
    {
        //std::cout << "name[" << i << "] : " << name[i] << std::endl;
        for (char c : invalid_characters)
        {
            //std::cout << c << std::endl;
            if (name[i] == c)
            {
                valid = false;
                break;
            }
        }
        if (!valid)
            break;
    }
    return valid;
}

std::string get_top_folder(std::string path){
	std::string delimiter = "/";
	std::string dir = path;
	struct stat sb;
	if (stat(dir.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)){
		size_t pos = 0;
		while ((pos = dir.find(delimiter)) != std::string::npos) {
			dir.erase(0, pos + delimiter.length());
		}
	}
	return dir;
}

std::vector<std::string> get_binding(std::string b){
	std::vector<std::string> binding;
	std::stringstream line_stream(b);
	std::string word;
	while (std::getline(line_stream, word, ':')){
		binding.push_back(word);
	}
	return binding;
}

std::vector<std::string> get_current_mapping(SDL_GameController* sdl_controller){
	std::vector<std::string> mapping;
	if(SDL_GameControllerMapping(sdl_controller)){
		char* current_mapping = SDL_GameControllerMapping(sdl_controller);
		//std::cout << current_mapping << std::endl;
		std::stringstream line_stream(current_mapping);
		std::string word;
		while (std::getline(line_stream, word, ',')){
		    mapping.push_back(word);
		}
		//mapping.erase(mapping.begin());
		//mapping.erase(mapping.begin());
		mapping.erase(mapping.end());
		mapping.erase(mapping.end());
	}
	return mapping;
}

std::string get_first_model(){
	std::string dir_path = SDL_GetBasePath();
	dir_path.append("models/");
	std::vector<std::string> model_folders;
	struct stat sb;
	for (const auto & entry : std::filesystem::directory_iterator(dir_path)){
		if (stat(entry.path().string().c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)){
			model_folders.push_back(entry.path().string());
		}
	}
	return model_folders.front();
}

void OsOpenInShell(const char* path){
	std::string open_executable = "";
	
	#if defined(__linux__) 
	open_executable = "xdg-open";
	#elif __FreeBSD__
	#elif __ANDROID__
	#elif __APPLE__
	open_executable = "open";
	#elif _WIN32
	::ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
	#else //some other operating system
	#endif

	char command[256];
	snprintf(command, 256, "%s \"%s\"", open_executable.c_str(), path);
	system(command);
}
