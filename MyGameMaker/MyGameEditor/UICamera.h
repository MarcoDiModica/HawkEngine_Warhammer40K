#ifndef __UI_CAMERA_H__
#define __UI_CAMERA_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

class UICamera : public UIElement
{
public:
	UICamera(UIType type, std::string name);
	~UICamera();
	bool Draw();
};
#endif // !__UI_CAMERA_H__