#ifndef __UI_HIERARCHY_H__
#define __UI_HIERARCHY_H__
#pragma once

#include "UIElement.h"
//#include "MyGameEngine/Scene.h"
#include "Root.h"
#include <vector>
#include <string>
#include <memory>

class GameObject;

class UIHierarchy : public UIElement
{
public:
	UIHierarchy(UIType type, std::string name);
	~UIHierarchy();

	void RenderSceneHierarchy(Scene* scene);
	bool DrawSceneObject(GameObject& obj);

	bool Draw();

	GameObject* draggedObject;
};
#endif // !__UI_HIERARCHY_H__