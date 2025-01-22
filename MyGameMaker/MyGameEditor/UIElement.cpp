#include "UIElement.h"

UIElement::UIElement(UIType type, std::string name) : type(type), name(name)
{
}

UIElement::~UIElement()
{
}

UIType UIElement::GetType() const
{
	return type;
}

std::string UIElement::GetName() const
{
	return name;
}

bool UIElement::GetState()
{
	return enabled;
}

void UIElement::SetState(bool state)
{
	this->enabled = enabled;
}

void UIElement::SwitchState()
{
	enabled = !enabled;
}

void UIElement::Update(float deltaTime)
{
	
}

bool UIElement::IsEnabled() const
{
	return enabled;
}