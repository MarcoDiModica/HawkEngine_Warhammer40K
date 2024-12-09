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
#include "../MyGameEditor/EditorCamera.h"
#include "../MyGameEditor/Transform.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner) { name = "MeshRenderer"; }

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

std::unique_ptr<Component> MeshRenderer::Clone(GameObject* owner)
{
    auto meshRenderer = std::make_unique<MeshRenderer>(*this);
    meshRenderer->mesh = mesh;
    meshRenderer->material = material;
    meshRenderer->image = image;
    meshRenderer->color = color;
    meshRenderer->owner = owner;
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
        material->setShaderUniform("model", owner->GetTransform()->GetMatrix());
        material->setShaderUniform("modColor", glm::vec4( 1,0.2f,0,1));
        if (material->GetColor().x > 0) 
        {
            material->GetColor();
        }
        glUniform4f(glGetUniformLocation(material->shader.GetProgram(), "modColor"), material->GetColor().x, material->GetColor().y, material->GetColor().z, material->GetColor().w);
        //material->setShaderUniform("model", owner->GetComponent<Transform_Component>()->GetMatrix());
        material->setShaderUniform("view", Application->camera->view());
        material->setShaderUniform("projection", Application->camera->projection());

        material->setShaderUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        material->setShaderUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        material->setShaderUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        material->setShaderUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        material->setShaderUniform("dirLight.intensity", 3.0f);

        material->setShaderUniform("viewPos", Application->camera->GetTransform().GetPosition());

        material->setShaderUniform("pointLights.position", glm::vec3(0.0f, 0.0f, 0.0f));
        material->setShaderUniform("pointLights.ambient", glm::vec3(0.2f, 0.8f, 0.2f));
        material->setShaderUniform("pointLights.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        material->setShaderUniform("pointLights.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        material->setShaderUniform("pointLights.constant", 1.0f);
        material->setShaderUniform("pointLights.linear", 0.09f);
        material->setShaderUniform("pointLights.quadratic", 0.032f);
        material->setShaderUniform("pointLights.radius", 1.0f);
        material->setShaderUniform("pointLights.intensity", 2.5f);

    }

    if (material)
    {
        color4 newColor = color4(material->color.x, material->color.y, material->color.z, material->color.w);
        
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