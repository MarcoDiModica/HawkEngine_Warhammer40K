#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include "../MyGameEditor/Log.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyGameEditor/Root.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "yaml-cpp/yaml.h"
#include "MyGameEditor/BindlessManager.h"

class Mesh;
class Material;
class Image;
class ShaderComponent;
class GameObject;

class MeshRenderer : public Component {
public:
	explicit MeshRenderer(GameObject* owner);
	~MeshRenderer() override = default;

	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	ComponentType GetType() const override { return ComponentType::MESH_RENDERER; }

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	void SetMesh(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh() const;

	void SetMaterial(std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial() const;

	void SetImage(std::shared_ptr<Image> image);

	void SetColor(const glm::vec3& color);
	glm::vec3 GetColor() const;

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

	void SetupLightProperties(Shaders* shader, const glm::vec3& viewPos) const;
	
private:
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    std::shared_ptr<Material> material = std::make_shared<Material>();
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

protected:
    friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node = Component::encode();

		if (mesh) {
			node["meshID"] = std::to_string(mesh->getModel()->GetID());
			std::string meshID = std::to_string(mesh->getModel()->GetID());
			mesh->SaveBinary(meshID);
		}
		else {
			node["meshID"] = "";
		}

		if (material) {
			std::string materialName = material->GetMatName();
			node["materialName"] = materialName.empty() ? "" : materialName;
			if (material->matID == 0) {
				std::hash<std::string> hasher;
				material->matID = static_cast<unsigned int>(hasher(materialName));
			}
			material->SaveBinary(materialName);
		}
		else {
			node["materialName"] = "";
		}

		node["color"] = std::vector<float>{ color.x, color.y, color.z };

		return node;
	}

	bool decode(const YAML::Node& node) override {
		if (node["meshID"] && !node["meshID"].as<std::string>().empty()) {
			std::string meshID = node["meshID"].as<std::string>();
			try {
				std::shared_ptr<Mesh> loadedMesh = Mesh::LoadBinary(meshID);
				if (loadedMesh) {
					SetMesh(loadedMesh);
					LOG(LogType::LOG_INFO, "Loaded mesh from binary with ID: %s", meshID.c_str());
				}
				else {
					LOG(LogType::LOG_ERROR, "Failed to load mesh binary with ID: %s", meshID.c_str());
					return false;
				}
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "Exception loading mesh binary: %s", e.what());
				return false;
			}
		}
		else if (node["mesh"]) {
			std::shared_ptr<Mesh> loadedMesh = std::make_shared<Mesh>();
			if (!loadedMesh->decode(node["mesh"])) {
				LOG(LogType::LOG_ERROR, "Failed to decode mesh in MeshRenderer");
				return false;
			}

			if (node["id"]) {
				int id = node["id"].as<int>();
				loadedMesh->getModel()->SetID(id);
			}

			auto mesh = Application->root->GetResourceManager()->AddMesh(loadedMesh);
			SetMesh(mesh);
			BindlessManager::GetInstance().RegisterMesh(mesh.get());
		}

		if (node["materialName"] && !node["materialName"].as<std::string>().empty()) {
			std::string materialName = node["materialName"].as<std::string>();
			try {
				auto mat = Application->root->GetResourceManager()->GetMaterial(materialName);
				if (mat) {
					SetMaterial(mat);
					BindlessManager::GetInstance().RegisterMaterial(mat.get());
					LOG(LogType::LOG_INFO, "Found material in ResourceManager: %s", materialName.c_str());
				}
				else {
					std::shared_ptr<Material> loadedMaterial = Material::LoadBinary(materialName);
					if (loadedMaterial) {
						auto mat = Application->root->GetResourceManager()->AddMaterial(loadedMaterial);
						SetMaterial(mat);
						BindlessManager::GetInstance().RegisterMaterial(mat.get());
						LOG(LogType::LOG_INFO, "Loaded material from binary: %s", materialName.c_str());
					}
					else {
						LOG(LogType::LOG_ERROR, "Failed to load material binary: %s", materialName.c_str());
					}
				}
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "Exception loading material binary: %s", e.what());
			}
		}
		else if (node["material"]) {
			std::shared_ptr<Material> loadedMaterial;
			YAML::Node matnode = node["material"];
			std::string name = matnode["name"].as<std::string>();
			if (Application->root->GetResourceManager()->GetMaterial(name) != nullptr) {
				auto mat = Application->root->GetResourceManager()->GetMaterial(name);
				SetMaterial(mat);
				BindlessManager::GetInstance().RegisterMaterial(mat.get());
			}
			else {
				loadedMaterial = std::make_shared<Material>();
				if (!loadedMaterial->decode(node["material"])) {
					LOG(LogType::LOG_ERROR, "Failed to decode material in MeshRenderer");
					return false;
				}
				auto mat = Application->root->GetResourceManager()->AddMaterial(loadedMaterial);
				SetMaterial(mat);
				BindlessManager::GetInstance().RegisterMaterial(mat.get());
			}
		}

		if (node["color"] && node["color"].IsSequence() && node["color"].size() == 3) {
			glm::vec3 decodedColor;
			decodedColor.x = node["color"][0].as<float>();
			decodedColor.y = node["color"][1].as<float>();
			decodedColor.z = node["color"][2].as<float>();
			SetColor(decodedColor);
		}

		return true;
	}
};