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

		/*Save node to the emitter*/
		emitter << node;
	}

	std::string saved_string = emitter.c_str();
	
	std::ofstream file("Serialized/scene.yaml");

	if (file.is_open()) {
		file << saved_string;

		file.close();
	}

}