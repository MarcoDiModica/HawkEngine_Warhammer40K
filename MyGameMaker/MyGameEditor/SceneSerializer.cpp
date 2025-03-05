// Last update: 2021/10/12 - 20:00
#include <iostream>
#include <fstream>

#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include "Input.h"
#include "Log.h"

#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyShadersEngine/ShaderComponent.h"

SceneSerializer::SceneSerializer(App* app) : Module(app) {
	RegisterComponentType("Transform_Component", [](GameObject* gameObject, const YAML::Node& node) {
		gameObject->GetTransform()->decode(node);
		});

	RegisterComponentType("MeshRenderer", [this](GameObject* gameObject, const YAML::Node& node) {
		auto _mesh = std::make_shared<Mesh>();
		auto _material = std::make_shared<Material>();

		if (node["mesh_path"]) {
			auto path = node["mesh_path"].as<std::string>();

			if (path.substr(0, 6) == "shapes" || path.substr(0, 5) == "shape") {
				if (path.find("cube") != std::string::npos) {
					_mesh = Mesh::CreateCube();
					LOG(LogType::LOG_INFO, "MeshRenderer: Created cube mesh");
				}
				else if (path.find("sphere") != std::string::npos) {
					_mesh = Mesh::CreateSphere();
					LOG(LogType::LOG_INFO, "MeshRenderer: Created sphere mesh");
				}
				else if (path.find("plane") != std::string::npos) {
					_mesh = Mesh::CreatePlane();
					LOG(LogType::LOG_INFO, "MeshRenderer: Created plane mesh");
				}
				else if (path.find("cylinder") != std::string::npos) {
					_mesh = Mesh::CreateCylinder();
					LOG(LogType::LOG_INFO, "MeshRenderer: Created cylinder mesh");
				}
			}
			else if (node["mesh_name"]) {
				std::string meshName = node["mesh_name"].as<std::string>();
				_mesh = Mesh::LoadBinary(meshName);

				if (!_mesh) {
					LOG(LogType::LOG_ERROR, "MeshRenderer: Failed to load mesh %s", meshName.c_str());
					_mesh = Mesh::CreateCube();
				}
			}
		}

		if (node["material_path"]) {
			std::string materialPath = node["material_path"].as<std::string>();
			try {
				_material = Material::LoadBinary(materialPath);
				LOG(LogType::LOG_INFO, "MeshRenderer: Loaded material %s", materialPath.c_str());
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "MeshRenderer: Error loading material %s: %s",
					materialPath.c_str(), e.what());
				_material = std::make_shared<Material>();
			}
		}

		if (node["color"] && node["color"].IsSequence() && node["color"].size() == 4) {
			glm::vec4 color;
			color.r = node["color"][0].as<float>();
			color.g = node["color"][1].as<float>();
			color.b = node["color"][2].as<float>();
			color.a = node["color"][3].as<float>();
			_material->SetColor(color);
		}

		std::string texturePath = "default.png";
		if (node["image_path"]) {
			texturePath = node["image_path"].as<std::string>();
			auto image = std::make_shared<Image>();
			image->LoadTexture(texturePath);

			if (image->id() != 0) {
				_material->setImage(image);
				LOG(LogType::LOG_INFO, "MeshRenderer: Loaded image %s", texturePath.c_str());
			}
			else {
				LOG(LogType::LOG_ERROR, "MeshRenderer: Failed to load image %s", texturePath.c_str());
			}
		}

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(_mesh);
		meshRenderer->SetMaterial(_material);

		if (!texturePath.empty() && texturePath != "default.png") {
			auto image = std::make_shared<Image>();
			image->LoadTexture(texturePath);
			meshRenderer->SetImage(image);
		}
		});

	RegisterComponentType("CameraComponent", [this](GameObject* gameObject, const YAML::Node& node) {
		gameObject->AddComponent<CameraComponent>()->decode(node);
		});

	RegisterComponentType("LightComponent", [this](GameObject* gameObject, const YAML::Node& node) {
		gameObject->AddComponent<LightComponent>()->decode(node);
		});

	RegisterComponentType("ShaderComponent", [this](GameObject* gameObject, const YAML::Node& node) {
		gameObject->AddComponent<ShaderComponent>()->decode(node);
		});
}

void SceneSerializer::RegisterComponentType(const std::string& typeName,
	ComponentDeserializeFunc deserializeFunc) {
	m_componentRegistry[typeName] = deserializeFunc;
}

void SceneSerializer::Serialize(const std::string& directoryPath, bool play) {
	std::vector<std::shared_ptr<GameObject>> gameObjects = Application->root->GetActiveScene()->_children;

	if (gameObjects.empty()) {
		std::string filepath = directoryPath + "/" + Application->root->GetActiveScene()->GetName() + ".scene";
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << "";
			file.close();
		}
		return;
	}

	YAML::Node rootNode;

	YAML::Node sceneMetaNode;
	sceneMetaNode["SceneName"] = Application->root->GetActiveScene()->GetName();
	rootNode["SceneMetadata"] = sceneMetaNode;

	for (size_t i = 0; i < gameObjects.size(); ++i) {
		rootNode["GameObject" + std::to_string(i)] = SerializeGameObject(*gameObjects[i]);
	}

	YAML::Emitter emitter;
	emitter << rootNode;

	std::string filepath;
	if (!play) {
		filepath = directoryPath + "/" + Application->root->GetActiveScene()->GetName() + ".scene";
	}
	else {
		filepath = "EngineAssets/" + Application->root->GetActiveScene()->GetName() + ".scene";
	}

	try {
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << emitter.c_str();
			file.close();
			LOG(LogType::LOG_INFO, "Scene saved successfully: %s", filepath.c_str());
		}
		else {
			LOG(LogType::LOG_ERROR, "Could not open file to save the scene: %s", filepath.c_str());
		}
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error saving the scene: %s", e.what());
	}
}

YAML::Node SceneSerializer::SerializeGameObject(GameObject& gameObject, int depth) {
	YAML::Node node;

	node["name"] = gameObject.GetName();
	node["tag"] = gameObject.tag;
	node["active"] = gameObject.IsActive();
	node["isStatic"] = gameObject.isStatic;

	for (auto& component : gameObject.components) {
		if (component.second) {
			YAML::Node componentNode = component.second->encode();
			node[component.second->GetName()] = componentNode;
		}
	}

	if (!gameObject.scriptComponents.empty()) {
		YAML::Node scriptsNode;
		for (size_t i = 0; i < gameObject.scriptComponents.size(); ++i) {
			if (gameObject.scriptComponents[i]) {
				YAML::Node scriptNode = gameObject.scriptComponents[i]->encode();
				scriptsNode["Script" + std::to_string(i)] = scriptNode;
			}
		}
		node["ScriptComponents"] = scriptsNode;
	}

	SerializeChildren(node, gameObject, depth + 1);

	return node;
}

void SceneSerializer::SerializeChildren(YAML::Node& parentNode, GameObject& gameObject, int depth) {
	const auto& children = gameObject.GetChildren();
	for (size_t i = 0; i < children.size(); ++i) {
		parentNode["Child" + std::to_string(i)] = SerializeGameObject(*children[i], depth);
	}
}

void SceneSerializer::DeSerialize(const std::string& path) {
	try {
		YAML::Node rootNode = YAML::LoadFile(path);

		LOG(LogType::LOG_INFO, "Deserializing scene: %s", path.c_str());

		if (rootNode.IsNull()) {
			LOG(LogType::LOG_ERROR, "Error loading the scene: file is empty or invalid.");
			return;
		}

		if (Application->root->GetActiveScene() != nullptr) {
			for (const auto& child : Application->root->GetActiveScene()->_children) {
				child->isSelected = false;
			}
			Application->input->ClearSelection();
		}

		std::string sceneName = path.substr(path.find_last_of("/\\") + 1);
		sceneName = sceneName.substr(0, sceneName.find_last_of('.'));

		if (rootNode["SceneMetadata"] && rootNode["SceneMetadata"]["SceneName"]) {
			sceneName = rootNode["SceneMetadata"]["SceneName"].as<std::string>();
		}

		LOG(LogType::LOG_INFO, "Loading Scene: %s", sceneName.c_str());

		if (Application->root->GetActiveScene() != nullptr) {
			Application->root->RemoveScene(Application->root->GetActiveScene()->GetName());
		}

		Application->root->CreateScene(sceneName);
		Application->root->SetActiveScene(sceneName);

		LOG(LogType::LOG_INFO, "Scene created: %s", sceneName.c_str());

		for (int i = 0; ; i++) {
			YAML::Node objectNode = rootNode["GameObject" + std::to_string(i)];
			if (!objectNode || !objectNode["name"].IsDefined()) {
				break;
			}

			auto gameObject = DeserializeGameObject(objectNode);

			DeserializeChildren(gameObject.get(), objectNode);
		}

		LOG(LogType::LOG_INFO, "Scene deserialized successfully: %s", sceneName.c_str());
	}
	catch (const YAML::Exception& e) {
		LOG(LogType::LOG_ERROR, "YAML Exception during deserialization: %s", e.what());
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Exception during deserialization: %s", e.what());
	}
}

std::shared_ptr<GameObject> SceneSerializer::DeserializeGameObject(const YAML::Node& node) {
	if (!node["name"].IsDefined()) {
		LOG(LogType::LOG_ERROR, "GameObject node doesn't have a name defined!");
		return Application->root->CreateGameObject("Error_MissingName");
	}

	auto objectName = node["name"].as<std::string>();
	std::shared_ptr<GameObject> gameObject = Application->root->CreateGameObject(objectName);

	if (node["tag"].IsDefined()) {
		gameObject->tag = node["tag"].as<std::string>();
	}

	if (node["active"].IsDefined()) {
		gameObject->SetActive(node["active"].as<bool>());
	}

	if (node["isStatic"].IsDefined()) {
		gameObject->isStatic = node["isStatic"].as<bool>();
	}

	DeserializeComponents(gameObject.get(), node);

	ProcessScriptComponents(gameObject.get(), node);

	return gameObject;
}

void SceneSerializer::DeserializeComponents(GameObject* gameObject, const YAML::Node& node) {
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
		const auto key = it->first.as<std::string>();
		const YAML::Node& value = it->second;

		if (key == "name" || key == "tag" || key == "active" || key == "isStatic" ||
			key.substr(0, 5) == "Child" || key == "ScriptComponents") {
			continue;
		}

		if (m_componentRegistry.find(key) != m_componentRegistry.end()) {
			m_componentRegistry[key](gameObject, value);
		}
		else {
			LOG(LogType::LOG_WARNING, "Unknown component type: %s", key.c_str());
		}
	}
}

void SceneSerializer::ProcessScriptComponents(GameObject* gameObject, const YAML::Node& node) {
	if (node["ScriptComponents"] && node["ScriptComponents"].IsMap()) {
		for (YAML::const_iterator scriptIt = node["ScriptComponents"].begin();
			scriptIt != node["ScriptComponents"].end(); ++scriptIt) {
			const YAML::Node& scriptNode = scriptIt->second;
			if (scriptNode["name"].IsDefined()) {
				auto scriptName = scriptNode["name"].as<std::string>();
				// gameObject->AddScriptComponent(scriptName)->decode(scriptNode);
			}
		}
	}
}

void SceneSerializer::DeserializeChildren(GameObject* parentObject, const YAML::Node& node) {
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
		const auto key = it->first.as<std::string>();
		const YAML::Node& value = it->second;

		if (key.substr(0, 5) == "Child") {
			auto childObject = DeserializeGameObject(value);
			parentObject->AddChild(childObject.get());

			DeserializeChildren(childObject.get(), value);
		}
	}
}