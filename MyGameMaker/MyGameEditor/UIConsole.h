#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

class UIConsole : public UIElement
{
public:
	UIConsole(UIType type, std::string name);
	~UIConsole();

	bool Draw();
};
#endif // !__UI_CONSOLE_H__



