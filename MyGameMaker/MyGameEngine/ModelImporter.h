#pragma once

#include <string>
#include "GameObject.h"
#include <vector>
#include <iostream>
#include "../MyAnimationEngine/Animation.h"

using namespace std;

class Mesh;
class Material;
struct aiScene;
struct aiNode;

class ModelImporter {
public:
	void loadFromFile(const std::string& path);
	/* Saves fbx into custom file format .mesh*/
	void EncodeFBXScene(const std::string path, std::vector<std::shared_ptr<Mesh>> meshes, const aiScene* fbx_scene);
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<GameObject>> fbx_object;
	void graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials, glm::mat4 accumulatedTransform);
};
