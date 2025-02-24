#ifndef __UI_CAMERA_H__
#define __UI_CAMERA_H__
#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

#include "MyGameEngine/types.h"
#include "UIElement.h"


class UICamera : public UIElement
{
public:
	UICamera(UIType type, std::string name);
	~UICamera();

	void Init();
	void UpdateFramebuffer();

	bool Draw();

private:
	vec2 winSize = vec2(0, 0);
	vec2 winPos = vec2(0, 0);
};
#endif // !__UI_CAMERA_H__