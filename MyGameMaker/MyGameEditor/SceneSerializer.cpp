#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>



void SceneSerializer::Serialize() {

	std::vector gameObjects = Application->root->currentScene->_children;

	YAML::Emitter emitter;
	YAML::Node game_object_node;


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

		/*node["transform"] = gameObjects[i]->GetTransform()->encode();*/
		game_object_node["GameObject" + std::to_string(i)] = node;
		/*Save node to the emitter*/
		emitter << game_object_node;
	}

	std::string saved_string = emitter.c_str();
	
	std::ofstream file("Serialized/scene.yaml");

	if (file.is_open()) {
		file << saved_string;

		file.close();
	}

}


void SceneSerializer::DeSerialize(std::string path) {

	YAML::Emitter emitter;
	YAML::Node root = YAML::LoadFile(path);

	Application->root->currentScene->Destroy();

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

					}
				}


			}

			//for (const auto& component_node : _node) {

			//	if (component_node.IsDefined() && component_node["name"].IsDefined()) {

			//		std::string component_name = component_node["name"].as<std::string>();

			//		if (component_name == "Transform_Component") {

			//			game_obj->transform->decode(component_node);
			//		}
			//	}
			//	else {
			//		int h = 9;
			//	}
			//}
		}

	}


	emitter << root;

	std::string saved_string = emitter.c_str();

	int j = 9;

}