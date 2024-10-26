#include "TransformComponent.h"
#include "GameObject.h"

Transform_Component::Transform_Component(std::weak_ptr<GameObject> owner) : Component(owner) {}

void Transform_Component::SetPosition(const glm::vec3& newPosition) 
{
    position = newPosition;
    isMatrixDirty = true;
}

void Transform_Component::Translate(const glm::vec3& translation) 
{
    position += translation;
    isMatrixDirty = true;
}

void Transform_Component::SetRotation(const glm::quat& newRotation) 
{
    rotation = newRotation;
    isMatrixDirty = true;
}

void Transform_Component::Rotate(const glm::vec3& axis, float angle) 
{
    rotation = glm::rotate(rotation, glm::radians(angle), axis);
    isMatrixDirty = true;
}

void Transform_Component::SetScale(const glm::vec3& newScale) 
{
    scale = newScale;
    isMatrixDirty = true;
}

void Transform_Component::SetParent(std::weak_ptr<Transform_Component> newParent) 
{
    parent = newParent;
    isMatrixDirty = true;
}

std::shared_ptr<Transform_Component> Transform_Component::GetParent() const 
{
    return parent.lock();
}

glm::vec3 Transform_Component::GetForward() const 
{
    return rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Transform_Component::GetRight() const 
{
    return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform_Component::GetUp() const 
{
    return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Transform_Component::LookAt(const glm::vec3& target) 
{
    glm::vec3 direction = glm::normalize(target - position);
    rotation = glm::quatLookAt(direction, GetUp());
    isMatrixDirty = true;
}

glm::dmat4 Transform_Component::GetModelMatrix() const 
{
    if (isMatrixDirty) {
        glm::mat4 localModel = glm::dmat4(1.0f);
        localModel = glm::translate(localModel, position);
        localModel = glm::mat4_cast(rotation) * localModel;
        localModel = glm::scale(localModel, scale);

        if (auto parentTransform = parent.lock()) {
            cachedModelMatrix = parentTransform->GetModelMatrix() * (glm::dmat4)localModel;
        }
        else {
            cachedModelMatrix = localModel;
        }

        isMatrixDirty = false;

        PropagateDirtyFlag();
    }
    return cachedModelMatrix;
}

void Transform_Component::PropagateDirtyFlag() const 
{
    for (auto child : GetOwner()->GetChildren()) {
        if (child->HasComponent<Transform_Component>()) {
            auto childTransform = child->GetComponent<Transform_Component>();
            childTransform->isMatrixDirty = true;
            childTransform->PropagateDirtyFlag();
        }
    }
}