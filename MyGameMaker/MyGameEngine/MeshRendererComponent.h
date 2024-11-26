#pragma once
#include "Component.h"
#include "Mesh.h"
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

    std::shared_ptr<Component> Clone(GameObject* owner) override;

    void SetMesh(std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> GetMesh() const;

    void SetMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> GetMaterial() const;

    void SetImage(std::shared_ptr<Image> image);
    std::shared_ptr<Image> GetImage() const;

    void SetColor(const glm::vec3& color);
    glm::vec3 GetColor() const;

    void Render() const;

private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Image> image;
    std::shared_ptr<Material> material;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

protected:
    YAML::Node encode() override {

        YAML::Node node = Component::encode();

        node["mesh_path"] = mesh->filePath;
        node["image_path"] = image->image_path;

        return node;
    }

    bool decode(const YAML::Node& node) {

        Component::decode(node);

        if (!node["mesh_path"] || !node["image_path"])
            return false;

        // node["mesh_path"].as<std::string>();

        auto _mesh = std::make_shared<Mesh>();
        _mesh->LoadMesh("Assets/Meshes/BakerHouse.fbx");
        SetMesh(_mesh);


        return true;
    }

};