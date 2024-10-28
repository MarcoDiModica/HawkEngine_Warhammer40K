#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__
#pragma once

#include <string>

enum class UIType
{
	SETTINGS,
	DEFAULT,
	CONSOLE,
	UNKNOWN	
};

class UIElement
{
public:

	UIElement(UIType, std::string name);
	virtual ~UIElement();

	virtual bool Draw() = 0;

	virtual UIType GetType() const;
	virtual std::string GetName() const;

	virtual bool GetState();
	virtual void SetState(bool state);
	virtual void SwitchState();

protected:

	UIType type = UIType::UNKNOWN;
	std::string name;
	bool enabled = false;

};

#endif // !__UIELEMENT_H__


