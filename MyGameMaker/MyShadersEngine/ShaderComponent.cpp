#include "ShaderComponent.h"


ShaderComponent::ShaderComponent()
{
}

ShaderComponent::~ShaderComponent()
{
}

void ShaderComponent::SetShader(Shaders* newShader)
{
	shader = newShader;
}

Shaders* ShaderComponent::GetShader() const
{
	return shader;
}

void ShaderComponent::SetType(ShaderType newType)
{
	//shader = Application->root->shaders[newType];
	type = newType;
}

ShaderType ShaderComponent::GetType() const
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
