#pragma once

#include <memory>
#include "Image.h"
#include "Shaders.h"
#include "types.h"

class Material
{
public:
	Material();

	enum WrapModes { Repeat, MirroredRepeat, Clamp };
	WrapModes wrapMode = Repeat;

	enum Filters { Nearest, Linear };
	Filters filter = Nearest;

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	ShaderType shaderType = ShaderType::UNLIT;

	unsigned int GetId() const { return gid; }
	unsigned int id() const { return imagePtr ? imagePtr->id() : 0; }

	void bind() const;
	void unbind() const;

	void setImage(const std::shared_ptr<Image>& img_ptr) { imagePtr = img_ptr; }
	auto& image() { return *imagePtr; }
	std::shared_ptr<Image> getImg() { return imagePtr; }

	const glm::vec4& GetColor() const;
	void SetColor(const vec4& color);

	void SetShaderType(ShaderType type) { shaderType = type; }
	ShaderType GetShaderType() const { return shaderType; }

	void ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

	void SaveBinary(const std::string& filename) const;
	static std::shared_ptr<Material> LoadBinary(const std::string& filename);

	std::shared_ptr<Image> imagePtr = std::make_shared<Image>();

private:
	unsigned int gid;
	static unsigned int next_id;
	std::string image_path;
};