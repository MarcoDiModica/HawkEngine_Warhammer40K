#pragma once

#include "Transform.h"

class Camera {

public:
	double fov = glm::radians(60.0);
	double aspect = 16.0 / 9.0;
	double zNear = 0.1;
	double zFar = 128.0;

private:
	Transform _transform;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }

	mat4 projection() const;
	mat4 view() const;

};