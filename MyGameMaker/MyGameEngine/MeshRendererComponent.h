#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include <glm/glm.hpp>

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

private:
    std::shared_ptr<Mesh> mesh;
   // std::shared_ptr<Image> image;
    std::shared_ptr<Material> material;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

protected:
    YAML::Node encode() override {

        YAML::Node node = Component::encode();

        node["mesh_path"] = mesh->filePath;
        //node["image_path"] = image->image_path;
        node["mesh_name"] = mesh->nameM;

        if (material) {
            std::string materialFilename = "material_" + std::to_string(material->GetId());
            material->SaveBinary(materialFilename);

            node["material_path"] = materialFilename + ".mat";
        }

        return node;
    }

    bool decode(const YAML::Node& node) {

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
             _mesh->LoadMesh(path.c_str());
        }
        SetMesh(_mesh);

        if (node["material_path"]) {
            std::string materialPath = node["material_path"].as<std::string>();
            material = Material::LoadBinary(materialPath);

            if (!material) {
                //log error
            }
        }
        SetMaterial(material);

        return true;
    }

};