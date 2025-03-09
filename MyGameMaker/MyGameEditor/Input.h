#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once

#include <iostream>

#include "Module.h"
#include "../MyGameEngine/InputEngine.h"

#define MAX_MOUSE_BUTTONS 5

class Input : public Module
{
public:
	Input(App* Application);

	virtual ~Input();

	bool Awake();

	bool PreUpdate();

	bool Update(double dt);

	bool processSDLEvents();

	
	//Direction of MousePick Ray
	glm::vec3 GetMousePickDir(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y);

	//Origin of MousePick Ray
	glm::vec3 ConvertMouseToWorldCoords(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y);


	glm::vec3 getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize);

	glm::vec3 getMousePickRay();

	KEY_STATE GetControllerKey(int id);

	KEY_STATE GetKey(int id);

	KEY_STATE GetMouseButton(int id);

	int GetMouseX();

	int GetMouseY();

	int GetMouseZ();

	int GetMouseXMotion();

	int GetMouseYMotion();

	int GetAxis(const char* axisName);

	int GetJoystickAxis(int gamepadIndex, SDL_GameControllerAxis axis);

	GameObject* GetDraggedGameObject();

	void SetDraggedGameObject(GameObject* gameObject);

	void AddToSelection(GameObject* gameObject);
	void RemoveFromSelection(GameObject* gameObject);
	std::vector<GameObject*> GetSelectedGameObjects();
	void ClearSelection();
	bool IsGameObjectSelected(GameObject* gameObject);

	void HandleFileDrop(const std::string& fileDir);

private:
	/*KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	float dx;
	float dy;
	std::vector<GameObject*> selectedObjects;
	std::vector<GameObject*> copiedObjects;
	GameObject* draggedObject;*/
	EditorCamera* camera;
};

#endif // !__INPUT_H__


