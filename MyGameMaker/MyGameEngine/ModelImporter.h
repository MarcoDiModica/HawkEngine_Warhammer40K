#pragma once

#include <string>
#include "GameObject.h"
#include <vector>
#include <iostream>

using namespace std;

class Mesh;
class Material;
struct aiScene;
struct aiNode;

class ModelImporter {
public:
	std::shared_ptr<GameObject> loadFromFile(const std::string& path);
	/* Saves fbx into custom file format .mesh*/
	void EncodeFBXScene(const std::string path, std::vector<std::shared_ptr<Mesh>> meshes, const aiScene* fbx_scene);
};