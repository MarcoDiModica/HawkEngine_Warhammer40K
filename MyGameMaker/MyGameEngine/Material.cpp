#include "Material.h"
#include <GL/glew.h>
#include <fstream>
#include <zlib.h>
#include <unordered_map>
#include <filesystem>
#include "../MyGameEditor/Log.h"
#include "LightComponent.h"
#include "../MyGameEditor/App.h"

Material::Material() :
	shaderType(UNLIT),
	albedoColor(1.0f),
	shader(nullptr),
	roughnessValue(0.5f),
	metalnessValue(0.0f),
	aoValue(1.0f),
	emissiveColor(0.0f),
	emissiveIntensity(0.0f),
	hasAlbedoMap(false),
	hasNormalMap(false),
	hasRoughnessMap(false),
	hasMetalnessMap(false),
	hasAOMap(false),
	hasEmissiveMap(false)
{
}

void Material::SetShader(Shaders* shader) {
	this->shader = shader;
}

Shaders* Material::GetShader() const {
	return shader;
}

void Material::SetShaderType(ShaderType type) {
	shaderType = type;
}

ShaderType Material::GetShaderType() const {
	return shaderType;
}

void Material::SetColor(const glm::vec4& color) {
	albedoColor = color;
}

const glm::vec4& Material::GetColor() const {
	return albedoColor;
}

void Material::SetAlbedoMap(const std::shared_ptr<Image>& image) {
	albedoMap = image;
	hasAlbedoMap = (image != nullptr && image->id() != 0);
}

void Material::SetNormalMap(const std::shared_ptr<Image>& image) {
	normalMap = image;
	hasNormalMap = (image != nullptr && image->id() != 0);
}

void Material::SetRoughnessMap(const std::shared_ptr<Image>& image) {
	roughnessMap = image;
	hasRoughnessMap = (image != nullptr && image->id() != 0);
}

void Material::SetMetalnessMap(const std::shared_ptr<Image>& image) {
	metalnessMap = image;
	hasMetalnessMap = (image != nullptr && image->id() != 0);
}

void Material::SetAOMap(const std::shared_ptr<Image>& image) {
	aoMap = image;
	hasAOMap = (image != nullptr && image->id() != 0);
}

void Material::SetEmissiveMap(const std::shared_ptr<Image>& image) {
	emissiveMap = image;
	hasEmissiveMap = (image != nullptr && image->id() != 0);
}

void Material::SetRoughness(float value) {
	roughnessValue = value;
}

void Material::SetMetalness(float value) {
	metalnessValue = value;
}

void Material::SetAmbientOcclusion(float value) {
	aoValue = value;
}

void Material::SetEmissiveColor(const glm::vec3& color) {
	emissiveColor = color;
}

void Material::SetEmissiveIntensity(float intensity) {
	emissiveIntensity = intensity;
}

std::shared_ptr<Image> Material::GetAlbedoMap() const
{
	return albedoMap;
}

std::shared_ptr<Image> Material::GetNormalMap() const
{
	return normalMap;
}

std::shared_ptr<Image> Material::GetRoughnessMap() const
{
	return roughnessMap;
}

std::shared_ptr<Image> Material::GetMetalnessMap() const
{
	return metalnessMap;
}

std::shared_ptr<Image> Material::GetAOMap() const
{
	return aoMap;
}

std::shared_ptr<Image> Material::GetEmissiveMap() const
{
	return emissiveMap;
}

float Material::GetRoughnessValue() const
{
	return roughnessValue;
}

float Material::GetMetalnessValue() const
{
	return metalnessValue;
}

float Material::GetAmbientOcclusionValue() const
{
	return aoValue;
}

glm::vec3 Material::GetEmissiveColor() const
{
	return emissiveColor;
}

float Material::GetEmissiveIntensity() const
{
	return emissiveIntensity;
}

void Material::Bind() {
	// Bind albedo texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	if (hasAlbedoMap) {
		glBindTexture(GL_TEXTURE_2D, albedoMap->id());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Only bind PBR textures if we're using PBR shader
	if (shaderType == PBR) {
		// Normal map - texture unit 1
		glActiveTexture(GL_TEXTURE1);
		if (hasNormalMap) {
			glBindTexture(GL_TEXTURE_2D, normalMap->id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Roughness map - texture unit 2
		glActiveTexture(GL_TEXTURE2);
		if (hasRoughnessMap) {
			glBindTexture(GL_TEXTURE_2D, roughnessMap->id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Metalness map - texture unit 3
		glActiveTexture(GL_TEXTURE3);
		if (hasMetalnessMap) {
			glBindTexture(GL_TEXTURE_2D, metalnessMap->id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Ambient Occlusion map - texture unit 4
		glActiveTexture(GL_TEXTURE4);
		if (hasAOMap) {
			glBindTexture(GL_TEXTURE_2D, aoMap->id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Emissive map - texture unit 5
		glActiveTexture(GL_TEXTURE5);
		if (hasEmissiveMap) {
			glBindTexture(GL_TEXTURE_2D, emissiveMap->id());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void Material::Unbind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (shaderType == PBR) {
		for (int i = 1; i <= 5; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}

void Material::ApplyShaderUniforms(Shaders* shader,
	const glm::mat4& model,
	const glm::mat4& view,
	const glm::mat4& projection,
	const glm::vec3& viewPos) {
	if (!shader) return;

	// Common uniforms for both shader types
	shader->SetUniform("model", model);
	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);
	shader->SetUniform("modColor", albedoColor);
	shader->SetUniform("u_HasTexture", hasAlbedoMap);
	shader->SetUniform("texture1", 0); // Albedo texture

	// Additional uniforms for PBR shader
	if (shaderType == PBR) {
		shader->SetUniform("viewPos", viewPos);

		// Material parameters
		shader->SetUniform("material.albedoMap", 0);
		shader->SetUniform("material.albedoColor", albedoColor);

		// Normal mapping
		shader->SetUniform("material.useNormalMap", hasNormalMap);
		if (hasNormalMap) {
			shader->SetUniform("material.normalMap", 1);
		}

		// Roughness
		shader->SetUniform("material.useRoughnessMap", hasRoughnessMap);
		if (hasRoughnessMap) {
			shader->SetUniform("material.roughnessMap", 2);
		}
		else {
			shader->SetUniform("material.roughnessValue", roughnessValue);
		}

		// Metalness
		shader->SetUniform("material.useMetalnessMap", hasMetalnessMap);
		if (hasMetalnessMap) {
			shader->SetUniform("material.metalnessMap", 3);
		}
		else {
			shader->SetUniform("material.metalnessValue", metalnessValue);
		}

		// Ambient Occlusion
		shader->SetUniform("material.useAOMap", hasAOMap);
		if (hasAOMap) {
			shader->SetUniform("material.aoMap", 4);
		}
		else {
			shader->SetUniform("material.aoValue", aoValue);
		}

		// Emissive
		shader->SetUniform("material.useEmissiveMap", hasEmissiveMap);
		if (hasEmissiveMap) {
			shader->SetUniform("material.emissiveMap", 5);
		}
		else {
			shader->SetUniform("material.emissiveColor", emissiveColor);
		}
		shader->SetUniform("material.emissiveIntensity", emissiveIntensity);
	}
}

void Material::ConfigureLighting(Shaders* shader,
	const std::vector<std::shared_ptr<GameObject>>& lights,
	const glm::vec3& viewPos) {
	if (!shader || shaderType != PBR) return;

	int numLights = lights.size();
	shader->SetUniform("numPointLights", numLights);

	for (int i = 0; i < numLights; i++) {
		auto light = lights[i];
		auto lightComp = light->GetComponent<LightComponent>();
		auto transform = light->GetComponent<Transform_Component>();

		if (!lightComp || !transform) continue;

		std::string prefix = "pointLights[" + std::to_string(i) + "]";

		shader->SetUniform(prefix + ".position", transform->GetPosition());
		shader->SetUniform(prefix + ".ambient", lightComp->GetAmbient());
		shader->SetUniform(prefix + ".diffuse", lightComp->GetDiffuse());
		shader->SetUniform(prefix + ".specular", lightComp->GetSpecular());
		shader->SetUniform(prefix + ".constant", lightComp->GetConstant());
		shader->SetUniform(prefix + ".linear", lightComp->GetLinear());
		shader->SetUniform(prefix + ".quadratic", lightComp->GetQuadratic());
		shader->SetUniform(prefix + ".radius", lightComp->GetRadius());
		shader->SetUniform(prefix + ".intensity", lightComp->GetIntensity());
	}

	shader->SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	shader->SetUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->SetUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader->SetUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->SetUniform("dirLight.intensity", 3.0f);
}