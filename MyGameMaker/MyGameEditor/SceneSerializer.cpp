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


class MeshRenderer;

YAML::Node SceneSerializer::ObjectSerialize(GameObject& child, int num) {

	//YAML::Node game_object_node;
	YAML::Node node;
	node["name"] = child.GetName();

	/*            Save the components                 */
	for (auto& component : child.components)
	{
		YAML::Node node2;
		node2 = component.second->encode();

		node[component.second->name] = node2;
	}
	/* If it has children , save children node inside */

	int j = 0;
	for (const auto& child : child.GetChildren()) {
		node["Child" + std::to_string(j)] = ObjectSerialize(*child, j);
		j++;
	}

	return node;
}

void SceneSerializer::Serialize(const std::string& directoryPath, bool play) {

	std::vector gameObjects = Application->root->GetActiveScene()->_children;

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

	for (size_t i = 0; i < gameObjects.size(); ++i) {

		/*Define a node with the object's properties*/
		YAML::Node node;
		node["name"] = gameObjects[i]->GetName();

		for (auto& component : gameObjects[i]->components)
		{
			YAML::Node node2;
			node2 = component.second->encode();

			node[component.second->name] = node2;
		}

		/* If it has children , save children node inside */

		int j = 0;
		for (const auto& child : gameObjects[i]->GetChildren()) {
			node["Child" + std::to_string(j)] = ObjectSerialize(*child, j);
			j++;
		}

		game_object_node["GameObject" + std::to_string(i)] = node;


	}
	emitter << game_object_node;

	std::string filepath;
	if (!play) filepath = directoryPath + "/" + Application->root->GetActiveScene()->GetName() + ".scene";
	else { filepath = "EngineAssets/" + Application->root->GetActiveScene()->GetName() + ".scene"; }

	try {
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << emitter.c_str();
			file.close();
		}
		else {
			LOG(LogType::LOG_ERROR, "No se pudo abrir el archivo para guardar la escena.");
		}
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error al guardar la escena: %s", e.what());
	}

}

void SceneSerializer::DeSerialize(std::string path) {

	YAML::Emitter emitter;
	YAML::Node root = YAML::LoadFile(path);
	YAML::Node mesh_root_node;


	LOG(LogType::LOG_INFO, "Deserializando escena: %s", path.c_str());

	if (root.IsNull()) {
		LOG(LogType::LOG_ERROR, "Error al cargar la escena.");
		return;
	}

	if (Application->root->GetActiveScene() != nullptr)
	{
		for (const auto& child : Application->root->GetActiveScene()->_children) {
			child->isSelected = false;
		}

		Application->input->ClearSelection();
	}

	std::string sceneName = path.substr(path.find_last_of("/\\") + 1);
	sceneName = sceneName.substr(0, sceneName.find_last_of("."));

	LOG(LogType::LOG_INFO, "Scene Name: %s", sceneName.c_str());

	std::shared_ptr<Scene> scene = nullptr;
	for (auto& s : Application->root->scenes) {
		if (s->GetName() == sceneName) {
			scene = s;
			break;
		}
	}

	if (Application->root->GetActiveScene() != nullptr) {
		Application->root->RemoveScene(Application->root->GetActiveScene()->GetName());
	}

	Application->root->CreateScene(sceneName);
	Application->root->SetActiveScene(sceneName);

	Application->root->currentScene->_children.clear();

	int i = 0;
	for (const auto& child : root) {

		YAML::Node _node = root["GameObject" + std::to_string(i)];
		if (_node["name"].IsDefined()) {


			std::shared_ptr<GameObject> game_obj = Application->root->CreateGameObject(_node["name"].as<std::string>());

			//---Load Components---//

			for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {

				const std::string key = it->first.as<std::string>();
				const YAML::Node& value = it->second;

				if (key == "Transform_Component" || key == "MeshRenderer" || key == "CameraComponent") {
					if (value["name"].IsDefined()) {
						std::string component_name = value["name"].as<std::string>();

						if (component_name == "Transform_Component") {
							game_obj->GetTransform()->decode(value);
						}
						if (component_name == "MeshRenderer") {

							auto _mesh = std::make_shared<Mesh>();
							auto _material = std::make_shared<Material>();
							if (value["mesh_path"]) {
								std::string path = value["mesh_path"].as<std::string>();

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
									std::string meshPath = value["mesh_path"].as<std::string>();
									std::string node_name = value["mesh_name"].as<std::string>();
									_mesh->nameM = node_name;

									_mesh = Mesh::LoadBinary(node_name);

									if (_mesh == nullptr) {
										LOG(LogType::LOG_ERROR, "Error loading mesh %s", node_name.c_str());
									}
								}
							}

							if (value["material_path"]) {

								/*std::string path = value["material_path"].as<std::string>();
								_material = _material->LoadBinary(path);*/
							}

							Application->root->AddMeshRenderer(*game_obj, _mesh, "default.png", _material);
						}
						if (component_name == "CameraComponent") {
							game_obj->AddComponent<CameraComponent>()->decode(value);
						}
					}
				}
				else if (key.substr(0, 5) == "Child") {

					/* Deserialize child and add to children vector */
					game_obj->AddChild(&DeSerializeChild(value, mesh_root_node));
				}
			}
			i++;

		}
	}

	// Reassign to delete memory, works like smart ptr
	//mesh_root_node = YAML::Node();

}

GameObject& SceneSerializer::DeSerializeChild(YAML::Node _node, YAML::Node& mesh_root_node) {

	std::shared_ptr<GameObject> game_obj = Application->root->CreateGameObject(_node["name"].as<std::string>());

	for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {

		const std::string key = it->first.as<std::string>();
		const YAML::Node& value = it->second;

		if (key == "Transform_Component" || key == "MeshRenderer" || key == "CameraComponent") {
			if (value["name"].IsDefined()) {
				std::string component_name = value["name"].as<std::string>();

				if (component_name == "Transform_Component") {
					game_obj->GetTransform()->decode(value);
				}
				if (component_name == "MeshRenderer") {

					auto _mesh = std::make_shared<Mesh>();
					auto _material = std::make_shared<Material>();
					if (value["mesh_path"]) {
						std::string path = value["mesh_path"].as<std::string>();

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
							std::string meshPath = value["mesh_path"].as<std::string>();
							std::string node_name = value["mesh_name"].as<std::string>();
							_mesh->nameM = node_name;
							_mesh = Mesh::LoadBinary(node_name);

							if (_mesh == nullptr) {
								LOG(LogType::LOG_ERROR, "Error loading mesh %s", node_name.c_str());
							}
						}
					}

					if (value["material_path"]) {

						/*std::string path = value["material_path"].as<std::string>();
						_material = _material->LoadBinary(path);*/
					}

					Application->root->AddMeshRenderer(*game_obj, _mesh,"default.png", _material);


				}
				if (component_name == "CameraComponent") {
					game_obj->AddComponent<CameraComponent>()->decode(value);
				}
			}
		}
		else if (key.substr(0, 5) == "Child") {
			/* Deserialize child and add to children vector, RECURSIVE */
			Application->root->ParentGameObject(DeSerializeChild(value,  mesh_root_node), *game_obj);
			//game_obj->AddChild(&DeSerializeChild(value));
		}
	}
	return *game_obj;

}