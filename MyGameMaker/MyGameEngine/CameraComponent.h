#pragma once

#include "Component.h"
#include "GameObject.h"
#include "CameraBase.h"
#include "TransformComponent.h"

class GameObject;
class SceneSerializer;

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

    void Start() override;
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

    void SetDistance(float distance) { followDistance = distance; }
    float GetDistance() const { return followDistance; }

    void SetPriority(float newPriority);
	int GetPriority() const { return priority; }
    void SetAsHighestPriority();

    void SetOffset(const glm::vec3& offset) { followOffset = offset; }
    glm::vec3 GetOffset() const { return followOffset; }

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
	int priority;

    bool ShakeEnabled = false;
    bool frustrumCullingEnabled = true;
    bool frustrumRepresentation = true;


    ProjectionType projectionType = ProjectionType::Perspective;

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:

    void UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight);

protected:

    friend class SceneSerializer;

    YAML::Node encode() override
    {
		YAML::Node node = Component::encode();

		node["projection_type"] = projectionType == ProjectionType::Perspective ? "perspective" : "orthographic";
		node["fov"] = fov;
		node["size"] = orthoSize;
		node["near_plane"] = zNear;
		node["far_plane"] = zFar;
		node["priority"] = priority;

		return node;
    }

    bool decode(const YAML::Node& node) override
	{
        Component::decode(node);

		if (!node["projection_type"] || !node["fov"] || !node["near_plane"] || !node["far_plane"])
			return false;

		std::string projection = node["projection_type"].as<std::string>();
		if (projection == "perspective")
			projectionType = ProjectionType::Perspective;
		else if (projection == "orthographic")
			projectionType = ProjectionType::Orthographic;

		fov = node["fov"].as<float>();
		orthoSize = node["size"].as<float>();
		zNear = node["near_plane"].as<double>();
		zFar = node["far_plane"].as<double>();
		int newPriority = node["priority"].as<int>();
		SetPriority(newPriority);

		return true;
	}
};