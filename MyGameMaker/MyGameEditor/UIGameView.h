#ifndef __UI_GAME_VIEW_H__
#define __UI_GAME_VIEW_H__
#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

#include "MyGameEngine/types.h"
#include "UIElement.h"

class UIGameView : public UIElement
{
public:
    UIGameView(UIType type, std::string name);
    ~UIGameView();

    void Init();
    void UpdateFramebuffer();
    void BeginRender();
    void EndRender();
    bool Draw();

	vec2 GetWinSize() { return winSize; }
	vec2 GetWinPos() { return winPos; }

    vec2 GetViewportSize();
    vec2 GetViewportPos();

	float GetWidth() { return width; }
	float GetHeight() { return height; }

private:
    vec2 winSize = vec2(0, 0);
    vec2 winPos = vec2(0, 0);

    float width;
    float height;    
    glm::dvec2 viewportPos;
    glm::dvec2 viewportSize;
    
	GLuint msaaFbo = 0;
	GLuint msaaColorRbo = 0;
	GLuint msaaDepthRbo = 0;
	int msaaSamples = 0;
	bool useMSAA = false;
};
#endif // !__UI_GAME_VIEW_H__