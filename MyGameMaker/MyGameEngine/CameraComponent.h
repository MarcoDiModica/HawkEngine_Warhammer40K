#pragma once

#include "Component.h"
#include "GameObject.h"
#include "CameraBase.h"
#include "TransformComponent.h"

class GameObject;

enum class ProjectionType { Perspective, Orthographic };

class CameraComponent : public Component, public CameraBase
{
public:
    CameraComponent(GameObject* owner);
    CameraComponent(const CameraComponent& other);
    CameraComponent(CameraComponent&& other) noexcept;
    CameraComponent& operator=(const CameraComponent& other);
    CameraComponent& operator=(CameraComponent&& other) noexcept;
    ~CameraComponent() override = default;

    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void Start() override {}
    void Update(float deltaTime) override;
    void Destroy() override {}

    ComponentType GetType() const override { return ComponentType::CAMERA; }

    glm::dmat4 view() const { return GetViewMatrix(*owner->GetTransform()); }
    glm::dmat4 projection() const { return GetProjectionMatrix(); }

    void SetPerspective(float fov, float nearPlane, float farPlane);
    void SetOrthographic(float size, float nearPlane, float farPlane);

    void FollowTarget(GameObject* target, float distance, const glm::vec3& offset, float smoothness, bool followX = true, bool followY = true, bool followZ = true);
    void LookAtTarget(GameObject* target, const glm::vec3& offset, float smoothness);

    void Shake(float intensity, float duration, float frequency);

private:
    void UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight);

    GameObject* followTarget = nullptr;
    double followDistance = 5.0f;
    glm::dvec3 followOffset = glm::dvec3(0.0f);
    double followSmoothness = 0.1f;
    bool followX = true;
    bool followY = true;
    bool followZ = true;

    GameObject* lookAtTarget = nullptr;
    glm::dvec3 lookAtOffset = glm::dvec3(0.0f);
    float lookAtSmoothness = 0.1f;

    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
    float shakeFrequency = 0.0f;
    float shakeTimer = 0.0f;

    bool ShakeEnabled = false;
};