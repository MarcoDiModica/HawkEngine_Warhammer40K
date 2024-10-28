#include "TransformComponent.h"
#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

Transform_Component::Transform_Component(std::weak_ptr<GameObject> owner) : Component(owner) {}

void Transform_Component::Translate(const glm::dvec3& translation)
{
	matrix = glm::translate(matrix, translation);
}

void Transform_Component::Rotate(double rads, const glm::dvec3& axis)
{
	matrix = glm::rotate(matrix, rads, axis);
}

void Transform_Component::Scale(const glm::dvec3& scale)
{
	matrix = glm::scale(matrix, scale);
}

void Transform_Component::LookAt(const glm::dvec3& target)
{
	matrix = glm::lookAt(position, target, up);
}