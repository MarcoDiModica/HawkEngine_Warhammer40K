#ifndef __UI_SCENE_WINDOW_H__
#define __UI_SCENE_WINDOW_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>
#include "MyGameEngine/types.h" 

class UISceneWindow : public UIElement
{
public:
	UISceneWindow(UIType type, std::string name);
	~UISceneWindow();
	void Init();
	bool Draw();
	vec2 winSize = vec2(0, 0);
	vec2 winPos = vec2(0, 0);
	vec2 winMousePos = vec2(0, 0);
};
#endif // !__UI_SCENE_WINDOW_H__