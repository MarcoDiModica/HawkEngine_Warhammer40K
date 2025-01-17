#pragma once
#include <type_traits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <concepts>
#include <stdexcept>
  #ifdef YAML_CPP_DLL_EXPORTS
    #define YAML_CPP_API __declspec(dllexport)
  #else
    #define YAML_CPP_API __declspec(dllimport)
  #endif
#include <yaml-cpp/yaml.h>
#include <mono/metadata/object.h>

class GameObject;
class SceneSerializer;

enum class ComponentType {
	NONE,
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	SCRIPT,
	LIGHT
};

class Component
{
public:
	explicit Component(GameObject* owner) : owner(owner) {}
	virtual ~Component() = default;

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Destroy() = 0;

	virtual ComponentType GetType() const = 0;

	virtual std::unique_ptr<Component> Clone(GameObject* new_owner) = 0;

	GameObject* GetOwner() const { return owner; }
	std::string GetName() const { return name; }

	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool enabled) { this->enabled = enabled; }

	void SetName(const std::string& name) { this->name = name; }

	GameObject* owner;

	//GameObject* owner2;

	virtual  MonoObject* GetSharp() { return nullptr; }

protected:

	MonoObject* CsharpReference;

	friend class SceneSerializer;


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