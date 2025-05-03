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
#include "UIAudioTest.h"
#include "UITextEditor.h"
#include "UIGameView.h"
#include "RenderStats.h"
#include "RenderManager.h"

// Cache for ImGui colors and styles to minimize state changes
struct ImGuiStyleCache {
	ImVec4 colors[ImGuiCol_COUNT];
	ImGuiStyle style;
	bool initialized = false;

	void Initialize() {
		if (initialized) {
			return;
		}

		ImGuiStyle& currentStyle = ImGui::GetStyle();

		// Cache all style values
		style = currentStyle;

		// Cache all colors
		memcpy(colors, currentStyle.Colors, sizeof(ImVec4) * ImGuiCol_COUNT);

		initialized = true;
	}

	void ApplyStyle() {
		if (!initialized) {
			Initialize();
			return;
		}

		ImGuiStyle& currentStyle = ImGui::GetStyle();

		// Apply cached style values
		currentStyle.WindowRounding = style.WindowRounding;
		currentStyle.FrameRounding = style.FrameRounding;
		currentStyle.ScrollbarRounding = style.ScrollbarRounding;
		currentStyle.FramePadding = style.FramePadding;
		currentStyle.ItemSpacing = style.ItemSpacing;
		currentStyle.ScrollbarSize = style.ScrollbarSize;
		currentStyle.WindowPadding = style.WindowPadding;
		currentStyle.GrabRounding = style.GrabRounding;
		currentStyle.GrabMinSize = style.GrabMinSize;

		// Apply cached colors
		memcpy(currentStyle.Colors, colors, sizeof(ImVec4) * ImGuiCol_COUNT);
	}
};

// UI state tracking
struct UIState {
	bool showHierarchy = true;
	bool showInspector = true;
	bool showConsole = true;
	bool showSettings = false;
	bool showMainMenuBar = true;
	bool showSceneWindow = true;
	bool showProject = true;
	bool showTextEditor = false;
	bool showGameView = true;
	bool layoutChanged = false;

	// Compare with current state and determine if changed
	bool HasChanged(const MyGUI* gui) const {
		return showHierarchy != gui->showHierarchy ||
			showInspector != gui->showInspector ||
			showConsole != gui->showConsole ||
			showSettings != gui->showSettings ||
			showMainMenuBar != gui->showMainMenuBar ||
			showSceneWindow != gui->showSceneWindow ||
			showProject != gui->showProject ||
			showTextEditor != gui->showTextEditor ||
			showGameView != gui->showGameView;
	}

	// Update state from current GUI
	void UpdateFrom(const MyGUI* gui) {
		showHierarchy = gui->showHierarchy;
		showInspector = gui->showInspector;
		showConsole = gui->showConsole;
		showSettings = gui->showSettings;
		showMainMenuBar = gui->showMainMenuBar;
		showSceneWindow = gui->showSceneWindow;
		showProject = gui->showProject;
		showTextEditor = gui->showTextEditor;
		showGameView = gui->showGameView;
		layoutChanged = true;
	}
};

// Font cache
struct FontCache {
	ImFont* mainFont = nullptr;
	ImFont* headerFont = nullptr;
	ImFont* smallFont = nullptr;
	bool initialized = false;
};

// Static caches
static ImGuiStyleCache styleCache;
static UIState uiState;
static FontCache fontCache;

MyGUI::MyGUI(App* app) : Module(app) {
	ImGui::CreateContext();
	SetColorScheme();
	ImGui_ImplSDL2_InitForOpenGL(app->window->windowPtr(), app->window->contextPtr());
	ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
	for (auto& element : elements)
	{
		if (element)
		{
			delete element;
			element = nullptr;
		}
	}

	elements.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

struct SceneObject {
	const char* name;
	std::vector<SceneObject> children;
};

bool MyGUI::Awake() {
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

	UITextEditorPanel = new UITextEditor(UIType::TEXTEDITOR, "TextEditor");
	elements.push_back(UITextEditorPanel);
	ret = isInitialized(UITextEditorPanel);

	UIGameViewPanel = new UIGameView(UIType::DEFAULT, "GameView");
	elements.push_back(UIGameViewPanel);
	ret = isInitialized(UIGameViewPanel);

	// Cache initial UI state
	uiState.UpdateFrom(this);

	return ret;
}

bool MyGUI::isInitialized(UIElement* element) {
	if (!element)
	{
		LOG(LogType::LOG_ERROR, "-%s", element->GetName().c_str());
		return false;
	}
	return true;
}


bool MyGUI::Start() {
	IMGUI_CHECKVERSION();

	if (!ImGui::GetCurrentContext) {
		LOG(LogType::LOG_ERROR, "-ImGui Context not created");
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Initialize font cache
	if (!fontCache.initialized) {
		fontCache.mainFont = io.Fonts->AddFontFromFileTTF("EngineAssets/Rubik-Regular.ttf", 14.0f);
		if (fontCache.mainFont == nullptr) {
			LOG(LogType::LOG_WARNING, "Failed to load main font, using default font");
			fontCache.mainFont = io.Fonts->AddFontFromFileTTF("EngineAssets/Rubik-Light.ttf", 14.0f);
		}

		fontCache.headerFont = io.Fonts->AddFontFromFileTTF("EngineAssets/Rubik-Bold.ttf", 15.0f);
		if (fontCache.headerFont == nullptr) {
			LOG(LogType::LOG_WARNING, "Failed to load header font, using main font for headers");
		}

		fontCache.smallFont = io.Fonts->AddFontFromFileTTF("EngineAssets/Rubik-Light.ttf", 10.0f);
		if (fontCache.smallFont == nullptr) {
			LOG(LogType::LOG_WARNING, "Failed to load small font, using main font for small text");
		}

		if (fontCache.mainFont != nullptr) {
			io.FontDefault = fontCache.mainFont;
		}

		io.Fonts->Build();
		fontCache.initialized = true;
	}

	if (!&io) {
		LOG(LogType::LOG_ERROR, "-ImGui IO not created");
	}

	// Initialize style cache
	styleCache.Initialize();

	Application->gui->UIconsolePanel->SetState(true);
	Application->gui->UIProjectPanel->SetState(true);
	Application->gui->UIsettingsPanel->SetState(true);
	Application->gui->UIinspectorPanel->SetState(true);
	Application->gui->UIMainMenuBarPanel->SetState(true);
	Application->gui->UISceneWindowPanel->SetState(true);
	Application->gui->UIGameViewPanel->SetState(true);
	UIGameViewPanel->Init();
	UISceneWindowPanel->Init();

	return true;
}


bool MyGUI::PreUpdate()
{
	return true;
}

bool MyGUI::Update(double dt)
{
	// Check if UI state has changed
	if (uiState.HasChanged(this)) {
		uiState.UpdateFrom(this);
	}

	return true;
}

bool MyGUI::PostUpdate()
{
	return true;
}

bool MyGUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (auto& element : elements)
	{
		if (element)
		{
			delete element;
			element = nullptr;
		}
	}
	elements.clear();

	return true;
}

// Template function for drawing UI elements conditionally
template<typename T>
void DrawElementIfVisible(T* element, bool isVisible) {
	if (isVisible && element) {
		element->Draw();
	}
}

void MyGUI::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Configure dockspace once
	static ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_UnsavedDocument |
		ImGuiDockNodeFlags_AutoHideTabBar |
		ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr, dockspaceFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();

	// Draw UI elements conditionally using template function
	DrawElementIfVisible(UIHierarchyPanel, showHierarchy);
	DrawElementIfVisible(UIconsolePanel, showConsole);
	DrawElementIfVisible(UIsettingsPanel, showSettings);
	DrawElementIfVisible(UIMainMenuBarPanel, showMainMenuBar);
	DrawElementIfVisible(UIinspectorPanel, showInspector);
	DrawElementIfVisible(UISceneWindowPanel, showSceneWindow);
	DrawElementIfVisible(UIProjectPanel, showProject);
	DrawElementIfVisible(UITextEditorPanel, showTextEditor);
	DrawElementIfVisible(UIGameViewPanel, showGameView);

	RenderDebugPanel::GetInstance().Render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Reset layout change flag
	uiState.layoutChanged = false;
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MyGUI::SetColorScheme()
{
	// Apply the cached style if available
	if (styleCache.initialized) {
		styleCache.ApplyStyle();
		return;
	}

	// Otherwise, set up the style for the first time
	ImGuiStyle& style = ImGui::GetStyle();

	// Example style customizations
	style.WindowRounding = 5.0f;
	style.FrameRounding = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.FramePadding = ImVec2(10, 5);
	style.ItemSpacing = ImVec2(5, 5);
	style.ScrollbarSize = 15.0f;

	// Set colors
	ImVec4* colors = ImGui::GetStyle().Colors;

	// Text
	colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Backgrounds
	colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

	// Headers
	colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

	// Borders and separators
	colors[ImGuiCol_Border] = ImVec4(0.05f, 0.05f, 0.05f, 0.7f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	// Frame background (used for inputs, sliders, etc.)
	colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	// Titles
	colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Scrollbars
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.5f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

	// Checkmark
	colors[ImGuiCol_CheckMark] = ImVec4(0.1f, 0.9f, 0.4f, 1.0f);

	// Separators
	colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);

	// Resize grip (used for resizable windows)
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.6f, 0.6f, 0.7f, 1.0f);

	// Plot lines and histogram
	colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.6f, 0.1f, 1.0f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.61f, 0.76f, 0.51f, 0.75f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);

	// Modal window darkening
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);

	// Table
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.15f, 0.15f, 0.2f, 1.0f);

	// Set the main docking background color
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);

	// Change the color of the docking preview area
	colors[ImGuiCol_DockingPreview] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);

	// Save to cache
	styleCache.Initialize();
}