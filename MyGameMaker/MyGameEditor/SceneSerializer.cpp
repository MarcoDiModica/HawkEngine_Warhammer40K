#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

class MeshRenderer;

void SceneSerializer::Serialize(const std::string& directoryPath) {

	std::vector gameObjects = Application->root->currentScene->_children;

	YAML::Emitter emitter;
	YAML::Node game_object_node;

	if (gameObjects.empty()) {
		std::string filepath = directoryPath + "/" + Application->root->currentScene->GetName() + ".scene";
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
		//for (size_t j = 0; j < gameObjects[i]->children().size(); ++j) {

		//	YAML::Node node;
		//	node["Child" + std::to_string(j)] = ObjectSerialize(gameObjects[i]->children()[j], j);

		//}
		int j = 0;
		for (auto it = gameObjects[i]->children().begin(); it != gameObjects[i]->children().end(); ++it) {

			node["Child" + std::to_string(j)] = ObjectSerialize(*it, j);

			j++;
		}

		game_object_node["GameObject" + std::to_string(i)] = node;


	}
	emitter << game_object_node;

	std::string filepath = directoryPath + "/" + Application->root->currentScene->GetName() + ".scene";

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

	LOG(LogType::LOG_INFO, "Deserializando escena: %s", path.c_str());

	if (root.IsNull()) {
		LOG(LogType::LOG_ERROR, "Error al cargar la escena.");
		return;
	}

	if (Application->root->GetActiveScene() != nullptr)
	{
		for (const auto& child : Application->root->currentScene->_children) {
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
		Application->root->RemoveScene(Application->root->currentScene->GetName());
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

				if (key == "Transform_Component" || key == "MeshRenderer") {
					if (value["name"].IsDefined()) {
						std::string component_name = value["name"].as<std::string>();

						if (component_name == "Transform_Component") {
							game_obj->transform->decode(value);
						}
						if (component_name == "MeshRenderer") {

							auto _mesh = std::make_shared<Mesh>();
							std::string path = value["mesh_path"].as<std::string>();

							if (path.substr(0, 6) == "shapes") {
								if (path.find("cube")) {
									_mesh = Mesh::CreateCube();
								}
								else if (path.find("sphere")) {
									_mesh = Mesh::CreateSphere();
								}
								else if (path.find("plane")) {
									_mesh = Mesh::CreatePlane();
								}
							}
							else {
								_mesh->LoadMesh(path.c_str());
							}

							Application->root->AddMeshRenderer(*game_obj, _mesh, value["image_path"].as<std::string>() );
						}
					}
				}
				else if (key.substr(0, 5) == "Child") {

					/* Deserialize child and add to children vector */
					Application->root->ParentGameObject( DeSerializeChild(value) , *game_obj );


				}
			}
			i++;

		}
	}
}

GameObject& SceneSerializer::DeSerializeChild(YAML::Node _node) {

	std::shared_ptr<GameObject> game_obj = Application->root->CreateGameObject(_node["name"].as<std::string>());

	for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it) {

		const std::string key = it->first.as<std::string>();
		const YAML::Node& value = it->second;

		if (key == "Transform_Component" || key == "MeshRenderer") {
			if (value["name"].IsDefined()) {
				std::string component_name = value["name"].as<std::string>();

				if (component_name == "Transform_Component") {
					game_obj->transform->decode(value);
				}
				if (component_name == "MeshRenderer") {

					auto _mesh = std::make_shared<Mesh>();
					std::string path = value["mesh_path"].as<std::string>();

					if (path.substr(0, 6) == "shapes") {
						if (path.find("cube")) {
							_mesh = Mesh::CreateCube();
						}
						else if (path.find("sphere")) {
							_mesh = Mesh::CreateSphere();
						}
						else if (path.find("plane")) {
							_mesh = Mesh::CreatePlane();
						}
					}
					else {
						_mesh->LoadMesh(path.c_str());
					}
					Application->root->AddMeshRenderer(*game_obj, _mesh, value["image_path"].as<std::string>());
				}
			}
		}
		else if (key.substr(0, 5) == "Child") {
			/* Deserialize child and add to children vector, RECURSIVE */
			Application->root->ParentGameObject(DeSerializeChild(value), *game_obj);
		}
	}
	return *game_obj;

}