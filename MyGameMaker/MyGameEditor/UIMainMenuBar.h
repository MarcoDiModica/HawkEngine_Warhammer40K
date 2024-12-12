#ifndef __UI_MAIN_MENU_BAR_H__
#define __UI_MAIN_MENU_BAR_H__
#pragma once

#include "UIElement.h"
#include "MyGameEngine/Image.h"
#include <vector>
#include <string>

class Image;

class UIMainMenuBar : public UIElement
{
private:
	Image play_image;
	Image stop_image;


	bool pressing_play = false;

public:
	UIMainMenuBar(UIType type, std::string name);
	~UIMainMenuBar();

	bool Draw();

	int newSceneCount = 1;
};
#endif // !__UI_MAIN_MENU_BAR_H__