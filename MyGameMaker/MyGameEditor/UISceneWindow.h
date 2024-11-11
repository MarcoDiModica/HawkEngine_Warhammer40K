#ifndef __UI_SCENE_WINDOW_H__
#define __UI_SCENE_WINDOW_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

class UISceneWindow : public UIElement
{
public:
	UISceneWindow(UIType type, std::string name);
	~UISceneWindow();
	void Init();
	bool Draw();
	
};
#endif // !__UI_SCENE_WINDOW_H__