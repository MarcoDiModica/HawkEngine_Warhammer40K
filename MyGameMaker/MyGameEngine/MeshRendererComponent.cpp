#include "MeshRendererComponent.h"
#include "GameObject.h" 
#include "TransformComponent.h"
#include "Mesh.h" 
#include <GL/glew.h> 

MeshRenderer::MeshRenderer(std::weak_ptr<GameObject> owner) : Component(owner) {}

void MeshRenderer::Start()
{
	
}

void MeshRenderer::Update(float deltaTime)
{
	
}

void MeshRenderer::Destroy()
{
    mesh.reset();
}

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = mesh;
}

std::shared_ptr<Mesh> MeshRenderer::GetMesh() const
{
    return mesh;
}

void MeshRenderer::SetColor(const glm::vec3& color)
{
    this->color = color;
}

glm::vec3 MeshRenderer::GetColor() const
{
    return color;
}

void MeshRenderer::Render() const
{
    if (!mesh || !IsOwnerValid()) return;

    auto ownerPtr = GetOwner();
    auto transform = ownerPtr->GetComponent<Transform_Component>();

    glm::mat4 modelMatrix = transform->GetModelMatrix();

    mesh->Draw();
}