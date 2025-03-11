#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UICanvasComponent : public Component
{
public:
	explicit UICanvasComponent(GameObject* owner);
	~UICanvasComponent() override = default;

	UICanvasComponent(const UICanvasComponent&) = delete;
	UICanvasComponent& operator=(const UICanvasComponent&) = delete;

	UICanvasComponent(UICanvasComponent&&) noexcept = default;
	UICanvasComponent& operator=(UICanvasComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::CANVAS; }

	float GetMonitorWidth() const { return monitorHeight; }
	float GetMonitorHeight() const { return monitorHeight; }

private:
	float monitorWidth;
	float monitorHeight;

	float width;
	float height;

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

		node["light_type"] = static_cast<int>(type);
		//node["color"] = { color.r, color.g, color.b };
		node["intensity"] = intensity;
		node["radius"] = radius;
		//node["direction"] = { direction.x, direction.y, direction.z };

		return node;
	}

	bool decode(const YAML::Node& node) override {

		Component::decode(node);

		type = static_cast<LightType>(node["light_type"].as<int>());
		//color = { node["color"][0].as<float>(), node["color"][1].as<float>(), node["color"][2].as<float>() };
		intensity = node["intensity"].as<float>();
		radius = node["radius"].as<float>();
		//direction = { node["direction"][0].as<float>(), node["direction"][1].as<float>(), node["direction"][2].as<float>() };

		return true;
	}
};