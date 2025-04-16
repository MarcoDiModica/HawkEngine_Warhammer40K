#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "Mesh.h"
#include "Model.h"
#include "Material.h"

class ResourceManager
{
public:
	ResourceManager() { };
	~ResourceManager();

	void Init();
	void CleanUp();

	void LoadResources();
	void LoadTextures();
	void LoadModels();
	void LoadMaterials();

	std::shared_ptr<Mesh> AddMesh(std::shared_ptr<Mesh> mesh) {
		if (meshIndex.find(mesh->getModel()->GetID()) != meshIndex.end()) {
			return meshes[meshIndex[mesh->getModel()->GetID()]];
		}
		meshes.push_back(mesh);
		meshIndex[mesh->getModel()->GetID()] = meshes.size() - 1;
		std::string str = std::to_string(mesh->getModel()->GetID());
		meshes[meshes.size() - 1]->SaveBinary(str);
		return meshes.back();
	}

	std::shared_ptr<Mesh> GetMesh(size_t id) {
		if (meshIndex.find(id) != meshIndex.end()) {
			return meshes[meshIndex[id]];
		}
		return nullptr;
	}

	void ClearMesh(size_t id) {
		if (meshIndex.find(id) != meshIndex.end()) {
			meshes.erase(meshes.begin() + meshIndex[id]);
			meshIndex.erase(id);
		}
	}

	int GetMeshCount() const {
		return meshes.size();
	}

	void ClearAllMeshes() {
		meshes.clear();
		meshIndex.clear();
	}

private:
	std::unordered_map<size_t, size_t> meshIndex;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
};

