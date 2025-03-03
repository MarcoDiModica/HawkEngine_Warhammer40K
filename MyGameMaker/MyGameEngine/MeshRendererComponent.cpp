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
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "LightComponent.h"
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
    material->shader->Bind();

    glm::vec4 color(material->GetColor().x, material->GetColor().y, material->GetColor().z, material->GetColor().w);
    material->shader->SetUniform("modColor", color);

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

    int numPointLights = static_cast<int>(Application->root->GetActiveScene()->_lights.size());
    int i = 0;

    switch (material->shaderType)
    {
    case LIGHT:

        material->setShaderUniform("viewPos", Application->camera->GetTransform().GetPosition());
        
        material->setShaderUniform("numPointLights", numPointLights);
        
        
        for (const auto& light : Application->root->GetActiveScene()->_lights)
        {
            std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
            material->setShaderUniform(pointLightstr + ".position", light->GetComponent<Transform_Component>()->GetPosition());
            material->setShaderUniform(pointLightstr + ".ambient", light->GetComponent<LightComponent>()->GetAmbient());
            material->setShaderUniform(pointLightstr + ".diffuse", light->GetComponent<LightComponent>()->GetDiffuse());
            material->setShaderUniform(pointLightstr + ".specular", light->GetComponent<LightComponent>()->GetSpecular());
            material->setShaderUniform(pointLightstr + ".constant", light->GetComponent<LightComponent>()->GetConstant());
            material->setShaderUniform(pointLightstr + ".linear", light->GetComponent<LightComponent>()->GetLinear());
            material->setShaderUniform(pointLightstr + ".quadratic", light->GetComponent<LightComponent>()->GetQuadratic());
            material->setShaderUniform(pointLightstr + ".radius", light->GetComponent<LightComponent>()->GetRadius());
            material->setShaderUniform(pointLightstr + ".intensity", light->GetComponent<LightComponent>()->GetIntensity());
            i++;
        }

        material->setShaderUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        material->setShaderUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        material->setShaderUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        material->setShaderUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        material->setShaderUniform("dirLight.intensity", 3.0f);

        break;
    case WATER:

        material->shader->SetUniform("u_Time", owner->GetTimeActive());
        material->shader->SetUniform("u_Amplitude", owner->GetComponent<ShaderComponent>()->amplitude);
        material->shader->SetUniform("u_Frequency", owner->GetComponent<ShaderComponent>()->frequency);
        material->shader->SetUniform("u_ColorLow", glm::vec3(0.0f, 0.0f, 1.0f));
        material->shader->SetUniform("u_ColorHigh", glm::vec3(1.0f, 1.0f, 1.0f));
        material->shader->SetUniform("u_Factor", 0.8f);

        break;
    case DEFAULT:
        break;
    default:
        break;
    }

    if (owner->HasComponent<SkeletalAnimationComponent>() &&
        owner->GetComponent<SkeletalAnimationComponent>()->GetAnimator()) {
        auto transforms = owner->GetComponent<SkeletalAnimationComponent>()->GetAnimator()->GetBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            material->setShaderUniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }


    glBindVertexArray(mesh->model.get()->GetModelData().vA);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);

    // Esto dibuja las caras!!
    glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

    material->shader->UnBind();

    if (material->image().image_path != "") {
        material->unbind();
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    mesh->drawBoundingBox(mesh->_boundingBox);
}