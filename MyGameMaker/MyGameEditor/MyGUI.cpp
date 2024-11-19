#include "MyGUI.h"
#include "App.h"
#include "Module.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_internal.h>	
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <iostream>
#include "MyWindow.h"
#include <imGuizmo.h>

#include "UIElement.h"
#include "UIConsole.h"
#include "UISettings.h"
#include "UIMainMenuBar.h"
#include "UIInspector.h"
#include "UIHierarchy.h"
#include "UISceneWindow.h"


MyGUI::MyGUI(App* app) : Module(app) {
	ImGui::CreateContext();
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
	LOG(LogType::LOG_INFO, "UI Awake");

	bool ret = true;

	UIHierarchyPanel = new UIHierarchy(UIType::HIERARCHY, "Hierarchy");
	elements.push_back(UIHierarchyPanel);
	ret *= isInitialized(UIHierarchyPanel);

	UIconsolePanel = new UIConsole(UIType::CONSOLE, "Console");
	elements.push_back(UIconsolePanel);
	ret *= isInitialized(UIconsolePanel);

	UIsettingsPanel = new UISettings(UIType::SETTINGS, "Settings");
	elements.push_back(UIsettingsPanel);
	ret *= isInitialized(UIsettingsPanel);

	UIMainMenuBarPanel = new UIMainMenuBar(UIType::DEFAULT, "MainMenuBar");
	elements.push_back(UIMainMenuBarPanel);
	ret *= isInitialized(UIMainMenuBarPanel);

	UIinspectorPanel = new UIInspector(UIType::INSPECTOR, "Inspector");
	elements.push_back(UIinspectorPanel);
	ret *= isInitialized(UIinspectorPanel);

	UISceneWindowPanel = new UISceneWindow(UIType::DEFAULT, "SceneWindow");
	elements.push_back(UISceneWindowPanel);
	ret *= isInitialized(UISceneWindowPanel);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(5, 5);

	return ret;
}

bool MyGUI::isInitialized(UIElement* element) {
	if (!element)
	{
		LOG(LogType::LOG_ERROR, "-%s", element->GetName().c_str());
		return false;
	}
	LOG(LogType::LOG_INFO, "+%s", element->GetName().c_str());
	return true;
}


bool MyGUI::Start() {

	LOG(LogType::LOG_INFO, "Initializing ImGui/ImPlot...");

	IMGUI_CHECKVERSION();
	LOG(LogType::LOG_OK, "-ImGui Version: %s", IMGUI_VERSION);

	ImGuiContext* contextGUI = ImGui::CreateContext();

	if (!contextGUI)
	{
		LOG(LogType::LOG_ERROR, "-ImGui Context not created");
	}
	else
	{
		LOG(LogType::LOG_OK, "-ImGui Context created");
	}


	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if (!&io)
	{
		LOG(LogType::LOG_ERROR, "-ImGui IO not created");
	}
	else
	{
		LOG(LogType::LOG_OK, "-ImGui IO created");
	}

	ImGui::StyleColorsClassic();

	Application->gui->UIconsolePanel->SetState(true);
	Application->gui->UIsettingsPanel->SetState(true);
	Application->gui->UIinspectorPanel->SetState(true);
	Application->gui->UIMainMenuBarPanel->SetState(true);
	Application->gui->UISceneWindowPanel->SetState(true);
	UISceneWindowPanel->Init();

	return true;
}


bool MyGUI::PreUpdate() {


	return true;
}

bool MyGUI::Update(double dt)
{
	return true;
}

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

void MyGUI::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (showHierarchy) {
		UIHierarchyPanel->Draw();
	}

	if (showConsole) {
		UIconsolePanel->Draw();
	}

	if (showSettings) {
		UIsettingsPanel->Draw();
	}

	if (showMainMenuBar) {
		UIMainMenuBarPanel->Draw();
	}

	if (showInspector) {
		UIinspectorPanel->Draw();
	}

	if (showSceneWindow) {
		UISceneWindowPanel->Draw();
	}



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}