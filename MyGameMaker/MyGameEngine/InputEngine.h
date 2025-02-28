#pragma once

#include <iostream>
#include "GameObject.h"
#include "SDL2/SDl_GameController.h"
#include <vector>
#include "SDL2/SDL_haptic.h"
#include "SDL2/SDL_events.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_CONTROLLER_BUTTONS 21
#define MAX_CONTROLLERS 4

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct GamePad {
	SDL_GameController* controller = nullptr;
	SDL_Haptic* haptic = nullptr;
	bool enabled = false;
	int index = -1;
	float l_x, l_y, r_x, r_y, l2, r2;
	bool a, b, x, y, l1, r1, l3, r3, up, down, left, right, start, guide, back;
	float l_dz = 0.1f, r_dz = 0.1f;
	int rumble_countdown = 0;
	float rumble_strength = 0.0f;
	int buttons[SDL_CONTROLLER_BUTTON_MAX];
	int axes[SDL_CONTROLLER_AXIS_MAX];
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
	void CheckSDLEvent(SDL_Event tempEvent);


public:
	KEY_STATE* keyboard;
	KEY_STATE* controller_buttons;
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
	GamePad gamepads[MAX_CONTROLLERS];
	SDL_Event event;

};

extern InputEngine* InputManagement;

