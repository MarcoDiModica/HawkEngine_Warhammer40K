#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#pragma once

#include "Module.h"
#include "../MyGameEngine/CameraBase.h"
#include "../MyGameEngine/Component.h"

class EditorCamera : public Module, public CameraBase 
{
public:
	EditorCamera(App* app);
	~EditorCamera() override = default;

	bool Awake() override;
	bool Start() override;
	bool FixedUpdate() override;
	bool Update(double dt) override;
	bool PostUpdate() override;
	bool CleanUp() override;

	Transform_Component& GetTransform() { return transform; }

	glm::dmat4 view() const { return GetViewMatrix(transform); }
	glm::dmat4 projection() const { return GetProjectionMatrix(); }

	float GetMovementSpeed() const { return cameraSpeed; }
	void SetMovementSpeed(float speed) { cameraSpeed = speed; }

	float GetMouseSensitivity() const { return static_cast<float>(sensitivity); }
	void SetMouseSensitivity(float sensitivity) { this->sensitivity = sensitivity; }

	void UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight);

private:
	glm::dvec2 lastMousePos = glm::dvec2(0.0, 0.0);
	Transform_Component transform;
	GameObject* listenerObject = nullptr;

	void move_camera(float speed, float deltaTime);
	bool infiniteMouse = false;
	float cameraSpeed = 20.0f;
	float zoomSpeed = 5.0f;
	double sensitivity = 20.0f;
	double yaw = 0.0;
	double pitch = 0.0;
	double orbitYaw = 0.0;
	double orbitPitch = 0.0;
	double MAX_PITCH = 89.0;
	glm::dvec3 orbitPoint = glm::dvec3(0.0, 0.0, 0.0);

	bool isFocusing = false;
	bool mouseHidden = false;
	float focusTime = 0.0f;
	float focusDuration = 0.5f;
	glm::dvec3 targetPosition = glm::dvec3(0.0, 0.0, 0.0);
	glm::dquat targetRotation = glm::dquat(1.0, 0.0, 0.0, 0.0);
	
};

#endif