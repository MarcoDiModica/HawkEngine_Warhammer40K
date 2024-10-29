#ifndef __UI_MAIN_MENU_BAR_H__
#define __UI_MAIN_MENU_BAR_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

class UIMainMenuBar : public UIElement
{
public:
	UIMainMenuBar(UIType type, std::string name);
	~UIMainMenuBar();

	bool Draw();
};
#endif // !__UI_MAIN_MENU_BAR_H__
