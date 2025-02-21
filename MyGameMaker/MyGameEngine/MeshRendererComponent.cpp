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
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

//cosailegal
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/EditorCamera.h"

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
   // this->image = image;
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

void MeshRenderer::Render() const
{
    glBindVertexArray(mesh->model.get()->GetModelData().vA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);
   
    material->shader->Bind();

    if (material->image().image_path != "") {
        material->image().bind();
        material->shader->SetUniform("u_HasTexture", true);
        material->shader->SetUniform("texture1", 0);
    }
    else {
        material->shader->SetUniform("u_HasTexture", false);
    }

    material->shader->SetUniform("model", owner->GetTransform()->GetMatrix());
    material->shader->SetUniform("view", Application->camera->view());
    material->shader->SetUniform("projection", Application->camera->projection());

    glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

    material->shader->UnBind();

    if (material->image().image_path != "") {
        material->unbind();
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*switch (material->shaderType)
    {
    case DEFAULT:
        material->setShaderUniform("model", owner->GetTransform()->GetMatrix());

        break;
    case LIGHT:
        break;
    case WATER:
        break;
    default:
        break;
    }*/

    //material->setShaderUniform("aPos", glm::vec3(0, 0, 0));
    //material->setShaderUniform("model", owner->GetTransform()->GetMatrix());
    //material->setShaderUniform("modColor", glm::vec4( 1,0.2f,0,1));
    //glUniform4f(glGetUniformLocation(material->shader.GetProgram(), "modColor"), material->GetColor().x, material->GetColor().y, material->GetColor().z, material->GetColor().w);
    ////material->setShaderUniform("model", owner->GetComponent<Transform_Component>()->GetMatrix());
    //material->setShaderUniform("view", Application->camera->view());
    //material->setShaderUniform("projection", Application->camera->projection());

    //material->setShaderUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    //material->setShaderUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    //material->setShaderUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    //material->setShaderUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    //material->setShaderUniform("dirLight.intensity", 3.0f);

    //material->setShaderUniform("viewPos", Application->camera->GetTransform().GetPosition());

    //material->setShaderUniform("pointLights.position",   glm::vec3(0.0f, 0.0f, 0.0f));
    //material->setShaderUniform("pointLights.ambient",    glm::vec3(0.2f, 0.8f, 0.2f));
    //material->setShaderUniform("pointLights.diffuse",    glm::vec3(0.5f, 0.5f, 0.5f));
    //material->setShaderUniform("pointLights.specular",   glm::vec3(1.0f, 1.0f, 1.0f));
    //material->setShaderUniform("pointLights.constant",   1.0f);
    //material->setShaderUniform("pointLights.linear",     0.09f);
    //material->setShaderUniform("pointLights.quadratic",  0.032f);
    //material->setShaderUniform("pointLights.radius",     1.0f);
    //material->setShaderUniform("pointLights.intensity",  2.5f);    
}