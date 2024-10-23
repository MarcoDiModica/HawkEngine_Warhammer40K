#pragma once
#include "Component.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 

class Mesh;

class MeshRenderer : public Component
{
public:
    explicit MeshRenderer(std::weak_ptr<GameObject> owner);
    ~MeshRenderer() override = default;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    void SetMesh(std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> GetMesh() const;

    void SetColor(const glm::vec3& color);
    glm::vec3 GetColor() const;

    void Render() const;

private:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};