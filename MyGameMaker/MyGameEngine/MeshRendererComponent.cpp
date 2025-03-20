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
	material = std::make_shared<Material>();

	auto image = std::make_shared<Image>();
	image->LoadTexture("default.png");
	material->setImage(image);
	material->SetColor(glm::dvec4(0.7f, 0.7f, 0.7f, 1.0f));

	if (!owner->GetComponent<ShaderComponent>()) {
		owner->AddComponent<ShaderComponent>();
	}
	owner->GetComponent<ShaderComponent>()->SetOwnerMaterial(material.get());
	owner->GetComponent<ShaderComponent>()->SetShaderType(ShaderType::PBR);
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

	CsharpReference = monoObject;
	return CsharpReference;
}

void MeshRenderer::SetupLightProperties(Shaders* shader, const glm::vec3& viewPos) const {
	if (!shader) return;

	int numPointLights = static_cast<int>(Application->root->GetActiveScene()->_lights.size());
	shader->SetUniform("numPointLights", numPointLights);

	int i = 0;
	for (const auto& light : Application->root->GetActiveScene()->_lights) {
		if (!light) continue;

		std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
		auto transformComponent = light->GetComponent<Transform_Component>();
		auto lightComponent = light->GetComponent<LightComponent>();

		if (!transformComponent || !lightComponent) continue;

		shader->SetUniform(pointLightstr + ".position", transformComponent->GetPosition());
		shader->SetUniform(pointLightstr + ".ambient", lightComponent->GetAmbient());
		shader->SetUniform(pointLightstr + ".diffuse", lightComponent->GetDiffuse());
		shader->SetUniform(pointLightstr + ".specular", lightComponent->GetSpecular());
		shader->SetUniform(pointLightstr + ".constant", lightComponent->GetConstant());
		shader->SetUniform(pointLightstr + ".linear", lightComponent->GetLinear());
		shader->SetUniform(pointLightstr + ".quadratic", lightComponent->GetQuadratic());
		shader->SetUniform(pointLightstr + ".radius", lightComponent->GetRadius());
		shader->SetUniform(pointLightstr + ".intensity", lightComponent->GetIntensity());
		i++;
	}

	glBindVertexArray(mesh->model.get()->GetModelData().vA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model.get()->GetModelData().iBID);

	shader->SetUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->SetUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader->SetUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader->SetUniform("dirLight.intensity", 3.0f);
}

void MeshRenderer::BindMeshForRendering() const {
	if (!mesh || !mesh->model) return;

	glBindVertexArray(mesh->model->GetModelData().vA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->model->GetModelData().iBID);
}

void MeshRenderer::UnbindMeshAfterRendering() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MeshRenderer::DrawMeshElements() const {
	if (!mesh || !mesh->model) return;

	glDrawElements(GL_TRIANGLES, mesh->model->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);
}

void MeshRenderer::Render() const {
	if (!mesh || !material || !owner || !mesh->model) return;

	glm::dvec4 materialColor = material->GetColor();
	std::string texturePath = material->image().image_path;

	GLboolean blendEnabled;
	glGetBooleanv(GL_BLEND, &blendEnabled);

	GLint blendSrc, blendDst;
	glGetIntegerv(GL_BLEND_SRC, &blendSrc);
	glGetIntegerv(GL_BLEND_DST, &blendDst);

	GLint currentVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);

	GLint currentElementArrayBuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentElementArrayBuffer);

	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	GLint lastActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &lastActiveTexture);

	glUseProgram(0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

#ifndef _BUILD
	material->ApplyShader(owner->GetTransform()->GetMatrix(),
		Application->camera->view(),
		Application->camera->projection());
#else
	material->ApplyShader(owner->GetTransform()->GetMatrix(),
		Application->root->mainCamera->GetComponent<CameraComponent>()->view(),
		Application->root->mainCamera->GetComponent<CameraComponent>()->projection());
#endif // !_BUILD

	if (material->GetShaderType() == ShaderType::PBR) {
		Shaders* shader = ShaderManager::GetInstance().GetShader(material->GetShaderType());
		if (shader) 
		{
#ifdef _BUILD
			shader->SetUniform("viewPos", Application->root->mainCamera->GetTransform()->GetPosition());
			SetupLightProperties(shader, Application->root->mainCamera->GetTransform()->GetPosition());
#endif 
			SetUpAnimationProperties(shader);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindMeshForRendering();
	DrawMeshElements();

	glUseProgram(0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (!blendEnabled) {
		glDisable(GL_BLEND);
	}
	else {
		glBlendFunc(blendSrc, blendDst);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentElementArrayBuffer);
	glBindVertexArray(currentVAO);

	glActiveTexture(lastActiveTexture);

	if (lastProgram > 0) {
		glUseProgram(lastProgram);
	}

	/*if (mesh->drawBoundingbox) {
		mesh->drawBoundingBox(mesh->_boundingBox);
	}*/
}

void MeshRenderer::RenderWithUnlitShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const {
	if (!shader) return;

	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);
}

void MeshRenderer::SetUpAnimationProperties(Shaders* shader) const
{
	shader->SetUniform("isAnimated", 0);
	if (owner->HasComponent<SkeletalAnimationComponent>() &&
		owner->GetComponent<SkeletalAnimationComponent>()->GetAnimator())
	{
		shader->SetUniform("isAnimated", 1);
		auto transforms = owner->GetComponent<SkeletalAnimationComponent>()->GetAnimator()->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			shader->SetUniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
}

void MeshRenderer::RenderWithPBRShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const {
	if (!shader) return;

	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);

	shader->SetUniform("viewPos", Application->camera->GetTransform().GetPosition());

	SetupLightProperties(shader, Application->camera->GetTransform().GetPosition());
}

void MeshRenderer::RenderMainCamera() const {
	if (!mesh || !material || !owner || !mesh->model) return;

	GLboolean blendEnabled;
	glGetBooleanv(GL_BLEND, &blendEnabled);

	GLint blendSrc, blendDst;
	glGetIntegerv(GL_BLEND_SRC, &blendSrc);
	glGetIntegerv(GL_BLEND_DST, &blendDst);

	GLint currentVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);

	GLint currentElementArrayBuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentElementArrayBuffer);

	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	GLint lastActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &lastActiveTexture);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	material->ApplyShader(
		owner->GetTransform()->GetMatrix(),
		Application->root->mainCamera->GetComponent<CameraComponent>()->view(),
		Application->root->mainCamera->GetComponent<CameraComponent>()->projection()
	);

	if (material->GetShaderType() == ShaderType::PBR) 
	{
		Shaders* shader = ShaderManager::GetInstance().GetShader(material->GetShaderType());
		if (shader) 
		{
			shader->SetUniform("viewPos", Application->root->mainCamera->GetTransform()->GetPosition());
			SetupLightProperties(shader, Application->root->mainCamera->GetTransform()->GetPosition());
			SetUpAnimationProperties(shader);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindMeshForRendering();
	DrawMeshElements();

	glUseProgram(0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	material->unbind();

	if (!blendEnabled) {
		glDisable(GL_BLEND);
	}
	else {
		glBlendFunc(blendSrc, blendDst);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentElementArrayBuffer);
	glBindVertexArray(currentVAO);

	glActiveTexture(lastActiveTexture);

	if (lastProgram > 0) {
		glUseProgram(lastProgram);
	}
}