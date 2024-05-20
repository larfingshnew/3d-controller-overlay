#if defined(__linux__) 
#elif __FreeBSD__
#elif __ANDROID__
#elif __APPLE__
#elif _WIN32
#define SDL_MAIN_HANDLED
#else //some other operating system
#endif

#include "settings_window.h"

bool gQuit = false;

void InitializeProgram(){
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0){
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }
	
	createSettingsWindow();

	loadTabs();
}

void Input(){
	glfwPollEvents();
	
	settings_window_input(gQuit);
	controller_window_input();
	
	SDL_Event event;
	while (SDL_PollEvent(&event)){
    	settings_sdl_events(&event);	
		controller_sdl_events(&event);
	}
}

void Draw(){
	drawSettingsWindow();
	drawControllerWindows();
}

void MainLoop(){
	while(!gQuit){
		Input();

		Draw();
	}
}

void Cleanup(){
	saveTabs();

	removeSettingsWindow();
	
	destroyWindows();
	
	SDL_Quit();

	glfwTerminate();
}

int main(){
	InitializeProgram();
	
	MainLoop();

	Cleanup();

	return 0;
}
