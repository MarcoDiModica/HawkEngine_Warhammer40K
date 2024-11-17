#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>



void SceneSerializer::Serialize() {

	std::vector gameObjects = Application->root->currentScene->children;

	YAML::Emitter emitter;

	for (size_t i = 0; i < gameObjects.size(); ++i) {

		/*Define a node with the object's properties*/
		YAML::Node node;
		node["name"] = gameObjects[i]->GetName();

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