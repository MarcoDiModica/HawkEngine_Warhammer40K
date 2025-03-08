#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UITransformComponent : public Component
{
public:
	explicit UITransformComponent(GameObject* owner);
	~UITransformComponent() override = default;

	UITransformComponent(const UITransformComponent&) = delete;
	UITransformComponent& operator=(const UITransformComponent&) = delete;

	UITransformComponent(UITransformComponent&&) noexcept = default;
	UITransformComponent& operator=(UITransformComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::UITRANSFORM; }

	void SetTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	void SetRotation(const glm::dvec3& eulerAngles);
	void SetPosition(const glm::dvec3& position);
	void SetScale(const glm::dvec3& scale);
	void Translate(const glm::dvec3& translation);
	void Rotate(double rads, const glm::dvec3& axis);
	void Scale(const glm::dvec3& scale);

	glm::dvec3 GetEulerAngles() const {
		return glm::degrees(glm::eulerAngles(GetRotation()));
	}

	void SetMatrix(const glm::dmat4& newMatrix) {
		matrix = newMatrix;
	}
	glm::vec3 GetPosition() const { return position; }
	glm::dquat GetRotation() const {
		return glm::quat_cast(matrix);
	}
	glm::vec3 GetScale() const {
		return scale;
	}

private:
	//posicion, rotacion, escala
	
	glm::vec3 rotation;
	glm::vec3 scale;

	union
	{
		//DONT modify directly, use SetMatrix
		glm::dmat4 matrix = glm::dmat4(1.0);
		struct
		{
			glm::dvec3 left; glm::dmat4::value_type left_w;
			glm::dvec3 up; glm::dmat4::value_type up_w;
			glm::dvec3 forward; glm::dmat4::value_type fwd_w;
			glm::dvec3 position; glm::dmat4::value_type pos_w;
		};
	};
};

