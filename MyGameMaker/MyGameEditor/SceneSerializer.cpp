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
		//game_object_node["GameObject"] = node;
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


void SceneSerializer::DeSerialize(std::string path) {

	YAML::Emitter emitter;
	YAML::Node node = YAML::LoadFile(path);

	emitter << node;

	std::string saved_string = emitter.c_str();

	int i = 9;

}