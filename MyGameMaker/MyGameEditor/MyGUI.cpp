#include "MyGUI.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>


MyGUI::MyGUI(SDL_Window* window, void* context) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void MyGUI::render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	//with ImGui create a top horizontal menu bar
	if (ImGui::BeginMainMenuBar()) {
		// Inicia el menú "File"
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Import3DModel")) { /* Lógica para crear un nuevo archivo */ }
			ImGui::EndMenu();
		}
		// Finaliza la barra de menú principal
		ImGui::EndMainMenuBar();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}