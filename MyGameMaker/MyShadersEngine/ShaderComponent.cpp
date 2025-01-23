#include "ShaderComponent.h"

#include "MyGameEditor/App.h"
#include "MyGameEditor/Root.h"

ShaderComponent::ShaderComponent(GameObject* owner) : Component(owner) { name = "MeshRenderer"; }

void ShaderComponent::Start()
{
}

void ShaderComponent::Update(float deltaTime)
{
}

void ShaderComponent::Destroy()
{
}

std::unique_ptr<Component> ShaderComponent::Clone(GameObject* owner)
{
	return std::unique_ptr<Component>();
}

void ShaderComponent::SetShader(Shaders* newShader)
{
	shader = newShader;
}

Shaders* ShaderComponent::GetShader() const
{
	return shader;
}

void ShaderComponent::SetShaderType(ShaderType newType)
{
	shader = &Application->root->shaders[newType];
	ownerMaterial->shader = *shader;
	ownerMaterial->shaderType = newType;
	ownerMaterial->useShader = true;
	type = newType;
}

ShaderType ShaderComponent::GetShaderType() const
{
	return type;
}

void ShaderComponent::SetOwnerMaterial(Material* newOwnerMaterial)
{
	ownerMaterial = newOwnerMaterial;
}

Material* ShaderComponent::GetOwnerMaterial() const
{
	return ownerMaterial;
}
