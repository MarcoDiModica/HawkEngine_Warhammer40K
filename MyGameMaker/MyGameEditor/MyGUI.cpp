#include "MyGUI.h"
#include "App.h"
#include "Module.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <vector>


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


bool MyGUI::Awake() {
	return true;
}


bool MyGUI::Start() {


	return true;
}


bool MyGUI::PreUpdate() { 
	

	return true;
}
bool MyGUI::Update(double dt) { 

	//Render();

	////move the transform of the camera
	//


	//if (ImGui::IsKeyDown(ImGuiKey_W)) {
	//	camera.transform().translate(-camera.transform().fwd() * 0.1);
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_S)) camera.transform().translate(camera.transform().fwd() * 0.1);
	//if (ImGui::IsKeyDown(ImGuiKey_A)) camera.transform().translate(-camera.transform().left() * 0.1);
	//if (ImGui::IsKeyDown(ImGuiKey_D)) camera.transform().translate(camera.transform().left() * 0.1);
	//if (ImGui::IsKeyDown(ImGuiKey_Q)) camera.transform().translate(-camera.transform().up() * 0.1);
	//if (ImGui::IsKeyDown(ImGuiKey_E)) camera.transform().translate(camera.transform().up() * 0.1);
	////rotate the transform of the camera
	//
	//if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
	//	camera.transform().rotate(0.02, vec3(0, 1, 0));
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
	//	camera.transform().rotate(-0.02, vec3(0, 1, 0));
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) camera.transform().rotate(0.02, camera.transform().left());
	//if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) camera.transform().rotate(-0.02, camera.transform().left());




	//Application->window->SwapBuffers();

	return true; 

}


bool MyGUI::PostUpdate() { 
	
	


	return true; 

}
bool MyGUI::CleanUp() { return true; }


void MyGUI::Render() {
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