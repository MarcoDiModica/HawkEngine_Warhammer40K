#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#pragma once

#include "Module.h"
#include "../MyGameEngine/CameraBase.h"

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

	float GetMouseSensitivity() const { return sensitivity; }
	void SetMouseSensitivity(float sensitivity) { this->sensitivity = sensitivity; }

private:
	glm::dvec2 lastMousePos = glm::dvec2(0.0, 0.0);
	Transform_Component transform;

	void move_camera(float speed, float deltaTime);
	void UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight);

	float cameraSpeed = 5.0f;
	float zoomSpeed = 5.0f;
	double sensitivity = 5.0f;
	double yaw = 0.0;
	double pitch = 0.0;
	double MAX_PITCH = glm::radians(89.0);
};

#endif