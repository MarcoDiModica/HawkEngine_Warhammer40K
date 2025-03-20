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

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    _width = width;
    _height = height;

    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    SDL_GL_SetSwapInterval(-1); //vsync
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