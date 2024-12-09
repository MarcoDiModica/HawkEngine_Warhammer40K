#pragma once

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include "Module.h"


class IEventProcessor {
public:
	virtual void processEvent(const SDL_Event& event) = 0;
};

class Window : public Module {

	SDL_Window* _window = nullptr;
	void* _ctx = nullptr;

	unsigned short _width = 1280;
	unsigned short _height = 720;

	bool isFullscreen = false;

public:

	// TODO fully implement theese methods
	bool Awake();
	bool Start() { return true; }
	bool CleanUp() { return true; }

	//bool initSDLWindowWithOpenGL();
	//bool createSdlGlContext();
	//bool initOpenGL();

	int width() const { return _width; }

	int height() const { return _height; }

	double aspect() const { return static_cast<double>(_width) / _height; }

	auto* windowPtr() const { return _window; }

	auto* contextPtr() const { return _ctx; }

	Window(const char* title, unsigned short width, unsigned short height);

	Window(Window&&) noexcept = delete;

	Window(const Window&) = delete;

	Window& operator=(const Window&) = delete;

	~Window();

	void Open(const char* title, unsigned short width, unsigned short height);

	void Close();

	bool IsOpen() const { return _window; }

	bool ProcessEvents(IEventProcessor* event_processor = nullptr);

	void SwapBuffers() const;

	void ToggleFullscreen();

	unsigned int GetDisplayRefreshRate();
	
private:
	unsigned int refreshRate;

};