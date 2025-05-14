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
	void SaveMaterials();

	//meshes
	std::shared_ptr<Mesh> AddMesh(std::shared_ptr<Mesh> mesh) {
		if (meshIndex.find(mesh->getModel()->GetID()) != meshIndex.end()) {
			return meshes[meshIndex[mesh->getModel()->GetID()]];
		}
		meshes.push_back(mesh);
		meshIndex[mesh->getModel()->GetID()] = meshes.size() - 1;
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

	//materials
	std::shared_ptr<Material> AddMaterial(std::shared_ptr<Material> material) {
		if (materialIndex.find(material->GetMatName()) != materialIndex.end()) {
			return materials[materialIndex[material->GetMatName()]];
		}
		materials.push_back(material);
		materialIndex[material->GetMatName()] = materials.size() - 1;
		materials[materials.size() - 1]->SaveBinary(material->GetMatName());
		return materials.back();
	}

	std::shared_ptr<Material> GetMaterial(std::string id) {
		if (materialIndex.find(id) != materialIndex.end()) {
			return materials[materialIndex[id]];
		}
		return nullptr;
	}

	void ClearMaterial(std::string id) {
		if (materialIndex.find(id) != materialIndex.end()) {
			materials.erase(materials.begin() + materialIndex[id]);
			materialIndex.erase(id);
		}
	}

	int GetMaterialCount() const {
		return materials.size();
	}

	void ClearAllMaterials() {
		materials.clear();
		materialIndex.clear();
	}

	//images
	std::shared_ptr<Image> AddImage(std::shared_ptr<Image> image) {
		if (imageIndex.find(image->image_name) != imageIndex.end()) {
			return images[imageIndex[image->image_name]];
		}
		images.push_back(image);
		imageIndex[image->image_name] = images.size() - 1;
		return images.back();
	}

	std::shared_ptr<Image> GetImage(std::string id) {
		if (imageIndex.find(id) != imageIndex.end()) {
			return images[imageIndex[id]];
		}
		return nullptr;
	}

	void ClearImage(std::string id) {
		if (imageIndex.find(id) != imageIndex.end()) {
			images.erase(images.begin() + imageIndex[id]);
			imageIndex.erase(id);
		}
	}

	int GetImageCount() const {
		return images.size();
	}

	void ClearAllImages() {
		images.clear();
		imageIndex.clear();
	}

	std::shared_ptr<Mesh> Cube;
	std::shared_ptr<Material> DefaultMaterial;

	std::shared_ptr<Material> GetDefaultMaterial();

	void CreateDefaultCube() {
		Cube = Mesh::CreateCube();
		if (materials.size() > 0) {
			DefaultMaterial = materials[0];
		}
		else {
			DefaultMaterial = GetDefaultMaterial();
		}
	}

	void DeleteAllUselessResources();

private:
	//meshes
	std::unordered_map<size_t, size_t> meshIndex;
	std::vector<std::shared_ptr<Mesh>> meshes;
	//materials
	std::unordered_map<std::string, size_t> materialIndex;
	std::vector<std::shared_ptr<Material>> materials;
	//images
	std::unordered_map<std::string, size_t> imageIndex;
	std::vector<std::shared_ptr<Image>> images;


};

