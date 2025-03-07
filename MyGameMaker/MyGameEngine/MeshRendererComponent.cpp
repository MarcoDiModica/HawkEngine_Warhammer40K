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
#include "ShaderManager.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/EditorCamera.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(owner) {
	name = "MeshRenderer";
	mesh = Mesh::CreateCube();
	auto image = std::make_shared<Image>();
	image->LoadTexture("default.png");
	material->setImage(image);
	material->SetColor(glm::dvec4(0.7f, 0.7f, 0.7f, 1.0f));

	owner->AddComponent<ShaderComponent>();
	owner->GetComponent<ShaderComponent>()->SetOwnerMaterial(material.get());
	owner->GetComponent<ShaderComponent>()->SetShaderType(ShaderType::UNLIT);
}

void MeshRenderer::Start() {
}

void MeshRenderer::Update(float deltaTime) {
}

void MeshRenderer::Destroy() {
	mesh.reset();
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
	this->material = material;

	auto shaderComponent = owner->GetComponent<ShaderComponent>();
	if (shaderComponent) {
		shaderComponent->SetOwnerMaterial(material.get());
	}
}

std::shared_ptr<Material> MeshRenderer::GetMaterial() const {
	return material;
}

void MeshRenderer::SetImage(std::shared_ptr<Image> image) {
	material->setImage(image);
}

MonoObject* MeshRenderer::GetSharp() {
	if (CsharpReference) {
		return CsharpReference;
	}

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
		return nullptr;
	}

	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner->GetSharp();
	if (!ownerGo) {
		return nullptr;
	}

	void* args[2];
	args[0] = &componentPtr;
	args[1] = ownerGo;

	mono_runtime_invoke(method, monoObject, args, NULL);

	CsharpReference = monoObject;
	return CsharpReference;
}

void MeshRenderer::Render() const {
	Shaders* shader = ShaderManager::GetInstance().GetShader(material->GetShaderType());
	if (!shader) {
		std::cerr << "Shader not found for type " << static_cast<int>(material->GetShaderType()) << std::endl;
		return;
	}

	shader->Bind();

	if (!material->image().image_path.empty()) {
		material->bind();
		shader->SetUniform("u_HasTexture", 1);
		shader->SetUniform("texture1", 0);
	}
	else {
		shader->SetUniform("u_HasTexture", 0);
	}

	shader->SetUniform("modColor", glm::vec4(material->GetColor()));
	shader->SetUniform("model", owner->GetTransform()->GetMatrix());

	glm::mat4 viewMatrix = Application->camera->view();
	glm::mat4 projectionMatrix = Application->camera->projection();

	switch (material->GetShaderType()) {
	case ShaderType::UNLIT:
		RenderWithUnlitShader(shader, viewMatrix, projectionMatrix);
		break;
	case ShaderType::PBR:
		RenderWithPBRShader(shader, viewMatrix, projectionMatrix);
		break;
	}

	glBindVertexArray(mesh->model.get()->GetModelData().vA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);
	glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

	shader->UnBind();
	if (!material->image().image_path.empty()) {
		material->unbind();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mesh->drawBoundingBox(mesh->_boundingBox);
}

void MeshRenderer::RenderWithUnlitShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const {
	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);
}

void MeshRenderer::RenderWithPBRShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const {
	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);

	shader->SetUniform("viewPos", Application->camera->GetTransform().GetPosition());

	int numPointLights = static_cast<int>(Application->root->GetActiveScene()->_lights.size());
	shader->SetUniform("numPointLights", numPointLights);

	int i = 0;
	for (const auto& light : Application->root->GetActiveScene()->_lights) {
		std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
		shader->SetUniform(pointLightstr + ".position", light->GetComponent<Transform_Component>()->GetPosition());
		shader->SetUniform(pointLightstr + ".ambient", light->GetComponent<LightComponent>()->GetAmbient());
		shader->SetUniform(pointLightstr + ".diffuse", light->GetComponent<LightComponent>()->GetDiffuse());
		shader->SetUniform(pointLightstr + ".specular", light->GetComponent<LightComponent>()->GetSpecular());
		shader->SetUniform(pointLightstr + ".constant", light->GetComponent<LightComponent>()->GetConstant());
		shader->SetUniform(pointLightstr + ".linear", light->GetComponent<LightComponent>()->GetLinear());
		shader->SetUniform(pointLightstr + ".quadratic", light->GetComponent<LightComponent>()->GetQuadratic());
		shader->SetUniform(pointLightstr + ".radius", light->GetComponent<LightComponent>()->GetRadius());
		shader->SetUniform(pointLightstr + ".intensity", light->GetComponent<LightComponent>()->GetIntensity());
		i++;
	}

	shader->SetUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->SetUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader->SetUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader->SetUniform("dirLight.intensity", 3.0f);
}

void MeshRenderer::RenderMainCamera() const {
	Shaders* shader = ShaderManager::GetInstance().GetShader(material->GetShaderType());
	if (!shader) {
		std::cerr << "Shader not found for type " << static_cast<int>(material->GetShaderType()) << std::endl;
		return;
	}

	shader->Bind();

	if (!material->image().image_path.empty()) {
		material->bind();
		shader->SetUniform("u_HasTexture", 1);
		shader->SetUniform("texture1", 0);
	}
	else {
		shader->SetUniform("u_HasTexture", 0);
	}

	shader->SetUniform("modColor", glm::vec4(material->GetColor()));
	shader->SetUniform("model", owner->GetTransform()->GetMatrix());

	glm::mat4 viewMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->view();
	glm::mat4 projectionMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->projection();

	switch (material->GetShaderType()) {
	case ShaderType::UNLIT:
		shader->SetUniform("view", viewMatrix);
		shader->SetUniform("projection", projectionMatrix);
		break;

	case ShaderType::PBR:
		shader->SetUniform("view", viewMatrix);
		shader->SetUniform("projection", projectionMatrix);

		shader->SetUniform("viewPos", Application->root->mainCamera->GetTransform()->GetPosition());

		int numPointLights = static_cast<int>(Application->root->GetActiveScene()->_lights.size());
		shader->SetUniform("numPointLights", numPointLights);

		int i = 0;
		for (const auto& light : Application->root->GetActiveScene()->_lights) {
			std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
			shader->SetUniform(pointLightstr + ".position", light->GetComponent<Transform_Component>()->GetPosition());
			shader->SetUniform(pointLightstr + ".ambient", light->GetComponent<LightComponent>()->GetAmbient());
			shader->SetUniform(pointLightstr + ".diffuse", light->GetComponent<LightComponent>()->GetDiffuse());
			shader->SetUniform(pointLightstr + ".specular", light->GetComponent<LightComponent>()->GetSpecular());
			shader->SetUniform(pointLightstr + ".constant", light->GetComponent<LightComponent>()->GetConstant());
			shader->SetUniform(pointLightstr + ".linear", light->GetComponent<LightComponent>()->GetLinear());
			shader->SetUniform(pointLightstr + ".quadratic", light->GetComponent<LightComponent>()->GetQuadratic());
			shader->SetUniform(pointLightstr + ".radius", light->GetComponent<LightComponent>()->GetRadius());
			shader->SetUniform(pointLightstr + ".intensity", light->GetComponent<LightComponent>()->GetIntensity());
			i++;
		}

		shader->SetUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader->SetUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader->SetUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		shader->SetUniform("dirLight.intensity", 3.0f);
		break;
	}

	glBindVertexArray(mesh->model.get()->GetModelData().vA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);
	glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

	shader->UnBind();
	if (!material->image().image_path.empty()) {
		material->unbind();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}