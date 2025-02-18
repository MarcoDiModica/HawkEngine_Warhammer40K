#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"

#include "Log.h"

using namespace std;

Window::Window(const char* title, unsigned short width, unsigned short height) : refreshRate(0) {
    Open(title, width, height);
}

Window::~Window() {
    Close();
}

bool Window::Awake()
{
    bool ret = true;

    /*SetResolution(resolution);*/

  /*  LOG(LogType::LOG_INFO, "# Initializing SDL Window with OpenGL...");
    if (!initSDLWindowWithOpenGL())
        return false;

    LOG(LogType::LOG_INFO, "# Initializing OpenGL...");
    if (!createSdlGlContext())
        return false;

    if (!initOpenGL())
        return false;*/

    return true;
}

void Window::Open(const char* title, unsigned short width, unsigned short height) {
    if (IsOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw exception(SDL_GetError());
}

void Window::Close() {
    if (!IsOpen()) return;

    SDL_GL_DeleteContext(_ctx);
    _ctx = nullptr;

    SDL_DestroyWindow(static_cast<SDL_Window*>(_window));
    _window = nullptr;
}

void Window::SwapBuffers() const {
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

bool Window::ProcessEvents(IEventProcessor* event_processor) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (event_processor) event_processor->processEvent(e);
        switch (e.type) {
        case SDL_DROPFILE:

            printf("A file was dropped");
            return true;

        case SDL_QUIT: 
            printf("Closing application");
            Close(); 
            return false;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                printf("Window resized to %d %d\n", e.window.data1, e.window.data2);

                glViewport(0, 0, e.window.data1, e.window.data2);
			}
			break;
		}
    }
    return true;
}

unsigned int Window::GetDisplayRefreshRate()
{
    SDL_DisplayMode desktopDisplay;

    if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
        refreshRate = desktopDisplay.refresh_rate;
    else
        LOG(LogType::LOG_ERROR, "Getting display refresh rate: %s", SDL_GetError());

    return refreshRate;
}

void Window::ToggleFullscreen() 
{
	if (!_window) return;

	isFullscreen = !isFullscreen;

	if (isFullscreen) {
		SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
        SDL_SetWindowFullscreen(_window, 0);
        SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}