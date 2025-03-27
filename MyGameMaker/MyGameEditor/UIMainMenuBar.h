#ifndef __UI_MAIN_MENU_BAR_H__
#define __UI_MAIN_MENU_BAR_H__
#pragma once

#include <string>

#include "UIElement.h"
#include "MyGameEngine/Image.h"

class Image;

class UIMainMenuBar : public UIElement
{
private:
	Image play_image;
	Image stop_image;
	Image pause_image;


	bool pressing_play = false;
	bool isPaused = false;

public:
	UIMainMenuBar(UIType type, std::string name);
	~UIMainMenuBar();

	bool Draw();

	float fps;
	int newSceneCount = 1;
};
#endif // !__UI_MAIN_MENU_BAR_H__