#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__
#pragma once

#include <string>

enum class UIType
{
	SETTINGS,
	DEFAULT,
	CONSOLE,
	INSPECTOR,
	HIERARCHY,
	PROJECT,
	CAMERA,
	TEXTEDITOR,
	UNKNOWN
};

class UIElement
{
public:

	UIElement(UIType, std::string name) : type(type), name(name), enabled(false) {}
	virtual ~UIElement() {}

	virtual bool Draw() = 0;

	UIType GetType() const { return type; }
	const std::string& GetName() const { return name; }

	bool GetState() { return enabled; }
	void SetState(bool state) { enabled = state; }
	bool firstDraw = true;

protected:

	UIType type = UIType::UNKNOWN;
	std::string name;
	bool enabled;
};

#endif // !__UIELEMENT_H__