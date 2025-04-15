#pragma once

#include <vector>
#include <memory>

#include "Mesh.h"
#include "Material.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void Init();
	void CleanUp();

	void LoadResources();
	void LoadTextures();
	void LoadModels();
	void LoadMaterials();

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
};

