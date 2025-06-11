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

struct PendingReference {
	ScriptComponent* scriptComponent;
	MonoClassField* field;
	std::string goName;
};



class SceneSerializer : public Module
{
public:
	SceneSerializer(App* app);
	~SceneSerializer() = default;

	void Serialize(const std::string& directoryPath, bool play = false);
	bool DeSerialize(const std::string& path);
	YAML::Node SerializeGameObject(GameObject& gameObject);
	std::shared_ptr<GameObject> DeserializeGameObject(const YAML::Node& node);

	std::vector<PendingReference> g_PendingScriptReferences;
	void ApplyComponentDelta(GameObject* gameObject, const YAML::Node& prefabComponents);
	void RemoveComponentByName(GameObject* gameObject, const std::string& name);
	void TraverseGameObjects(std::shared_ptr<GameObject> gameObject);
	void TraverseAllGameObjects();
private:
	YAML::Node SerializeComponents(GameObject& gameObject);
	void SerializeChildren(YAML::Node& parentNode, GameObject& gameObject);

	void DeserializeComponents(GameObject* gameObject, const YAML::Node& node);
	void DeserializeChildren(GameObject* parentGameObject, const YAML::Node& node);
	void DeserializePendingScriptComponents();

	void SaveToFile(const YAML::Node& root, const std::string& filepath);
	YAML::Node LoadFromFile(const std::string& filepath);
	std::string GetComponentTypeName(ComponentType type);
	ComponentType GetComponentTypeFromName(const std::string& name);
};

#endif // SCENE_SERIALIZER_H