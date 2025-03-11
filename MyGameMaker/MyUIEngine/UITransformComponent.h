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
	void setRot(const glm::vec3& rot) { rotation = rot; }

	void setCanvasPosition(const glm::vec3& pos) { canvasPosition = pos; }
	glm::vec3 getCanvasPosition() const { return canvasPosition; }

	void setCanvasSize(const glm::vec3& size) { canvasSize = size; }
	glm::vec3 getCanvasSize() const { return canvasSize; }

private:
	//posicion, rotacion, escala
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 canvasPosition;
	glm::vec3 canvasSize;

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

		//node["light_type"] = static_cast<int>(type);
		////node["color"] = { color.r, color.g, color.b };
		//node["intensity"] = intensity;
		//node["radius"] = radius;
		//node["direction"] = { direction.x, direction.y, direction.z };

		return node;
	}

	bool decode(const YAML::Node& node) override {

		Component::decode(node);

		//type = static_cast<LightType>(node["light_type"].as<int>());
		////color = { node["color"][0].as<float>(), node["color"][1].as<float>(), node["color"][2].as<float>() };
		//intensity = node["intensity"].as<float>();
		//radius = node["radius"].as<float>();
		//direction = { node["direction"][0].as<float>(), node["direction"][1].as<float>(), node["direction"][2].as<float>() };

		return true;
	}
};

