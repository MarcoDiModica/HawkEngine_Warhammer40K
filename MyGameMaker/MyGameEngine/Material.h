#pragma once

#include <memory>
#include "Image.h"
#include "Shaders.h"
#include "types.h"

class GameObject;

class Material {
public:
	Material();
	~Material() = default;

	void SetColor(const glm::vec4& color);
	const glm::vec4& GetColor() const;

	void SetAlbedoMap(const std::shared_ptr<Image>& image);
	void SetNormalMap(const std::shared_ptr<Image>& image);
	void SetRoughnessMap(const std::shared_ptr<Image>& image);
	void SetMetalnessMap(const std::shared_ptr<Image>& image);
	void SetAOMap(const std::shared_ptr<Image>& image);
	void SetEmissiveMap(const std::shared_ptr<Image>& image);

	void SetRoughness(float value);
	void SetMetalness(float value);
	void SetAmbientOcclusion(float value);
	void SetEmissiveColor(const glm::vec3& color);
	void SetEmissiveIntensity(float intensity);

	std::shared_ptr<Image> GetAlbedoMap() const;
	std::shared_ptr<Image> GetNormalMap() const;
	std::shared_ptr<Image> GetRoughnessMap() const;
	std::shared_ptr<Image> GetMetalnessMap() const;
	std::shared_ptr<Image> GetAOMap() const;
	std::shared_ptr<Image> GetEmissiveMap() const;

	float GetRoughnessValue() const;
	float GetMetalnessValue() const;
	float GetAmbientOcclusionValue() const;
	glm::vec3 GetEmissiveColor() const;
	float GetEmissiveIntensity() const;

	void SetShader(Shaders* shader);
	Shaders* GetShader() const;
	void SetShaderType(ShaderType type);
	ShaderType GetShaderType() const;

	void Bind();
	void Unbind();
	void ApplyShaderUniforms(Shaders* shader,
		const glm::mat4& model,
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& viewPos);

	void ConfigureLighting(Shaders* shader,
		const std::vector<std::shared_ptr<GameObject>>& lights,
		const glm::vec3& viewPos);

private:
	ShaderType shaderType;
	glm::vec4 albedoColor;
	Shaders* shader;

	std::shared_ptr<Image> albedoMap;
	std::shared_ptr<Image> normalMap;
	std::shared_ptr<Image> roughnessMap;
	std::shared_ptr<Image> metalnessMap;
	std::shared_ptr<Image> aoMap;
	std::shared_ptr<Image> emissiveMap;

	float roughnessValue;
	float metalnessValue;
	float aoValue;
	glm::vec3 emissiveColor;
	float emissiveIntensity;

	bool hasAlbedoMap;
	bool hasNormalMap;
	bool hasRoughnessMap;
	bool hasMetalnessMap;
	bool hasAOMap;
	bool hasEmissiveMap;
};