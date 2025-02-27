#pragma region Includes
#define _CRT_SECURE_NO_WARNINGS
#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include "../MyGameEngine/Material.h"
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "../MyGameEngine/CameraComponent.h"
#include "Input.h"
#pragma endregion

class MeshRenderer;

YAML::Node SceneSerializer::ObjectSerialize(GameObject& child, int num) {
	YAML::Node node;
	node["name"] = child.GetName();
	node["tag"] = child.tag;
	node["active"] = child.IsActive();
	node["isStatic"] = child.isStatic;

	for (auto& component : child.components) {
		if (component.second) {
			YAML::Node node2 = component.second->encode();
			node[component.second->GetName()] = node2;
		}
	}

	if (!child.scriptComponents.empty()) {
		YAML::Node scriptsNode;
		for (size_t i = 0; i < child.scriptComponents.size(); ++i) {
			if (child.scriptComponents[i]) {
				YAML::Node scriptNode = child.scriptComponents[i]->encode();
				scriptsNode["Script" + std::to_string(i)] = scriptNode;
			}
		}
		node["ScriptComponents"] = scriptsNode;
	}

	int j = 0;
	for (const auto& childObj : child.GetChildren()) {
		node["Child" + std::to_string(j)] = ObjectSerialize(*childObj, j);
		j++;
	}

	return node;
}

void SceneSerializer::Serialize(const std::string& directoryPath, bool play) {
	std::vector<std::shared_ptr<GameObject>> gameObjects = Application->root->GetActiveScene()->_children;

	YAML::Emitter emitter;
	YAML::Node game_object_node;

	if (gameObjects.empty()) {
		std::string filepath = directoryPath + "/" + Application->root->GetActiveScene()->GetName() + ".scene";
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << "";
			file.close();
		}
		return;
	}

	YAML::Node sceneMetaNode;
	sceneMetaNode["SceneName"] = Application->root->GetActiveScene()->GetName();
	game_object_node["SceneMetadata"] = sceneMetaNode;

	for (size_t i = 0; i < gameObjects.size(); ++i) {
		YAML::Node node;
		node["name"] = gameObjects[i]->GetName();
		node["tag"] = gameObjects[i]->tag;
		node["active"] = gameObjects[i]->IsActive();
		node["isStatic"] = gameObjects[i]->isStatic;

		for (auto& component : gameObjects[i]->components) {
			if (component.second) {
				YAML::Node node2 = component.second->encode();
				node[component.second->GetName()] = node2;
			}
		}

		if (!gameObjects[i]->scriptComponents.empty()) {
			YAML::Node scriptsNode;
			for (size_t j = 0; j < gameObjects[i]->scriptComponents.size(); ++j) {
				if (gameObjects[i]->scriptComponents[j]) {
					YAML::Node scriptNode = gameObjects[i]->scriptComponents[j]->encode();
					scriptsNode["Script" + std::to_string(j)] = scriptNode;
				}
			}
			node["ScriptComponents"] = scriptsNode;
		}

		int j = 0;
		for (const auto& child : gameObjects[i]->GetChildren()) {
			node["Child" + std::to_string(j)] = ObjectSerialize(*child, j);
			j++;
		}

		game_object_node["GameObject" + std::to_string(i)] = node;
	}

	emitter << game_object_node;

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


void SceneSerializer::DeSerialize(std::string path) {
	try {
		YAML::Node root = YAML::LoadFile(path);

		LOG(LogType::LOG_INFO, "Deserializing scene: %s", path.c_str());

		if (root.IsNull()) {
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

		if (root["SceneMetadata"] && root["SceneMetadata"]["SceneName"]) {
			sceneName = root["SceneMetadata"]["SceneName"].as<std::string>();
		}

		LOG(LogType::LOG_INFO, "Loading Scene: %s", sceneName.c_str());

		if (Application->root->GetActiveScene() != nullptr) {
			Application->root->RemoveScene(Application->root->GetActiveScene()->GetName());
		}

		Application->root->CreateScene(sceneName);
		Application->root->SetActiveScene(sceneName);

		LOG(LogType::LOG_INFO, "Scene created: %s", sceneName.c_str());

		for (int i = 0; ; i++) {
			YAML::Node _node = root["GameObject" + std::to_string(i)];
			if (!_node || !_node["name"].IsDefined()) {
				break;
			}

			auto objectName = _node["name"].as<std::string>();
			std::shared_ptr<GameObject> game_obj = Application->root->CreateGameObject(objectName);

			if (_node["tag"].IsDefined()) {
				game_obj->tag = _node["tag"].as<std::string>();
			}

			if (_node["active"].IsDefined()) {
				game_obj->SetActive(_node["active"].as<bool>());
			}

			if (_node["isStatic"].IsDefined()) {
				game_obj->isStatic = _node["isStatic"].as<bool>();
			}

			for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {
				const auto key = it->first.as<std::string>();
				const YAML::Node& value = it->second;

				if (key == "Transform_Component" || key == "MeshRenderer" || key == "CameraComponent" ||
					key == "LightComponent" || key == "ShaderComponent") {
					if (value["name"].IsDefined()) {
						auto component_name = value["name"].as<std::string>();

						if (component_name == "Transform_Component") {
							game_obj->GetTransform()->decode(value);
						}
						else if (component_name == "MeshRenderer") {
							auto _mesh = std::make_shared<Mesh>();
							auto _material = std::make_shared<Material>();

							if (value["mesh_path"]) {
								auto path = value["mesh_path"].as<std::string>();

								if (path.substr(0, 6) == "Shapes") {
									if (path.find("Cube") != std::string::npos) {
										_mesh = Mesh::CreateCube();
									}
									else if (path.find("Sphere") != std::string::npos) {
										_mesh = Mesh::CreateSphere();
									}
									else if (path.find("Plane") != std::string::npos) {
										_mesh = Mesh::CreatePlane();
									}
								}
								else {
									auto meshPath = value["mesh_path"].as<std::string>();
									auto node_name = value["mesh_name"].as<std::string>();
									_mesh->nameM = node_name;

									_mesh = Mesh::LoadBinary(node_name);

									if (_mesh == nullptr) {
										LOG(LogType::LOG_ERROR, "Error loading mesh %s", node_name.c_str());
										_mesh = Mesh::CreateCube();
									}
								}
							}

							if (value["material_path"]) {
								auto path = value["material_path"].as<std::string>();
								//_material = _material->LoadBinary(path);

								if (!_material) {
									LOG(LogType::LOG_ERROR, "Error loading material %s", path.c_str());
									_material = std::make_shared<Material>();
								}
							}

							std::string texturePath = "default.png";
							if (value["image_path"]) {
								texturePath = value["image_path"].as<std::string>();
							}

							Application->root->AddMeshRenderer(*game_obj, _mesh/*, texturePath, _material*/);
						}
						else if (component_name == "CameraComponent") {
							game_obj->AddComponent<CameraComponent>()->decode(value);
						}
						else if (component_name == "LightComponent") {
							//game_obj->AddComponent<LightComponent>()->decode(value);
						}
						else if (component_name == "ShaderComponent") {
							//game_obj->AddComponent<ShaderComponent>()->decode(value);
						}
					}
				}
				else if (key == "ScriptComponents" && value.IsMap()) {
					for (YAML::const_iterator scriptIt = value.begin(); scriptIt != value.end(); ++scriptIt) {
						const YAML::Node& scriptNode = scriptIt->second;
						if (scriptNode["name"].IsDefined()) {
							auto scriptName = scriptNode["name"].as<std::string>();
							//game_obj->AddScriptComponent(scriptName)->decode(scriptNode);
						}
					}
				}
				else if (key.substr(0, 5) == "Child") {
					// do nothin here, we will deserialize children later
				}
			}
		}

		for (int i = 0; ; i++) {
			YAML::Node _node = root["GameObject" + std::to_string(i)];
			if (!_node || !_node["name"].IsDefined()) {
				break;
			}

			auto objectName = _node["name"].as<std::string>();

			std::shared_ptr<GameObject> parent_obj = nullptr;
			for (const auto& obj : Application->root->GetActiveScene()->_children) {
				if (obj->GetName() == objectName) {
					parent_obj = obj;
					break;
				}
			}

			if (parent_obj) {
				for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {
					const auto key = it->first.as<std::string>();
					const YAML::Node& value = it->second;

					if (key.substr(0, 5) == "Child") {
						GameObject& child = DeSerializeChild(value, root);
						parent_obj->AddChild(&child);
					}
				}
			}
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

GameObject& SceneSerializer::DeSerializeChild(YAML::Node _node, YAML::Node& mesh_root_node) {
	if (!_node["name"].IsDefined()) {
		LOG(LogType::LOG_ERROR, "Child node doesn't have a name defined!");
		return *Application->root->CreateGameObject("Error_MissingName");
	}

	auto objectName = _node["name"].as<std::string>();
	std::shared_ptr<GameObject> game_obj = Application->root->CreateGameObject(objectName);

	// Set common properties
	if (_node["tag"].IsDefined()) {
		game_obj->tag = _node["tag"].as<std::string>();
	}

	if (_node["active"].IsDefined()) {
		game_obj->SetActive(_node["active"].as<bool>());
	}

	if (_node["isStatic"].IsDefined()) {
		game_obj->isStatic = _node["isStatic"].as<bool>();
	}

	for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {
		const auto key = it->first.as<std::string>();
		const YAML::Node& value = it->second;

		if (key == "Transform_Component" || key == "MeshRenderer" || key == "CameraComponent" ||
			key == "LightComponent" || key == "ShaderComponent") {
			if (value["name"].IsDefined()) {
				auto component_name = value["name"].as<std::string>();

				if (component_name == "Transform_Component") {
					game_obj->GetTransform()->decode(value);
				}
				else if (component_name == "MeshRenderer") {
					auto _mesh = std::make_shared<Mesh>();
					auto _material = std::make_shared<Material>();

					if (value["mesh_path"]) {
						auto path = value["mesh_path"].as<std::string>();

						if (path.substr(0, 6) == "shapes") {
							if (path.find("cube") != std::string::npos) {
								_mesh = Mesh::CreateCube();
							}
							else if (path.find("sphere") != std::string::npos) {
								_mesh = Mesh::CreateSphere();
							}
							else if (path.find("plane") != std::string::npos) {
								_mesh = Mesh::CreatePlane();
							}
						}
						else {
							auto meshPath = value["mesh_path"].as<std::string>();
							auto node_name = value["mesh_name"].as<std::string>();
							_mesh->nameM = node_name;
							_mesh = Mesh::LoadBinary(node_name);

							if (_mesh == nullptr) {
								LOG(LogType::LOG_ERROR, "Error loading mesh %s", node_name.c_str());
								_mesh = Mesh::CreateCube();
							}
						}
					}

					if (value["material_path"]) {
						auto path = value["material_path"].as<std::string>();
						_material = _material->LoadBinary(path);

						if (!_material) {
							LOG(LogType::LOG_ERROR, "Error loading material %s", path.c_str());
							_material = std::make_shared<Material>();
						}
					}

					std::string texturePath = "default.png";
					if (value["image_path"]) {
						texturePath = value["image_path"].as<std::string>();
					}

					Application->root->AddMeshRenderer(*game_obj, _mesh, texturePath, _material);
				}
				else if (component_name == "CameraComponent") {
					game_obj->AddComponent<CameraComponent>()->decode(value);
				}
				else if (component_name == "LightComponent") {
					//game_obj->AddComponent<LightComponent>()->decode(value);
				}
				else if (component_name == "ShaderComponent") {
					//game_obj->AddComponent<ShaderComponent>()->decode(value);
				}
			}
		}
		else if (key == "ScriptComponents" && value.IsMap()) {
			for (YAML::const_iterator scriptIt = value.begin(); scriptIt != value.end(); ++scriptIt) {
				const YAML::Node& scriptNode = scriptIt->second;
				if (scriptNode["name"].IsDefined()) {
					auto scriptName = scriptNode["name"].as<std::string>();
					// Example: game_obj->AddScriptComponent(scriptName)->decode(scriptNode);
				}
			}
		}
		else if (key.substr(0, 5) == "Child") {
			GameObject& child = DeSerializeChild(value, mesh_root_node);
			game_obj->AddChild(&child);
		}
	}

	return *game_obj;
}