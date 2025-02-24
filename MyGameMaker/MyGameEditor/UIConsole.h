#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__
#pragma once

#include <string>

#include "UIElement.h"

class UIConsole : public UIElement
{
public:
	UIConsole(UIType type, std::string name);
	~UIConsole();

	bool Draw();
};
#endif // !__UI_CONSOLE_H__s