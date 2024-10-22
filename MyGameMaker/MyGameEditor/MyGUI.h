#include "Module.h"
#include "MyWindow.h"
#include "App.h"


#pragma once




class MyGUI : public IEventProcessor , public Module
{
public:

	//MyGUI(App* Application);
	MyGUI(SDL_Window* window, void* context);
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