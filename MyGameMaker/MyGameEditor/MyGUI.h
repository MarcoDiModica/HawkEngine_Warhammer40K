#include "Module.h"
#include "MyWindow.h"
#include "App.h"
#include <list>

#pragma once

class UIElement;
class UIConsole;
class UISettings;
class UIProject;
class UIInspector;
class UIMainMenuBar;
class UIHierarchy;
class UISceneWindow;
class UICamera;
class UIAudioTest;

class MyGUI : public IEventProcessor, public Module
{
public:

	//MyGUI(App* Application);
	MyGUI(App* app);
	MyGUI(MyGUI&&) noexcept = delete;
	MyGUI(const MyGUI&) = delete;
	MyGUI& operator=(const MyGUI&) = delete;
	~MyGUI();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();
	bool CleanUp();

	void Render();
	bool isInitialized(UIElement* element);

	void processEvent(const SDL_Event& event) override;
	void SetColorScheme();

public:

	// UI Elements
	UIHierarchy* UIHierarchyPanel = nullptr;
	UIConsole* UIconsolePanel = nullptr;
	UISettings* UIsettingsPanel = nullptr;
	UIInspector* UIinspectorPanel = nullptr;
	UIMainMenuBar* UIMainMenuBarPanel = nullptr;
	UISceneWindow* UISceneWindowPanel = nullptr;
	UIProject* UIProjectPanel = nullptr;
	UICamera* UICameraPanel = nullptr;
	UIAudioTest* UIAudioTestPanel = nullptr;

	bool showHierarchy = true;
	bool showInspector = true;
	bool showConsole = true;
	bool showSettings = false;
	bool showMainMenuBar = true;
	bool showSceneWindow = true;
	bool showProject = true;
	bool showCamera = true;
	bool showAudioTest = false;

	GLuint fbo = 0;
	vec2 camSize = vec2(1280, 720);
	GLuint fboTexture = 0;
	GLuint rbo = 0;

	GLuint fboCamera = 0;
	GLuint fboTextureCamera = 0;
	GLuint rboCamera = 0;

private:
	std::list<UIElement*> elements;

	bool showGUI = true;

};