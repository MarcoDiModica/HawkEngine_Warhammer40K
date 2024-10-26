#include "Module.h"
#include "MyWindow.h"
#include "App.h"
#include <list>

#pragma once

class Panel;
class PanelHierarchy;


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

	void processEvent(const SDL_Event& event) override;

};