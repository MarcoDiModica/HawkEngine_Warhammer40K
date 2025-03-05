#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include "../MyGameEditor/Log.h"

class Mesh;
class Material;
class Image;
class Shaders;

class MeshRenderer : public Component {
public:
    explicit MeshRenderer(GameObject* owner);
    ~MeshRenderer() override = default;

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
//std::shared_ptr<Image> GetImage() const;

    void SetColor(const glm::vec3& color);
    glm::vec3 GetColor() const;

    void Render() const;
    void RenderMainCamera() const;

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

private:
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    std::shared_ptr<Material> material = std::make_shared<Material>();
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


protected:
	friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node = Component::encode();

		if (mesh) {
			node["mesh_path"] = mesh->filePath;
			node["mesh_name"] = mesh->nameM;
		}

		if (material) {
			std::string materialFilename = "material_" + std::to_string(material->GetId());
			material->SaveBinary(materialFilename);

			node["material_path"] = materialFilename;

			YAML::Node colorNode;
			colorNode.push_back(material->GetColor().r);
			colorNode.push_back(material->GetColor().g);
			colorNode.push_back(material->GetColor().b);
			colorNode.push_back(material->GetColor().a);
			node["color"] = colorNode;

			if (material->imagePtr && !material->imagePtr->image_path.empty()) {
				node["image_path"] = material->imagePtr->image_path;
			}
		}

		return node;
	}

	bool decode(const YAML::Node& node) {
		if (!Component::decode(node)) {
			return false;
		}

		if (!node["mesh_path"]) {
			LOG(LogType::LOG_ERROR, "MeshRenderer: Missing mesh_path in node");
			return false;
		}

		std::string path = node["mesh_path"].as<std::string>();
		std::shared_ptr<Mesh> _mesh = nullptr;

		if (path.substr(0, 6) == "shapes" || path.substr(0, 5) == "shape") {
			if (path.find("cube") != std::string::npos) {
				_mesh = Mesh::CreateCube();
				LOG(LogType::LOG_INFO, "MeshRenderer: Created cube mesh");
			}
			else if (path.find("sphere") != std::string::npos) {
				_mesh = Mesh::CreateSphere();
				LOG(LogType::LOG_INFO, "MeshRenderer: Created sphere mesh");
			}
			else if (path.find("plane") != std::string::npos) {
				_mesh = Mesh::CreatePlane();
				LOG(LogType::LOG_INFO, "MeshRenderer: Created plane mesh");
			}
			else if (path.find("cylinder") != std::string::npos) {
				_mesh = Mesh::CreateCylinder();
				LOG(LogType::LOG_INFO, "MeshRenderer: Created cylinder mesh");
			}
		}
		else if (node["mesh_name"]) {
			std::string meshName = node["mesh_name"].as<std::string>();
			_mesh = Mesh::LoadBinary(meshName);

			if (!_mesh) {
				LOG(LogType::LOG_ERROR, "MeshRenderer: Failed to load mesh %s", meshName.c_str());
				_mesh = Mesh::CreateCube();
			}
		}

		if (_mesh) {
			SetMesh(_mesh);
		}
		else {
			LOG(LogType::LOG_ERROR, "MeshRenderer: Failed to create or load mesh");
			return false;
		}

		std::shared_ptr<Material> _material = std::make_shared<Material>();

		if (node["material_path"]) {
			std::string materialPath = node["material_path"].as<std::string>();
			try {
				_material = Material::LoadBinary(materialPath);
				LOG(LogType::LOG_INFO, "MeshRenderer: Loaded material %s", materialPath.c_str());
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "MeshRenderer: Error loading material %s: %s",
					materialPath.c_str(), e.what());
				_material = std::make_shared<Material>();
			}
		}

		if (node["color"] && node["color"].IsSequence() && node["color"].size() == 4) {
			glm::vec4 color;
			color.r = node["color"][0].as<float>();
			color.g = node["color"][1].as<float>();
			color.b = node["color"][2].as<float>();
			color.a = node["color"][3].as<float>();
			_material->SetColor(color);
		}

		if (node["image_path"]) {
			std::string imagePath = node["image_path"].as<std::string>();
			auto image = std::make_shared<Image>();
			image->LoadTexture(imagePath);

			if (image->id() != 0) {
				_material->setImage(image);
				LOG(LogType::LOG_INFO, "MeshRenderer: Loaded image %s", imagePath.c_str());
			}
			else {
				LOG(LogType::LOG_ERROR, "MeshRenderer: Failed to load image %s", imagePath.c_str());
			}
		}

		SetMaterial(_material);
		return true;
	}

};