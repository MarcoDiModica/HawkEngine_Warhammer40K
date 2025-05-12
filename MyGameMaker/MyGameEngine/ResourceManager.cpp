#include "ResourceManager.h"

#include <filesystem>

namespace fs = std::filesystem;

void ResourceManager::LoadResources()
{
	meshIndex.clear();
	materialIndex.clear();
	meshes.clear();
	materials.clear();
	imageIndex.clear();
	images.clear();
	LoadModels();
	LoadTextures();
	LoadMaterials();
}

void ResourceManager::LoadModels()
{
	std::string modelPath = std::filesystem::current_path().string() + "\\Library\\Mesh\\";
	fs::path modelDir(modelPath);
	for (const auto& entry : std::filesystem::directory_iterator(modelDir))
	{
		if (entry.path().extension() == ".mesh")
		{
			std::shared_ptr<Mesh> loadedMesh = std::make_shared<Mesh>();
			std::string meshName = entry.path().stem().string();
			loadedMesh = loadedMesh->LoadBinary(meshName);
			AddMesh(loadedMesh);
		}
	}
}

void ResourceManager::LoadTextures()
{
	std::string texturePath = std::filesystem::current_path().string() + "\\Library\\Images\\";
	fs::path textureDir(texturePath);
	for (const auto& entry : std::filesystem::directory_iterator(textureDir))
	{
		if (entry.path().extension() == ".image")
		{
			std::shared_ptr<Image> loadedImage = std::make_shared<Image>();
			std::string imageName = entry.path().stem().string();
			loadedImage = loadedImage->LoadBinary(imageName);
			AddImage(loadedImage);
		}
	}
}

void ResourceManager::LoadMaterials()
{
	std::string materialPath = std::filesystem::current_path().string() + "\\Library\\Materials\\";

	fs::path materialeDir(materialPath);
	for (const auto& entry : std::filesystem::directory_iterator(materialeDir))
	{
		if (entry.path().extension() == ".mat") {

			std::string materialName = entry.path().string();

			std::ifstream fin(materialName, std::ios::binary);

			std::shared_ptr<Material> loadedMaterial = std::make_shared<Material>();

			loadedMaterial->matName = entry.path().stem().string();

			fin.read(reinterpret_cast<char*>(&loadedMaterial->wrapMode), sizeof(loadedMaterial->wrapMode));
			fin.read(reinterpret_cast<char*>(&loadedMaterial->filter), sizeof(loadedMaterial->filter));
			fin.read(reinterpret_cast<char*>(&loadedMaterial->color), sizeof(loadedMaterial->color));
			fin.read(reinterpret_cast<char*>(&loadedMaterial->shaderType), sizeof(loadedMaterial->shaderType));

			while (fin.peek() != EOF) {
				char type[4];
				fin.read(type, 3);
				type[3] = '\0';

				uint32_t pathLen;
				fin.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));

				std::string texturePath(pathLen, '\0');
				fin.read(&texturePath[0], pathLen);

				std::shared_ptr<Image> img; 
				if (GetImage(texturePath) != nullptr)
				{
					img = GetImage(texturePath);
				}
				else
				{
					img = Image::LoadBinary(texturePath);
					AddImage(img);
				}

				if (strcmp(type, "IMG") == 0) {
					loadedMaterial->setImage(img);
				}
				else if (strcmp(type, "NML") == 0) {
					loadedMaterial->setNormalMap(img);
				}
				else if (strcmp(type, "MTL") == 0) {
					loadedMaterial->setMetallicMap(img);
				}
				else if (strcmp(type, "RGL") == 0) {
					loadedMaterial->setRoughnessMap(img);
				}
				else if (strcmp(type, "AOM") == 0) {
					loadedMaterial->setAoMap(img);
				}
			}
			AddMaterial(loadedMaterial);
		}
	}
}

void ResourceManager::SaveMaterials()
{

	for (auto material : materials)
	{
		material->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		material->SaveBinary(material->GetMatName());
	}

}

void ResourceManager::CleanUp() {
	ClearAllImages();
	ClearAllMaterials();
	ClearAllMeshes();
}

std::shared_ptr<Material> ResourceManager::GetDefaultMaterial() {
	std::string materialPath = std::filesystem::current_path().string() + "\\Library\\Materials\\.mat";

	fs::path materialeDir(materialPath);
	std::ifstream fin(materialeDir, std::ios::binary);

	std::shared_ptr<Material> loadedMaterial = std::make_shared<Material>();

	loadedMaterial->matName = "";

	fin.read(reinterpret_cast<char*>(&loadedMaterial->wrapMode), sizeof(loadedMaterial->wrapMode));
	fin.read(reinterpret_cast<char*>(&loadedMaterial->filter), sizeof(loadedMaterial->filter));
	fin.read(reinterpret_cast<char*>(&loadedMaterial->color), sizeof(loadedMaterial->color));
	fin.read(reinterpret_cast<char*>(&loadedMaterial->shaderType), sizeof(loadedMaterial->shaderType));

	while (fin.peek() != EOF) {
		char type[4];
		fin.read(type, 3);
		type[3] = '\0';

		uint32_t pathLen;
		fin.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));

		std::string texturePath(pathLen, '\0');
		fin.read(&texturePath[0], pathLen);

		std::shared_ptr<Image> img;
		if (GetImage(texturePath) != nullptr)
		{
			img = GetImage(texturePath);
		}
		else
		{
			img = Image::LoadBinary(texturePath);
			AddImage(img);
		}

		if (strcmp(type, "IMG") == 0) {
			loadedMaterial->setImage(img);
		}
		else if (strcmp(type, "NML") == 0) {
			loadedMaterial->setNormalMap(img);
		}
		else if (strcmp(type, "MTL") == 0) {
			loadedMaterial->setMetallicMap(img);
		}
		else if (strcmp(type, "RGL") == 0) {
			loadedMaterial->setRoughnessMap(img);
		}
		else if (strcmp(type, "AOM") == 0) {
			loadedMaterial->setAoMap(img);
		}
	}
	AddMaterial(loadedMaterial);
	return loadedMaterial;
}