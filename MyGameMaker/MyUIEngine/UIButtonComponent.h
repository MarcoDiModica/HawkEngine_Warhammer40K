#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

enum class ButtonState {
	DEFAULT,
	HOVERED,
	CLICKED
};

class UIButtonComponent : public Component
{
public:
	explicit UIButtonComponent(GameObject* owner);
	~UIButtonComponent() override = default;

	UIButtonComponent(const UIButtonComponent&) = delete;
	UIButtonComponent& operator=(const UIButtonComponent&) = delete;

	UIButtonComponent(UIButtonComponent&&) noexcept = default;
	UIButtonComponent& operator=(UIButtonComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::BUTTON; }
	
	ButtonState GetState() const { return state; }
	void SetState(ButtonState newState) { state = newState; }

private:
	ButtonState state = ButtonState::DEFAULT;

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

