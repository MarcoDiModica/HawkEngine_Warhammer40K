#include "Material.h"
#include "ShaderManager.h"
#include <GL/glew.h>
#include <fstream>
#include <zlib.h>
#include <unordered_map>
#include <filesystem>
#include "../MyGameEditor/Log.h"

unsigned int Material::next_id = 0;

Material::Material() : gid(next_id++) {
	color = vec4(1.0f);
	imagePtr = std::make_shared<Image>();
	shaderType = ShaderType::UNLIT;
}

static auto GLWrapMode(Material::WrapModes mode) {
	switch (mode) {
	case Material::Repeat: return GL_REPEAT;
	case Material::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case Material::Clamp: return GL_CLAMP_TO_EDGE;
	default: return GL_REPEAT;
	}
}

static auto GLMagFilter(Material::Filters filter) {
	switch (filter) {
	case Material::Nearest: return GL_NEAREST;
	case Material::Linear: return GL_LINEAR;
	default: return GL_NEAREST;
	}
}

static auto GLMinFilter(Material::Filters filter) {
	switch (filter) {
	case Material::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
	case Material::Linear: return GL_LINEAR_MIPMAP_LINEAR;
	default: return GL_NEAREST_MIPMAP_LINEAR;
	}
}

void Material::SetColor(const vec4& color) {
	this->color = color;
}

const glm::vec4& Material::GetColor() const {
	return this->color;
}

void Material::bind() const {
	if (!imagePtr || imagePtr->id() == 0) return;

	glBindTexture(GL_TEXTURE_2D, imagePtr->id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapMode(wrapMode));

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
}

void Material::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "Shader not found for material");
		return;
	}

	shader->Bind();

	// Set common uniforms
	shader->SetUniform("model", model);
	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);
	shader->SetUniform("modColor", color);

	bool hasTexture = imagePtr && !imagePtr->image_path.empty();
	shader->SetUniform("u_HasTexture", hasTexture ? 1 : 0);

	if (hasTexture) {
		shader->SetUniform("texture1", 0);
	}
}

std::unordered_map<std::string, std::shared_ptr<Material>> materialCache;

void Material::SaveBinary(const std::string& filename) const {
	std::string fullPath = "Library/Materials/" + filename + ".mat";
	LOG(LogType::LOG_INFO, "Saving material to: %s", fullPath.c_str());

	if (!std::filesystem::exists("Library/Materials")) {
		std::filesystem::create_directory("Library/Materials");
	}

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		return;
	}

	fout.write(reinterpret_cast<const char*>(&wrapMode), sizeof(wrapMode));
	fout.write(reinterpret_cast<const char*>(&filter), sizeof(filter));
	fout.write(reinterpret_cast<const char*>(&color), sizeof(color));
	fout.write(reinterpret_cast<const char*>(&shaderType), sizeof(shaderType));

	if (imagePtr && !imagePtr->image_path.empty()) {
		fout.write("IMG", 3);
		imagePtr->SaveBinary(filename);
	}
	else {
		fout.write("NOI", 3);
	}
}

std::shared_ptr<Material> Material::LoadBinary(const std::string& filename) {
	std::string fullPath = "Library/Materials/" + filename + ".mat";
	LOG(LogType::LOG_INFO, "Loading material from: %s", fullPath.c_str());

	auto it = materialCache.find(fullPath);
	if (it != materialCache.end()) {
		return it->second;
	}

	std::shared_ptr<Material> mat;

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		throw std::runtime_error("Error opening material file: " + fullPath);
	}

	mat = std::make_shared<Material>();

	fin.read(reinterpret_cast<char*>(&mat->wrapMode), sizeof(mat->wrapMode));
	fin.read(reinterpret_cast<char*>(&mat->filter), sizeof(mat->filter));
	fin.read(reinterpret_cast<char*>(&mat->color), sizeof(mat->color));
	fin.read(reinterpret_cast<char*>(&mat->shaderType), sizeof(mat->shaderType));

	char type[4];
	fin.read(type, 3);
	type[3] = '\0';

	if (strcmp(type, "IMG") == 0) {
		std::shared_ptr<Image> img = Image::LoadBinary(filename);
		mat->setImage(img);
	}

	materialCache[fullPath] = mat;
	return mat;
}