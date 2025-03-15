#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>

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

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
	float monitorWidth;
	float monitorHeight;

	float width;
	float height;

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

		node["width"] = width;
		node["height"] = height;

		return node;
	}

	bool decode(const YAML::Node& node) override {

		Component::decode(node);

		height = node["height"].as<float>();
		width = node["width"].as<float>();
		
		return true;
	}
};