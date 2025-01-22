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
#include "UIProject.h"
#include "UICamera.h"

MyGUI::MyGUI(App* app) : Module(app) {
	ImGui::CreateContext();

	SetColorScheme();
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
	ret = isInitialized(UIHierarchyPanel);

	UIconsolePanel = new UIConsole(UIType::CONSOLE, "Console");
	elements.push_back(UIconsolePanel);
	ret = isInitialized(UIconsolePanel);

	UIsettingsPanel = new UISettings(UIType::SETTINGS, "Settings");
	elements.push_back(UIsettingsPanel);
	ret = isInitialized(UIsettingsPanel);

	UIMainMenuBarPanel = new UIMainMenuBar(UIType::DEFAULT, "MainMenuBar");
	elements.push_back(UIMainMenuBarPanel);
	ret = isInitialized(UIMainMenuBarPanel);

	UIinspectorPanel = new UIInspector(UIType::INSPECTOR, "Inspector");
	elements.push_back(UIinspectorPanel);
	ret = isInitialized(UIinspectorPanel);

	UIProjectPanel = new UIProject(UIType::PROJECT, "Project");
	elements.push_back(UIProjectPanel);
	ret = isInitialized(UIProjectPanel);

	UISceneWindowPanel = new UISceneWindow(UIType::DEFAULT, "SceneWindow");
	elements.push_back(UISceneWindowPanel);
	ret = isInitialized(UISceneWindowPanel);

	UICameraPanel = new UICamera(UIType::CAMERA, "Camera");
	elements.push_back(UICameraPanel);
	ret = isInitialized(UICameraPanel);

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

	if (!ImGui::GetCurrentContext)
	{
		LOG(LogType::LOG_ERROR, "-ImGui Context not created");
	}
	else
	{
		LOG(LogType::LOG_OK, "-ImGui Context created");
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("EngineAssets/SF-Pro-Text-Light.otf", 14.0f);
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

	Application->gui->UIconsolePanel->SetState(true);
	Application->gui->UIProjectPanel->SetState(true);
	Application->gui->UIsettingsPanel->SetState(true);
	Application->gui->UIinspectorPanel->SetState(true);
	Application->gui->UIMainMenuBarPanel->SetState(true);
	Application->gui->UISceneWindowPanel->SetState(true);
	//Application->gui->UICameraPanel->SetState(true);
	UISceneWindowPanel->Init();
	//UICameraPanel->Init();

	return true;
}


bool MyGUI::PreUpdate() 
{
	return true;
}

bool MyGUI::Update(double dt)
{
	return true;
}

bool MyGUI::PostUpdate() 
{
	return true;
}

bool MyGUI::CleanUp() { return true; }

void MyGUI::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Docking
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_UnsavedDocument |
		ImGuiDockNodeFlags_AutoHideTabBar |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();

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
		UIinspectorPanel->Draw(); // Ojo que minimizar peta el engine por el inspector
	}

	if (showSceneWindow) {
		UISceneWindowPanel->Draw();
	}

	if (showProject) {
		UIProjectPanel->Draw();
	}

	//if (showCamera) {
	//	UICameraPanel->Draw();
	//}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MyGUI::SetColorScheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	// Example style customizations
	style.WindowRounding = 5.0f;
	style.FrameRounding = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.FramePadding = ImVec2(10, 5);
	style.ItemSpacing = ImVec2(5, 5);
	style.ScrollbarSize = 15.0f;                                            // Size of scrollbar

	// Set colors
	ImVec4* colors = ImGui::GetStyle().Colors;

	// Text
	colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Backgrounds
	colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);             // Background color for main windows
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);              // Background color for child windows/panels
	colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);              // Background color for pop-up windows

	// Headers
	colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);               // Background color for headers (hovered or active)
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);        // Color when header is hovered
	colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);         // Color when header is active
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);			// Background color for menu bars

	// Borders and separators
	colors[ImGuiCol_Border] = ImVec4(0.05f, 0.05f, 0.05f, 0.7f);            // Border color
	colors[ImGuiCol_BorderShadow] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);			// Border shadow color

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);               // Button color
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);		// Button color when hovered
	colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);         // Button color when active

	// Frame background (used for inputs, sliders, etc.)
	colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);              // Background color for frames
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);      // Frame color when hovered
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);        // Frame color when active

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);					// Background color for tabs
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);        // Tab color when hovered
	colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);            // Tab color when active
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);      // Tab color when unfocused
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);   // Active tab color when unfocused

	// Titles
	colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);				// Background color for title bar
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);		// Title bar color when active
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);     // Title bar color when collapsed

	// Scrollbars
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.5f);        // Background color for scrollbars
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);        // Scrollbar grab color
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Scrollbar grab color when hovered
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Scrollbar grab color when active

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);           // Color for sliders
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);     // Color when slider is active

	// Checkmark
	colors[ImGuiCol_CheckMark] = ImVec4(0.1f, 0.9f, 0.4f, 1.0f);            // Color for checkmark

	// Separators
	colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);         // Separator color
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);     // Separator color when hovered
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);      // Separator color when active

	// Resize grip (used for resizable windows)
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);        // Resize grip color
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);    // Resize grip color when hovered
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);     // Resize grip color when active

	// Plot lines and histogram
	colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);            // Color for plot lines
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.6f, 0.1f, 1.0f);     // Plot lines when hovered
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.61f, 0.76f, 0.51f, 0.75f);     // Color for histogram plots
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.8f, 0.3f, 1.0f); // Histogram color when hovered

	// Modal window darkening
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);     // Dim background for modal windows

	// Table
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);        // Background for table headers
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);    // Strong border for tables
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);     // Light border for tables
	colors[ImGuiCol_TableRowBg] = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);          // Row background for tables
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.15f, 0.15f, 0.2f, 1.0f);      // Alternate row background for tables

	// Set the main docking background color
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);       // Transparent background

	// Change the color of the docking preview area
	colors[ImGuiCol_DockingPreview] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);       // Highlight color during docking
}