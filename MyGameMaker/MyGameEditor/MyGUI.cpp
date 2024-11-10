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

#include "UIElement.h"
#include "UIConsole.h"
#include "UISettings.h"
#include "UIMainMenuBar.h"
#include "UIInspector.h"
#include "UIHierarchy.h"


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

	// Generate and bind the FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create the texture to render to
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	// Create a renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

	

	ImGui::Begin("GameWindow");
	{
		ImGui::BeginChild("GameRender");
		// Get the size of the child (i.e. the whole draw size of the windows).
		ImVec2 wsize = ImVec2(Application->window->width(), Application->window->height());
		// Because I use the texture from OpenGL, I need to invert the V from the UV.
		ImGui::Image((ImTextureID)fboTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}