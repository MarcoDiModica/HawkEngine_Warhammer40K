#pragma once

#include <memory>
#include <vector>
#include "Image.h"
#include "Shaders.h"
#include "types.h"

class Material
{
public:
	Material();
	virtual ~Material() = default;

	enum WrapModes { Repeat, MirroredRepeat, Clamp };
	WrapModes wrapMode = Repeat;

	enum Filters { Nearest, Linear };
	Filters filter = Nearest;

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	float metallic = 0.0f;
	float roughness = 0.5f;
	float ao = 1.0f;

	float tonemapStrength = 1.0f;

	ShaderType shaderType = ShaderType::PBR;

	std::shared_ptr<Image> imagePtr = std::make_shared<Image>();  // Main texture (albedo/diffuse)
	std::shared_ptr<Image> normalMapPtr = nullptr;                // Normal map
	std::shared_ptr<Image> metallicMapPtr = nullptr;              // Metallic map
	std::shared_ptr<Image> roughnessMapPtr = nullptr;             // Roughness map
	std::shared_ptr<Image> aoMapPtr = nullptr;                    // Ambient occlusion map

	unsigned int GetId() const { return gid; }
	unsigned int id() const { return imagePtr ? imagePtr->id() : 0; }

	virtual void bind() const;
	virtual void unbind() const;

	void setImage(const std::shared_ptr<Image>& img_ptr) { imagePtr = img_ptr; }
	void setNormalMap(const std::shared_ptr<Image>& img_ptr) { normalMapPtr = img_ptr; }
	void setMetallicMap(const std::shared_ptr<Image>& img_ptr) { metallicMapPtr = img_ptr; }
	void setRoughnessMap(const std::shared_ptr<Image>& img_ptr) { roughnessMapPtr = img_ptr; }
	void setAoMap(const std::shared_ptr<Image>& img_ptr) { aoMapPtr = img_ptr; }

	auto& image() { return *imagePtr; }
	std::shared_ptr<Image> getImg() { return imagePtr; }

	const glm::vec4& GetColor() const;
	void SetColor(const vec4& color);

	void SetShaderType(ShaderType type) { shaderType = type; }
	ShaderType GetShaderType() const { return shaderType; }

	virtual void ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

	void SaveBinary(const std::string& filename) const;
	static std::shared_ptr<Material> LoadBinary(const std::string& filename);

	void SetTonemapStrength(float strength) { tonemapStrength = strength; }
	float GetTonemapStrength() const { return tonemapStrength; }

protected:
	unsigned int gid;
	static unsigned int next_id;
	std::string image_path;

	void bindTexture(const std::shared_ptr<Image>& texture, GLenum textureUnit) const;
};