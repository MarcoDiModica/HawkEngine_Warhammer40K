#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform_Component : public Component
{
public:
    Transform_Component(std::weak_ptr<GameObject> owner);
    ~Transform_Component() override = default;

    void Start() override {}
    void Update(float deltaTime) override {}
    void Destroy() override {}

    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& newPosition);
    void Translate(const glm::vec3& translation);

    glm::quat GetRotation() const { return rotation; }
    void SetRotation(const glm::quat& newRotation);
    void Rotate(const glm::vec3& axis, float angle);

    glm::vec3 GetScale() const { return scale; }
    void SetScale(const glm::vec3& newScale);

    void SetParent(std::weak_ptr<Transform_Component> newParent);
    std::shared_ptr<Transform_Component> GetParent() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
    void LookAt(const glm::vec3& target);

    // Matrix
    glm::mat4 GetModelMatrix() const;

private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(glm::vec3(0.0f));
    glm::vec3 scale = glm::vec3(1.0f);

    std::weak_ptr<Transform_Component> parent;

    mutable glm::mat4 cachedModelMatrix = glm::mat4(1.0f);
    mutable bool isMatrixDirty = true;
};