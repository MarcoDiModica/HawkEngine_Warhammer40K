#pragma once
#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H



#include "Module.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>


class SceneSerializer : public Module
{
	
public:

	SceneSerializer(App* app) : Module(app) { ; }

	void Serialize(const std::string& directoryPath);

	void DeSerialize(std::string path);

	YAML::Node ObjectSerialize(GameObject& child, int num) {

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
		for (auto it = child.children().begin(); it != child.children().end(); ++it) {

			node["Child" + std::to_string(j)] = ObjectSerialize(*it, j);
			j++;
		}

		return node;
	}

	GameObject& DeSerializeChild(YAML::Node _node);


};


#endif