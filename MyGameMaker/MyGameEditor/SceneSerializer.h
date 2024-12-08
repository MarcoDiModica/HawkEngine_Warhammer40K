#pragma once
#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H



#include "Module.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>

class SceneSerializer : public Module
{
private:
	
public:

	SceneSerializer(App* app) : Module(app) { ; }

	void Serialize(const std::string& directoryPath, bool play = false);

	void DeSerialize(std::string path);

	YAML::Node ObjectSerialize(GameObject& child, int num);

	GameObject& DeSerializeChild(YAML::Node _node);


};


#endif