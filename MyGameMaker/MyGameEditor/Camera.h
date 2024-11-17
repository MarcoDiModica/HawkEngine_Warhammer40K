#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "Module.h"
#include "Transform.h"


class Camera : public Module {

public:


	Camera (App* app);


	virtual ~Camera() = default;

	double fov = glm::radians(60.0);
	double aspect = 16.0 / 9.0;
	double zNear = 0.1;
	double zFar = 128.0;

	bool FixedUpdate();

	bool Start();

	bool Awake();

	bool Update(double dt);

	void move_camera();

	void UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight);

	float orbitRadius = 5.0f;
	float orbitSpeed = 0.01f;
	float angle = 0.0f;
	float angleHorizontal = 0.0f;
	float angleVertical = 0.0f;



private:
	Transform _transform;
	glm::vec3 front;
	glm::dmat4 viewMatrix;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }
	void setFront(const glm::vec3& front);

	mat4 projection() const;
	mat4 view() const;

};

#endif