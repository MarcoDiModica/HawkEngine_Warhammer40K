#pragma once
#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include <functional>
#include <unordered_map>
#include <memory>

#include <yaml-cpp/yaml.h>

#include "Module.h"
#include "../MyGameEngine/GameObject.h"

class Component;
class GameObject;
class App;

using ComponentSerializeFunc = std::function<YAML::Node(Component*)>;
using ComponentDeserializeFunc = std::function<void(GameObject*, const YAML::Node&)>;

class SceneSerializer : public Module
{
public:
	SceneSerializer(App* app);
	~SceneSerializer() = default;

	void Serialize(const std::string& directoryPath, bool play = false);
	void DeSerialize(const std::string& path);

	void RegisterComponentType(const std::string& typeName,
		ComponentDeserializeFunc deserializeFunc);

private:

	YAML::Node SerializeGameObject(GameObject& gameObject, int depth = 0);
	void SerializeChildren(YAML::Node& parentNode, GameObject& gameObject, int depth);

	std::shared_ptr<GameObject> DeserializeGameObject(const YAML::Node& node);
	void DeserializeComponents(GameObject* gameObject, const YAML::Node& node);
	void DeserializeChildren(GameObject* parentObject, const YAML::Node& node);
	void ProcessScriptComponents(GameObject* gameObject, const YAML::Node& node);

	std::unordered_map<std::string, ComponentDeserializeFunc> m_componentRegistry;
};

#endif // SCENE_SERIALIZER_H