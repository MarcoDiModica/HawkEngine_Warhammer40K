#include "MeshRendererComponent.h"
#include "GameObject.h" 
#include "TransformComponent.h"
#include "Mesh.h" 
#include "Material.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Image.h"

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

void MeshRenderer::SetImage(std::shared_ptr<Image> image)
{
	this->image = image;
}

std::shared_ptr<Image> MeshRenderer::GetImage() const
{
	return image;
}

void MeshRenderer::Render() const
{
    if (material)
    {
        glEnable(GL_TEXTURE_2D);
        material->bind();
    }

    if (mesh) 
    {    
        glPushMatrix();
        glMultMatrixd(owner.lock()->GetTransform()->GetData()); 

        mesh->Draw();

        glPopMatrix();
    }

    if (material)
	{
		glDisable(GL_TEXTURE_2D);
	}
}