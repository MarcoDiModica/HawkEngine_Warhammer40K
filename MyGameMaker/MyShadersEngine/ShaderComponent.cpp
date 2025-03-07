#include "ShaderComponent.h"
#include "../MyGameEngine/ShaderManager.h"

ShaderComponent::ShaderComponent(GameObject* owner)
	: Component(owner), shaderType(ShaderType::UNLIT), ownerMaterial(nullptr) {
	name = "Shader";
}

void ShaderComponent::Start() {
	// Initialize shader component
}

void ShaderComponent::Update(float deltaTime) {
	// Update shader parameters if needed
}

void ShaderComponent::Destroy() {
	ownerMaterial = nullptr;
}

std::unique_ptr<Component> ShaderComponent::Clone(GameObject* owner) {
	auto component = std::make_unique<ShaderComponent>(owner);
	component->shaderType = this->shaderType;
	component->frequency = this->frequency;
	component->amplitude = this->amplitude;
	// Material connection must be set separately
	return component;
}

void ShaderComponent::SetShaderType(ShaderType type) {
	shaderType = type;

	if (ownerMaterial) {
		ownerMaterial->SetShaderType(type);
	}
}

ShaderType ShaderComponent::GetShaderType() const {
	return shaderType;
}

void ShaderComponent::SetOwnerMaterial(Material* material) {
	ownerMaterial = material;
	if (ownerMaterial) {
		ownerMaterial->SetShaderType(shaderType);
	}
}

Material* ShaderComponent::GetOwnerMaterial() const {
	return ownerMaterial;
}

void ShaderComponent::SetFloat(const std::string& name, float value) {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (shader) {
		shader->Bind();
		shader->SetUniform(name, value);
		shader->UnBind();
	}
}

void ShaderComponent::SetVec3(const std::string& name, const glm::vec3& value) {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (shader) {
		shader->Bind();
		shader->SetUniform(name, value);
		shader->UnBind();
	}
}

void ShaderComponent::SetVec4(const std::string& name, const glm::vec4& value) {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (shader) {
		shader->Bind();
		shader->SetUniform(name, value);
		shader->UnBind();
	}
}

void ShaderComponent::SetMatrix(const std::string& name, const glm::mat4& value) {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (shader) {
		shader->Bind();
		shader->SetUniform(name, value);
		shader->UnBind();
	}
}