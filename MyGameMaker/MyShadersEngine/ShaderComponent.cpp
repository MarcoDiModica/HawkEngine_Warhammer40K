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

void ShaderComponent::SetMetallic(float value) {
	if (ownerMaterial) {
		ownerMaterial->metallic = value;
	}
}

void ShaderComponent::SetRoughness(float value) {
	if (ownerMaterial) {
		ownerMaterial->roughness = value;
	}
}

void ShaderComponent::SetAO(float value) {
	if (ownerMaterial) {
		ownerMaterial->ao = value;
	}
}

void ShaderComponent::SetNormalMap(const std::string& path) {
	if (!ownerMaterial) return;

	auto normalMap = std::make_shared<Image>();
	if (normalMap->LoadTexture(path)) {
		ownerMaterial->setNormalMap(normalMap);
	}
}

void ShaderComponent::SetMetallicMap(const std::string& path) {
	if (!ownerMaterial) return;

	auto metallicMap = std::make_shared<Image>();
	if (metallicMap->LoadTexture(path)) {
		ownerMaterial->setMetallicMap(metallicMap);
	}
}

void ShaderComponent::SetRoughnessMap(const std::string& path) {
	if (!ownerMaterial) return;

	auto roughnessMap = std::make_shared<Image>();
	if (roughnessMap->LoadTexture(path)) {
		ownerMaterial->setRoughnessMap(roughnessMap);
	}
}

void ShaderComponent::SetAOMap(const std::string& path) {
	if (!ownerMaterial) return;

	auto aoMap = std::make_shared<Image>();
	if (aoMap->LoadTexture(path)) {
		ownerMaterial->setAoMap(aoMap);
	}
}

float ShaderComponent::GetMetallic() const {
	return ownerMaterial ? ownerMaterial->metallic : 0.0f;
}

float ShaderComponent::GetRoughness() const {
	return ownerMaterial ? ownerMaterial->roughness : 0.5f;
}

float ShaderComponent::GetAO() const {
	return ownerMaterial ? ownerMaterial->ao : 1.0f;
}

bool ShaderComponent::HasNormalMap() const {
	return ownerMaterial && ownerMaterial->normalMapPtr && ownerMaterial->normalMapPtr->id() != 0;
}

bool ShaderComponent::HasMetallicMap() const {
	return ownerMaterial && ownerMaterial->metallicMapPtr && ownerMaterial->metallicMapPtr->id() != 0;
}

bool ShaderComponent::HasRoughnessMap() const {
	return ownerMaterial && ownerMaterial->roughnessMapPtr && ownerMaterial->roughnessMapPtr->id() != 0;
}

bool ShaderComponent::HasAOMap() const {
	return ownerMaterial && ownerMaterial->aoMapPtr && ownerMaterial->aoMapPtr->id() != 0;
}
