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
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "LightComponent.h"
#include "ShaderManager.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Root.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner) {
	name = "MeshRenderer";
	mesh = Application->root->GetResourceManager()->Cube;
	material = Application->root->GetResourceManager()->DefaultMaterial;

	if (!owner->GetComponent<ShaderComponent>()) {
		owner->AddComponent<ShaderComponent>();
	}
	owner->GetComponent<ShaderComponent>()->SetOwnerMaterial(material.get());
	owner->GetComponent<ShaderComponent>()->SetShaderType(ShaderType::PBR);
	
}

void MeshRenderer::Awake() {
}

void MeshRenderer::Start() {
}

void MeshRenderer::Update(float deltaTime) {
}

void MeshRenderer::Destroy() {
	mesh.reset();
	material.reset();
}

std::unique_ptr<Component> MeshRenderer::Clone(GameObject* owner) {
	auto meshRenderer = std::make_unique<MeshRenderer>(*this);
	meshRenderer->mesh = mesh;
	meshRenderer->material = material;
	meshRenderer->color = color;
	meshRenderer->owner = owner;
	return meshRenderer;
}

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh) {
	this->mesh = mesh;
}

std::shared_ptr<Mesh> MeshRenderer::GetMesh() const {
	return mesh;
}

void MeshRenderer::SetColor(const glm::vec3& color) {
	this->color = color;
}

glm::vec3 MeshRenderer::GetColor() const {
	return color;
}

void MeshRenderer::SetMaterial(std::shared_ptr<Material> material) {
	if (this->material) {
		auto shaderComponent = owner->GetComponent<ShaderComponent>();
		if (shaderComponent) {
			shaderComponent->SetOwnerMaterial(nullptr);
		}
	}

	this->material = material;

	auto shaderComponent = owner->GetComponent<ShaderComponent>();
	if (shaderComponent) {
		shaderComponent->SetOwnerMaterial(material.get());
	}

	if (material) {
		material->SetColor(material->GetColor());
	}
}

std::shared_ptr<Material> MeshRenderer::GetMaterial() const {
	return material;
}

void MeshRenderer::SetImage(std::shared_ptr<Image> image) {
	if (material) {
		material->setImage(image);
	}
}

MonoObject* MeshRenderer::GetSharp() {
	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "MeshRenderer");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.MeshRenderer:.ctor(uintptr,HawkEngine.GameObject)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	if (!method) {
		mono_method_desc_free(constructorDesc);
		return nullptr;
	}

	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner->GetSharp();
	if (!ownerGo) {
		mono_method_desc_free(constructorDesc);
		return nullptr;
	}

	void* args[2];
	args[0] = &componentPtr;
	args[1] = ownerGo;

	mono_runtime_invoke(method, monoObject, args, nullptr);
	mono_method_desc_free(constructorDesc);

	return monoObject;
}

void MeshRenderer::SetupLightProperties(Shaders* shader, const glm::vec3& viewPos) const
{
	if (!shader) return;

	int numPointLights = static_cast<int>(Application->root->GetActiveScene()->_lights.size());
	shader->SetUniform("numPointLights", numPointLights);
	bool hasDirLight = false;

	int i = 0;
	for (const auto& light : Application->root->GetActiveScene()->_lights) {
		if (!light) continue;
		if (!light->GetComponent<Transform_Component>() || !light->GetComponent<LightComponent>()) continue;
		if (light->GetComponent<LightComponent>()->GetLightType() == LightType::POINT)
		{
			std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
			auto transformComponent = light->GetComponent<Transform_Component>();
			auto lightComponent = light->GetComponent<LightComponent>();

			if (!transformComponent || !lightComponent) continue;

			if (!transformComponent || !lightComponent) continue;

			shader->SetUniformVec3(pointLightstr + ".position", transformComponent->GetPosition());
			shader->SetUniformVec3(pointLightstr + ".ambient", lightComponent->GetAmbient());
			shader->SetUniformVec3(pointLightstr + ".diffuse", lightComponent->GetDiffuse());
			shader->SetUniformVec3(pointLightstr + ".specular", lightComponent->GetSpecular());
			shader->SetUniform(pointLightstr + ".constant", lightComponent->GetConstant());
			shader->SetUniform(pointLightstr + ".linear", lightComponent->GetLinear());
			shader->SetUniform(pointLightstr + ".quadratic", lightComponent->GetQuadratic());
			shader->SetUniform(pointLightstr + ".radius", lightComponent->GetRadius());
			shader->SetUniform(pointLightstr + ".intensity", lightComponent->GetIntensity());
			i++;
		}
		if (hasDirLight == false)
		{
			shader->SetUniformVec3("dirLight.ambient", vec3(0, 0, 0));
			shader->SetUniformVec3("dirLight.diffuse", vec3(0, 0, 0));
			shader->SetUniformVec3("dirLight.specular", vec3(0, 0, 0));
			shader->SetUniformVec3("dirLight.direction", vec3(0, 0, 0));
			shader->SetUniform("dirLight.intensity", 0);
		}

		glBindVertexArray(mesh->model->GetModelData().vA);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model->GetModelData().iBID);

		shader->SetUniformVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->SetUniformVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader->SetUniformVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->SetUniformVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		shader->SetUniform("dirLight.intensity", 0.5f);
	}
}

