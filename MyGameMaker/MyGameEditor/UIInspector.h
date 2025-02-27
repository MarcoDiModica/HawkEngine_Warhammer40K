#ifndef __UI_INSPECTOR_H__
#define __UI_INSPECTOR_H__
#pragma once

#include "UIElement.h"

class GameObject;

class UIInspector : public UIElement
{
public:
	UIInspector(UIType type, std::string name);
	~UIInspector();

	bool Draw();
	void DrawGameObjectHeader(GameObject* gameObject);
	bool snap = false;
	float snapValue = 1;
	bool once = false;

private:
	bool matrixDirty;

	glm::vec3 position;
	glm::vec3 rotationDegrees = glm::vec3(0,0,0);
	glm::vec3 rotationRadians;
	glm::vec3 scale;
};

#endif // !__UI_INSPECTOR_H__