#pragma once
#ifndef LOOK_UP_H
#define LOOK_UP_H
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>
#include <map>
#include <string>

#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"

namespace SharpBinder {

	// table to translate c# class name to cpp class index
	static std::unordered_map<std::string, std::type_index > PlusLookUpTable;
	// table to translate cpp typeindex into sharp class name
	static std::unordered_map<std::type_index, std::string > SharpLookUpTable;


	static std::unordered_map<std::type_index, int > SharpLookUpTable2;

	void InitializeLookUpTable();



}






#endif