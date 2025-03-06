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
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "LightComponent.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

//cosailegal
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/EditorCamera.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner) {
    name = "MeshRenderer";
    mesh = Mesh::CreateCube();
    auto image = std::make_shared<Image>();
    image->LoadTexture("default.png");
    material->SetAlbedoMap(image);
    material->SetColor(glm::dvec4(0.7f, 0.7f, 0.7f, 1.0f));

    owner->AddComponent<ShaderComponent>();
    owner->GetComponent<ShaderComponent>()->SetOwnerMaterial(material.get());
    owner->GetComponent<ShaderComponent>()->SetShaderType(ShaderType::PBR);
}

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
    //meshRenderer->image = image;
    meshRenderer->color = color;
    meshRenderer->owner = owner;
    return meshRenderer;
}

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
{
   /* this->mesh->nameM = mesh->nameM;*/
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

void MeshRenderer::SetNormalMap(std::shared_ptr<Image> normalMap)
{
    material->SetNormalMap(normalMap);
}

void MeshRenderer::SetRoughnessMap(std::shared_ptr<Image> roughnessMap)
{
	material->SetRoughnessMap(roughnessMap);
}

void MeshRenderer::SetMetalnessMap(std::shared_ptr<Image> metalnessMap)
{
	material->SetMetalnessMap(metalnessMap);
}

void MeshRenderer::SetAmbientOcclusionMap(std::shared_ptr<Image> aoMap)
{
	material->SetAOMap(aoMap);
}

void MeshRenderer::SetEmissiveMap(std::shared_ptr<Image> emissiveMap)
{
	material->SetEmissiveMap(emissiveMap);
}

void MeshRenderer::SetRoughnessValue(float value)
{
    material->SetRoughness(value);
}

void MeshRenderer::SetMetalnessValue(float value)
{
	material->SetMetalness(value);
}

void MeshRenderer::SetAmbientOcclusionValue(float value)
{
	material->SetAmbientOcclusion(value);
}

void MeshRenderer::SetEmissiveColor(const glm::vec3& color)
{
	material->SetEmissiveColor(color);
}

void MeshRenderer::SetEmissiveIntensity(float intensity)
{
	material->SetEmissiveIntensity(intensity);
}

std::shared_ptr<Image> MeshRenderer::GetNormalMap() const
{
	return material->GetNormalMap();
}

std::shared_ptr<Image> MeshRenderer::GetRoughnessMap() const
{
	return material->GetRoughnessMap();
}

std::shared_ptr<Image> MeshRenderer::GetMetalnessMap() const
{
	return material->GetMetalnessMap();
}

std::shared_ptr<Image> MeshRenderer::GetAmbientOcclusionMap() const
{
	return material->GetAOMap();
}

std::shared_ptr<Image> MeshRenderer::GetEmissiveMap() const
{
	return material->GetEmissiveMap();
}

float MeshRenderer::GetRoughnessValue() const
{
    return material->GetRoughnessValue();
}

float MeshRenderer::GetMetalnessValue() const
{
	return material->GetMetalnessValue();
}

float MeshRenderer::GetAmbientOcclusionValue() const
{
	return material->GetAmbientOcclusionValue();
}

glm::vec3 MeshRenderer::GetEmissiveColor() const
{
	return material->GetEmissiveColor();
}

float MeshRenderer::GetEmissiveIntensity() const
{
    return material->GetEmissiveIntensity();
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
    material->SetAlbedoMap(image);
}

//std::shared_ptr<Image> MeshRenderer::GetImage() const
//{
//    return image;
//}

MonoObject* MeshRenderer::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    // 1. Obtener la clase de C#
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "MeshRenderer");
    if (!klass) {
        // Manejar el error si la clase no se encuentra
        return nullptr;
    }

    // 2. Crear una instancia del objeto de C#
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        // Manejar el error si la instancia no se puede crear
        return nullptr;
    }

    // 3. Obtener el constructor correcto
    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.MeshRenderer:.ctor(uintptr,HawkEngine.GameObject)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    if (!method)
    {
        //Manejar error si el constructor no se encuentra
        return nullptr;
    }

    // 4. Preparar los argumentos para el constructor
    uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this); 
    MonoObject* ownerGo = owner->GetSharp(); // Obtenemos la instancia del propietario de C#
    if (!ownerGo)
    {
        //Manejar error si el propietario no tiene instancia de C#
        return nullptr;
    }

    void* args[2];
    args[0] = &componentPtr;
    args[1] = ownerGo;

    // 5. Invocar al constructor
    mono_runtime_invoke(method, monoObject, args, NULL);

    // 6. Guardar la referencia y devolverla
    CsharpReference = monoObject;
    return CsharpReference;
}

void MeshRenderer::Render() const {
    if (!mesh || !material || !material->GetShader()) 
        return;
    
    Shaders* shader = material->GetShader();
    shader->Bind();
    
    // Apply all material uniforms to shader
    material->ApplyShaderUniforms(
        shader,
        owner->GetTransform()->GetMatrix(),
        Application->camera->view(),
        Application->camera->projection(),
        Application->camera->GetTransform().GetPosition()
    );
    
    // Configure lights if using PBR shader
    if (material->GetShaderType() == PBR) {
        auto& lights = Application->root->GetActiveScene()->_lights;
        material->ConfigureLighting(shader, lights, Application->camera->GetTransform().GetPosition());
    }
    
    // Bind material textures
    material->Bind();
    
    // Render the mesh
    glBindVertexArray(mesh->model.get()->GetModelData().vA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);
    glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);
    
    // Cleanup
    material->Unbind();
    shader->UnBind();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Render bounding box if enabled
    mesh->drawBoundingBox(mesh->_boundingBox);
}

void MeshRenderer::RenderMainCamera() const {
    if (!mesh || !material || !material->GetShader())
        return;

    Shaders* shader = material->GetShader();
    shader->Bind();

    material->ApplyShaderUniforms(
        shader,
        owner->GetTransform()->GetMatrix(),
        Application->root->mainCamera->GetComponent<CameraComponent>()->view(),
        Application->root->mainCamera->GetComponent<CameraComponent>()->projection(),
        Application->root->mainCamera->GetTransform()->GetPosition()
    );

    if (material->GetShaderType() == PBR) {
        auto& lights = Application->root->GetActiveScene()->_lights;
        material->ConfigureLighting(shader, lights, Application->root->mainCamera->GetTransform()->GetPosition());
    }

    material->Bind();

    glBindVertexArray(mesh->model.get()->GetModelData().vA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);
    glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

    material->Unbind();
    shader->UnBind();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //mesh->drawBoundingBox(mesh->_boundingBox);
}