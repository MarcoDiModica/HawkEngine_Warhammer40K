#include "MeshRendererComponent.h"
#include "GameObject.h" 
#include "TransformComponent.h"
#include "Mesh.h" 
#include "Material.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Image.h"
#include "Shaders.h"
#include <iostream>

//cosailegal
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Camera.h"
#include "../MyGameEditor/Transform.h"

MeshRenderer::MeshRenderer(std::weak_ptr<GameObject> owner) : Component(owner) { name = "MeshRenderer"; }

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

std::shared_ptr<Component> MeshRenderer::Clone()
{
	auto meshRenderer = std::make_shared<MeshRenderer>(*this);
	meshRenderer->mesh = mesh;
	meshRenderer->material = material;
	meshRenderer->image = image;
	meshRenderer->color = color;
	return meshRenderer;
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
    material->setImage(image);
    this->image = image;
}

std::shared_ptr<Image> MeshRenderer::GetImage() const
{
    return image;
}

void MeshRenderer::Render() const
{

    if (material && material->useShader)
    {
        material->bindShaders();
        material->setShaderUniform("aPos", glm::vec3(0, 0, 0));
        material->setShaderUniform("model", owner.lock()->GetComponent<Transform_Component>()->GetMatrix());
        material->setShaderUniform("view", Application->camera->view());
        material->setShaderUniform("projection", Application->camera->projection());




     

        
    }

    if (material)
    {
        glEnable(GL_TEXTURE_2D);
        material->bind();
        material->setShaderUniform("texture1", 0); // Pasar la unidad de textura al shader
    }
  
    if (mesh)
    {
        mesh->Draw();
    }


    if (material)
    {
        glDisable(GL_TEXTURE_2D);
    }
}