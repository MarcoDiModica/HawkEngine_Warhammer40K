#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"
#ifdef YAML_CPP_DLL_EXPORTS
#define YAML_CPP_API __declspec(dllexport)
#else
#define YAML_CPP_API __declspec(dllimport)
#endif
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

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

	void SetTransform(const glm::vec3& position, const glm::vec3 & scale);
	void Translate(const glm::dvec3& translation);
	void Rotate(double rads, const glm::dvec3& axis);
	void Scale(const glm::dvec3& scale);

	glm::vec3 GetPosition() const { return position; }
	void setPos(const glm::vec3& pos) { position = pos; }

	glm::vec3 GetScale() const {
		return scale;
	}
	void setScale(const glm::vec3& s) { scale = s; }

	glm::vec3 GetRotation() const {
		return rotation;
	}
	void SetRot(const glm::vec3& rot) { rotation = rot; }

	void SetCanvasPosition(const glm::vec3& pos) { canvasPosition = pos; }
	glm::vec3 GetCanvasPosition() const { return canvasPosition; }

	void SetCanvasSize(const glm::vec3& size) { canvasSize = size; }
	glm::vec3 GetCanvasSize() const { return canvasSize; }

	void SetPivotOffset(const glm::vec3& off) { pivotOffset = off; }
	glm::vec3 GetPivotOffset() const { return pivotOffset; }

	bool GetResised() { return resized; }
	void SetResized(bool resized) { this->resized = resized; }

	void SetSacale(const glm::vec3& scale) { this->scale = scale; }

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
	//posicion, rotacion, escala
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 canvasPosition;
	glm::vec3 canvasSize;

	glm::vec3 pivotOffset;
	bool resized = false;

	//union
	//{
	//	//DONT modify directly, use SetMatrix
	//	glm::dmat4 matrix = glm::dmat4(1.0);
	//	struct
	//	{
	//		glm::dvec3 left; glm::dmat4::value_type left_w;
	//		glm::dvec3 up; glm::dmat4::value_type up_w;
	//		glm::dvec3 forward; glm::dmat4::value_type fwd_w;
	//		glm::dvec3 position; glm::dmat4::value_type pos_w;
	//	};
	//};

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

		node["position"][0] = position.x;
		node["position"][1] = position.y;
		node["position"][2] = position.z;

		node["rotation"][0] = rotation.x;
		node["rotation"][1] = rotation.y;
		node["rotation"][2] = rotation.z;

		node["scale"][0] = scale.x;
		node["scale"][1] = scale.y;
		node["scale"][2] = scale.z;

		node["pivot_offset"][0] = pivotOffset.x;
		node["pivot_offset"][1] = pivotOffset.y;
		node["pivot_offset"][2] = pivotOffset.z;

		return node;
	}

	bool decode(const YAML::Node& node) override {

		if (!Component::decode(node)) {
			return false;
		}

		if (!node["position"] || !node["rotation"] || !node["scale"] || !node["pivot_offset"]) {
			return false;
		}

		position.x = node["position"][0].as<float>();
		position.y = node["position"][1].as<float>();
		position.z = node["position"][2].as<float>();

		rotation.x = node["rotation"][0].as<float>();
		rotation.y = node["rotation"][1].as<float>();
		rotation.z = node["rotation"][2].as<float>();

		scale.x = node["scale"][0].as<float>();
		scale.y = node["scale"][1].as<float>();
		scale.z = node["scale"][2].as<float>();

		pivotOffset.x = node["pivot_offset"][0].as<float>();
		pivotOffset.y = node["pivot_offset"][1].as<float>();
		pivotOffset.z = node["pivot_offset"][2].as<float>();

		SetTransform(position, scale);

		return true;
	}
};

