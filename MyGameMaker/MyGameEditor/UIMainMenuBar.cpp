#include "UIMainMenuBar.h"
#include "App.h"
#include "MyGUI.h"
#include <imgui.h>
#include "Input.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Image.h"
#include "Log.h"
#include "UIConsole.h"
#include "UISettings.h"
#include "UIInspector.h"
#include "UIHierarchy.h"
#include <SDL2/SDL.h>


//libraries to open websites
#include <windows.h>
#include <shellapi.h>


void SetColorScheme();
void SetRedStyle();

UIMainMenuBar::UIMainMenuBar(UIType type, std::string name) : UIElement(type, name)
{

	play_image.LoadTextureLocalPath("EngineAssets/plabtn.png");
	stop_image.LoadTextureLocalPath("EngineAssets/stobtn.png");

}

UIMainMenuBar::~UIMainMenuBar()
{
}

bool UIMainMenuBar::Draw()
{
	SetColorScheme();
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(10.0f, 10.f);

	if (ImGui::BeginMainMenuBar()) {
		// Inicia el men� "General"

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene")) {
				Application->scene_serializer->Serialize("Library/Scenes");
			}
			if (ImGui::MenuItem("New Scene")) {
				std::string sceneName = "NewScene" + std::to_string(newSceneCount);
				Application->root->CreateScene(sceneName);
				Application->root->SetActiveScene(sceneName);
				newSceneCount++;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("General"))
		{

			if (ImGui::MenuItem("About")) { ShellExecute(0, 0, "https://github.com/CITM-UPC/HawkEngine", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Quit")) {
				SDL_Quit();
				exit(0);
			}
			//Aqui abajo mas botones para esconder las diferentes ventanas de Imgui
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{

			if (ImGui::MenuItem("EmptyGameObject")) { Application->root->CreateGameObject("EmptyGameObject"); }
			if (ImGui::MenuItem("Cube")) { Application->root->CreateCube("Cube"); }
			if (ImGui::MenuItem("Sphere")) { Application->root->CreateSphere("Sphere"); }
			if (ImGui::MenuItem("Plane")) { Application->root->CreatePlane("Plane"); }
			if (ImGui::MenuItem("Camera")) { Application->root->CreateCameraObject("Camera"); }
			if (ImGui::MenuItem("Light")) { Application->root->CreateLightObject("Light"); }
			if (ImGui::MenuItem("Canvas")) { Application->root->CreateCanvasObject("Canvas"); }
			if (ImGui::MenuItem("Button")) { Application->root->CreateButtonObject("Button"); }
			if (ImGui::MenuItem("Crosshair")) { Application->root->CreateImageObject("Crosshair"); }
			if (ImGui::MenuItem("CheckBox")) { Application->root->CreateCheckBoxObject("CheckBox"); }
			if (ImGui::MenuItem("InputBox")) { Application->root->CreateInputBoxObject("InputBox"); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Hierarchy")) { Application->gui->showHierarchy = !Application->gui->showHierarchy; }
			if (ImGui::MenuItem("Console")) { Application->gui->showConsole = !Application->gui->showConsole; }
			if (ImGui::MenuItem("Settings")) { Application->gui->showSettings = !Application->gui->showSettings; }
			if (ImGui::MenuItem("Inspector")) { Application->gui->showInspector = !Application->gui->showInspector; }
			if (ImGui::MenuItem("Audio Test", NULL, &Application->gui->showAudioTest)) {}

			ImGui::EndMenu();
		}
		//---------Play and Stop Button----------//
		if (Application->play == false) {

			if (ImGui::ImageButton("Play Button", reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(play_image.id())), ImVec2(11.0f, 11.0f))) 
			{
				pressing_play = true;
			}
			else if (pressing_play) {
				pressing_play = false;
				Application->play = true;
				Application->scene_serializer->Serialize(std::string(""), true);
			}
		}
		else {
			SetRedStyle();
			if (ImGui::ImageButton("Stop Button", reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(stop_image.id())), ImVec2(11.0f, 11.0f))) 
			{
				Application->scene_serializer->DeSerialize("EngineAssets/" + Application->root->currentScene->GetName() +".scene");
				Application->play = false;
			}
		}

		// Finaliza la barra de men� principal
		ImGui::EndMainMenuBar();
	}

	return true;
}

void SetColorScheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	ImGui::StyleColorsDark();

	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.12f, 1.0f); 

	colors[ImGuiCol_Text] = ImVec4(0.93f, 0.93f, 0.93f, 1.0f); 

	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f); 
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.23f, 0.23f, 0.23f, 1.0f);  
	colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);  

	colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f); 
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f); 

	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);  
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.17f, 0.17f, 0.22f, 1.0f); 
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);  

	colors[ImGuiCol_Border] = ImVec4(0.48f, 0.48f, 0.48f, 1.0f); 

	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f); 
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.35f, 1.0f); 
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.45f, 1.0f); 
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.0f); 

	colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.26f, 1.0f); 
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.0f);  
	colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.33f, 0.43f, 1.0f);  
}

void SetRedStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Add red tint to everything manually
	colors[ImGuiCol_WindowBg] = ImVec4(0.29f, 0.09f, 0.12f, 1.0f);  
	colors[ImGuiCol_Text] = ImVec4(1.13f, 0.93f, 0.93f, 1.0f); 
	colors[ImGuiCol_Button] = ImVec4(0.33f, 0.13f, 0.13f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.23f, 0.23f, 1.0f);  
	colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.18f, 0.18f, 1.0f);  

	colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.1f, 0.2f, 1.0f); 
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.2f, 0.3f, 1.0f);  
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.5f, 0.3f, 0.4f, 1.0f);  

	colors[ImGuiCol_TitleBg] = ImVec4(0.32f, 0.12f, 0.18f, 1.0f);  
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.37f, 0.17f, 0.22f, 1.0f); 
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.1f, 0.15f, 1.0f);  

	colors[ImGuiCol_Border] = ImVec4(0.68f, 0.48f, 0.48f, 1.0f); 

	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.35f, 0.15f, 0.22f, 1.0f);  
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.45f, 0.25f, 0.35f, 1.0f);  
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.35f, 0.45f, 1.0f); 
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.45f, 0.55f, 1.0f); 

	colors[ImGuiCol_Header] = ImVec4(0.36f, 0.16f, 0.26f, 1.0f); 
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.25f, 0.35f, 1.0f);  
	colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.33f, 0.43f, 1.0f);  
}
