#include "TransformComponent.h"
#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

Transform_Component::Transform_Component(std::weak_ptr<GameObject> owner) : Component(owner) { name = "Transform_Component"; }

Transform_Component::Transform_Component(const Transform_Component& other) : Component(other)
{
    matrix = other.matrix;
    position = other.position;
    left = other.left;
    up = other.up;
    forward = other.forward;
}

Transform_Component& Transform_Component::operator=(const Transform_Component& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        matrix = other.matrix;
        position = other.position;
        left = other.left;
        up = other.up;
        forward = other.forward;
    }
    return *this;
}

std::shared_ptr<Component> Transform_Component::Clone()
{
    auto clone = std::make_shared<Transform_Component>(*this);
    clone->matrix = this->matrix;
    clone->left = this->left;
    clone->up = this->up;
    clone->forward = this->forward;
    clone->position = this->position;
    return clone;
}

void Transform_Component::Translate(const glm::dvec3& translation)
{
	matrix = glm::translate(matrix, translation);
}
void Transform_Component::SetPosition(const glm::dvec3& position)
{
	matrix[3] = glm::dvec4(position, 1);
}

void Transform_Component::Rotate(double rads, const glm::dvec3& axis)
{
	matrix = glm::rotate(matrix, rads, axis);
}

void Transform_Component::SetRotation(const glm::dvec3& eulerAngles)
{
    glm::dmat4 rotationMatrix = glm::identity<glm::dmat4>();
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.x, glm::dvec3(1, 0, 0));
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.y, glm::dvec3(0, 1, 0));
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.z, glm::dvec3(0, 0, 1));

    // Preserve the position
    glm::dvec3 position = glm::dvec3(matrix[3]);
    matrix = rotationMatrix;
    matrix[3] = glm::dvec4(position, 1);
}


void Transform_Component::SetScale(const glm::dvec3& scale)
{
    glm::dmat4 scaleMatrix = glm::identity<glm::dmat4>();
    scaleMatrix = glm::scale(scaleMatrix, scale);

    // Preserve the position
    glm::dvec3 position = glm::dvec3(matrix[3]);
    matrix = scaleMatrix;
    matrix[3] = glm::dvec4(position, 1);
}

void Transform_Component::Scale(const glm::dvec3& scale)
{
	matrix = glm::scale(matrix, scale);
}

void Transform_Component::LookAt(const glm::dvec3& target)
{
	matrix = glm::lookAt(position, target, up);
}