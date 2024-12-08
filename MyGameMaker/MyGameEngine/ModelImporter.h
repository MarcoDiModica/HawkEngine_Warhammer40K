#pragma once

#include <string>
#include "GameObject.h"
#include <vector>
#include <iostream>

using namespace std;

class Mesh;
class Material;
class aiScene;
class aiNode;

class ModelImporter {
public:
	void loadFromFile(const std::string& path);

	void graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials);
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<GameObject>> meshGameObjects;
};