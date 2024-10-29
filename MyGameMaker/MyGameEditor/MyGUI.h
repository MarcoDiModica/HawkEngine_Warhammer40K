#include "Module.h"
#include "MyWindow.h"
#include "App.h"
#include <list>

#pragma once

class UIElement;
class UIConsole;
class UISettings;
class UIMainMenuBar;
// class PanelHierarchy;

class MyGUI : public IEventProcessor , public Module
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

public:

	// UI Elements
	UIConsole* UIconsolePanel = nullptr;
	UISettings* UIsettingsPanel = nullptr;
	UIMainMenuBar* UIMainMenuBarPanel = nullptr;

private:
	std::list<UIElement*> elements;

	bool showGUI = true;

};