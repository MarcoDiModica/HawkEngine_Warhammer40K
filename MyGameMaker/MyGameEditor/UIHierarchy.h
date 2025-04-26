#ifndef __UI_HIERARCHY_H__
#define __UI_HIERARCHY_H__
#pragma once

#include <vector>
#include <string>
#include <memory>

#include "UIElement.h"
#include "Root.h"

class GameObject;

class UIHierarchy : public UIElement
{
public:
	UIHierarchy(UIType type, std::string name);
	~UIHierarchy();

	void RenderSceneHierarchy(Scene* scene);
	bool DrawSceneObject(GameObject& obj);

	bool startingDrag = false;

	bool Draw();
};
#endif // !__UI_HIERARCHY_H__