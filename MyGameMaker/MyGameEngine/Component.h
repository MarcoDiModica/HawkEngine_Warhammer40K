#pragma once
#include <type_traits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <concepts>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

class GameObject;
class SceneSerializer;

class Component
{
public:
	explicit Component(GameObject* owner) : owner(owner) {}
	virtual ~Component() = default;

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Destroy() = 0;

	virtual std::shared_ptr<Component> Clone() = 0;

	GameObject* GetOwner() const { return owner; }
	std::string GetName() const { return name; }

	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool enabled) { this->enabled = enabled; }

	void SetName(const std::string& name) { this->name = name; }

protected:

	friend class SceneSerializer;

	GameObject* owner;
	std::string name;
	bool enabled = true;

	bool IsOwnerValid() const { return owner; }

	// Saves the important attributes returning a Yaml node
	virtual YAML::Node encode() {
		YAML::Node node;

		node["name"] = name;
		node["enabled"] = enabled;

		return node;
	}

	// Pass the Yaml Node you want to extract data from
	virtual bool decode(const YAML::Node& node) {
		if (!node["name"] || !node["enabled"])
			return false;

		name = node["name"].as<std::string>();
		enabled = node["enabled"].as<bool>();

		return true;
	}
};

template <typename T>
concept IsComponent = std::is_base_of<Component, T>::value;