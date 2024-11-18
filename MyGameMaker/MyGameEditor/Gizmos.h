#ifndef GIZMOS_H
#define GIZMOS_H
#pragma once
#include "Module.h"
#include "Transform.h"


class Gizmos : public Module {

public:

	Gizmos(App* app);

	virtual ~Gizmos() = default;

	bool FixedUpdate();

	bool Start();

	bool Awake();

	bool Update(double dt);

	void DrawGizmos();

	void ProcessMousePicking();
	
	bool IntersectAxis(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& outAxis);

	void MoveObjectAlongAxis(const glm::vec3& rayOrigin, glm::vec3& rayDirection, const glm::vec3& axis);

private:

	glm::vec3 selectedAxis = glm::vec3(0,0,0);
	bool isAxisSelected = false;
	glm::vec3 rayStartPos;
	glm::vec3 rayDir;

};

#endif