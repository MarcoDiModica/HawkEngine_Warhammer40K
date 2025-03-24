#pragma once
#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "Module.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include <memory>

class App;
class Scene;

class SceneSerializer : public Module
{
public:
	SceneSerializer(App* app);
	~SceneSerializer() = default;

	void Serialize(const std::string& directoryPath, bool play = false);
	bool DeSerialize(const std::string& path);

private:
	YAML::Node SerializeGameObject(GameObject& gameObject);
	YAML::Node SerializeComponents(GameObject& gameObject);
	void SerializeChildren(YAML::Node& parentNode, GameObject& gameObject);

	std::shared_ptr<GameObject> DeserializeGameObject(const YAML::Node& node);
	void DeserializeComponents(GameObject* gameObject, const YAML::Node& node);
	void DeserializeChildren(GameObject* parentGameObject, const YAML::Node& node);

	void SaveToFile(const YAML::Node& root, const std::string& filepath);
	YAML::Node LoadFromFile(const std::string& filepath);
	std::string GetComponentTypeName(ComponentType type);
	ComponentType GetComponentTypeFromName(const std::string& name);
};

#endif // SCENE_SERIALIZER_H