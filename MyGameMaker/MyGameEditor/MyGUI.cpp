#include "MyGUI.h"
#include "App.h"
#include "Module.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <vector>


MyGUI::MyGUI(App* app) : Module(app) {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(app->window->windowPtr(), app->window->contextPtr());
	ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


struct SceneObject {
	const char* name;
	std::vector<SceneObject> children; // List of child objects
};

bool MyGUI::Awake() {

	// Example UI

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

void RenderSceneHierarchy(std::list< std::shared_ptr<GameObject>>& objects);

bool MyGUI::PostUpdate() { 
	
	/*ImGui::Render();*/
	//glViewport(100, 300, 100, 100); // Set your viewport
	//glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // Clear color
	//glClear(GL_COLOR_BUFFER_BIT);

	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//if (Application) { SDL_GL_SwapWindow(Application->window->windowPtr()); } // Swap the window buffer


	return true; 

}
bool MyGUI::CleanUp() { return true; }




std::vector<SceneObject> sceneObjects = {
	{"Object A", {{"Child A1", {}}, {"Child A2", {}}}}, {"Pau"},
	{"Object B", {{"Child B1", {}}}}, {"Pau2"}
};

void MyGUI::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	RenderSceneHierarchy(Application->root->children);

	//ImGui::ShowDemoWindow();
	//with ImGui create a top horizontal menu bar
	if (ImGui::BeginMainMenuBar()) {
		// Inicia el menú "File"


		if (ImGui::BeginMenu("not File")) {

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

void DrawSceneObject(GameObject& obj);

void RenderSceneHierarchy(std::list< std::shared_ptr<GameObject>>  &objects) {
	ImGui::Begin("Scene Hierarchy"); 

	for (auto& obj : objects) {
		DrawSceneObject(*obj); // Draw each object in the scene
	}

	ImGui::End(); 
}

void DrawSceneObject(GameObject& obj) {
	// Create a tree node for the current object
	bool open = ImGui::TreeNode(obj.GetName().c_str()); 

	if (open) {
		// If the node is open, draw its children
		for (auto& child : obj.children) {
			DrawSceneObject(*child); // Recursively draw children
		}
		ImGui::TreePop(); 
	}

	ImGui::SameLine(); // Place a button next to the tree node
	if (ImGui::Button("Remove")) {
		



	}
}

