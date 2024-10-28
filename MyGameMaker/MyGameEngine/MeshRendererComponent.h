#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Mesh;
class Material;
class Image;

class MeshRenderer : public Component {
public:
    explicit MeshRenderer(std::weak_ptr<GameObject> owner);
    ~MeshRenderer() override = default;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

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
};