#include "MeshRendererComponent.h"
#include "GameObject.h" 
#include "TransformComponent.h"
#include "Mesh.h" 
#include "Material.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

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

void MeshRenderer::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

std::shared_ptr<Material> MeshRenderer::GetMaterial() const
{
	return material;
}

void MeshRenderer::Render() const
{
    if (!mesh || !IsOwnerValid()) return;

    auto ownerPtr = GetOwner();
    auto transform = ownerPtr->GetComponent<Transform_Component>();

    glm::dmat4 modelMatrix = transform->GetModelMatrix();
    glMultMatrixd(&modelMatrix[0][0]);
    glColor3b(color.r, color.g, color.b);

    if (material)
	{
		material->Bind();
	}

    if (mesh)
    {
        mesh->Draw();
    }

    for (const auto& child : ownerPtr->GetChildren())
	{
		if (child->HasComponent<MeshRenderer>())
		{
			auto meshRenderer = child->GetComponent<MeshRenderer>();
			meshRenderer->Render();
		}
	}

    glPopMatrix();
}