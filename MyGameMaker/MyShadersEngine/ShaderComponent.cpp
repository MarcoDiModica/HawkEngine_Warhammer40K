#include "ShaderComponent.h"
#include "../MyGameEngine/GameObject.h"

ShaderComponent::ShaderComponent(GameObject* owner)
	: Component(owner),
	shaderType(UNLIT),
	shaders(std::make_unique<Shaders>()),
	ownerMaterial(nullptr)
{
	name = "ShaderComponent";
}

void ShaderComponent::Start() {
	LoadDefaultShaders();

	if (ownerMaterial) {
		ownerMaterial->SetShaderType(shaderType);
		ownerMaterial->SetShader(shaders.get());
	}
}

void ShaderComponent::Update(float deltaTime) {
}

void ShaderComponent::Destroy() {
}

std::unique_ptr<Component> ShaderComponent::Clone(GameObject* owner) {
	return std::unique_ptr<Component>();
}

void ShaderComponent::SetShaderType(ShaderType type) {
	if (shaderType != type) {
		shaderType = type;
		LoadDefaultShaders();

		if (ownerMaterial) {
			ownerMaterial->SetShaderType(shaderType);
		}
	}
}

ShaderType ShaderComponent::GetShaderType() const {
	return shaderType;
}

bool ShaderComponent::LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	bool success = shaders->LoadShaders(vertexShaderPath, fragmentShaderPath);

	if (success && ownerMaterial) {
		ownerMaterial->SetShader(shaders.get());
	}

	return success;
}

void ShaderComponent::SetOwnerMaterial(Material* material) {
	ownerMaterial = material;

	if (ownerMaterial && shaders) {
		ownerMaterial->SetShader(shaders.get());
		ownerMaterial->SetShaderType(shaderType);
	}
}

void ShaderComponent::LoadDefaultShaders() {
	switch (shaderType) {
	case UNLIT:
		shaders->LoadShaders("Shaders/unlit_vertex.glsl", "Shaders/unlit_fragment.glsl");
		break;

	case PBR:
		shaders->LoadShaders("Shaders/pbr_vertex.glsl", "Shaders/pbr_fragment.glsl");
		break;

	default:
		shaders->LoadShaders("Shaders/unlit_vertex.glsl", "Shaders/unlit_fragment.glsl");
		break;
	}
}