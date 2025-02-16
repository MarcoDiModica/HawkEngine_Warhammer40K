#pragma once

#include <iostream>
#include "GameObject.h"


#define MAX_MOUSE_BUTTONS 5
#define MAX_CONTROLLER_BUTTONS 15
#define MAX_CONTROLLERS 4

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class InputEngine
{
public:
	InputEngine();

	virtual ~InputEngine();

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

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}


	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	int GetAxis(const char* axisName) const;

	GameObject* GetDraggedGameObject() const
	{
		return draggedObject;
	}

	void SetDraggedGameObject(GameObject* gameObject)
	{
		draggedObject = gameObject;
	}

	void AddToSelection(GameObject* gameObject);
	void RemoveFromSelection(GameObject* gameObject);
	std::vector<GameObject*> GetSelectedGameObjects() const;
	void ClearSelection();
	bool IsGameObjectSelected(GameObject* gameObject) const;

	void HandleFileDrop(const std::string& fileDir);

	void InitControllers();
	void CleanUpControllers();
	void UpdateControllers();
	void HandleDeviceConnection(int index);
	void HandleDeviceRemoval(int index);


public:
	KEY_STATE* keyboard;
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
	GameObject* draggedObject;
	//GamePad gamepads[MAX_CONTROLLERS];

};

extern InputEngine* InputManagement;

