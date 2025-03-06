#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include <glm/glm.hpp>
#include <memory>

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

	void SetColor(const glm::vec3& color);
	glm::vec3 GetColor() const;

	void SetNormalMap(std::shared_ptr<Image> normalMap);
	void SetRoughnessMap(std::shared_ptr<Image> roughnessMap);
	void SetMetalnessMap(std::shared_ptr<Image> metalnessMap);
	void SetAmbientOcclusionMap(std::shared_ptr<Image> aoMap);
	void SetEmissiveMap(std::shared_ptr<Image> emissiveMap);

	void SetRoughnessValue(float value);
	void SetMetalnessValue(float value);
	void SetAmbientOcclusionValue(float value);
	void SetEmissiveColor(const glm::vec3& color);
	void SetEmissiveIntensity(float intensity);

	std::shared_ptr<Image> GetNormalMap() const;
	std::shared_ptr<Image> GetRoughnessMap() const;
	std::shared_ptr<Image> GetMetalnessMap() const;
	std::shared_ptr<Image> GetAmbientOcclusionMap() const;
	std::shared_ptr<Image> GetEmissiveMap() const;

	float GetRoughnessValue() const;
	float GetMetalnessValue() const;
	float GetAmbientOcclusionValue() const;
	glm::vec3 GetEmissiveColor() const;
	float GetEmissiveIntensity() const;

	void Render() const;
	void ConfigureLights() const;
	void ConfigurePBRLights() const;
	void ConfigureTraditionalShader() const;
	void RenderMainCamera() const;

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    std::shared_ptr<Material> material = std::make_shared<Material>();
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


protected:
    YAML::Node encode() override 
	{
		YAML::Node node = Component::encode();

		node["mesh_path"] = mesh->filePath;
		node["mesh_name"] = mesh->nameM;

		if (material) {
			/*std::string materialFilename = "material_" + std::to_string(material->id());
			material->SaveBinary(materialFilename);
			node["material_path"] = materialFilename + ".mat";*/
		}

		return node;
    }

    bool decode(const YAML::Node& node) 
	{
        Component::decode(node);

        if (!node["mesh_path"] || !node["image_path"])
            return false;

        // node["mesh_path"].as<std::string>();

        auto _mesh = std::make_shared<Mesh>();
        std::string path = node["mesh_path"].as<std::string>();

        if (path.substr(0, 6) == "shape") {
            if (path.find("cube")) {
                _mesh = Mesh::CreateCube();
            }
            else if (path.find("sphere")) {
                _mesh = Mesh::CreateSphere();
            }
            else if (path.find("plane")) {
                _mesh = Mesh::CreatePlane();
            }
        }
        else {
             //_mesh->LoadMesh(path.c_str());
        }
        SetMesh(_mesh);

        if (node["material_path"]) {
            //std::string materialPath = node["material_path"].as<std::string>();
            //material = Material::LoadBinary(materialPath);

            //if (!material) {
            //    //log error
            //}
        }
        SetMaterial(material);

        return true;
    }

};