#include "Material.h"
#include <GL/glew.h>
#include <fstream>
#include <zlib.h>
#include <unordered_map>
#include <filesystem>
#include "../MyGameEditor/Log.h"

unsigned int Material::next_id = 0;

Material::Material() : gid(next_id++){
	color = vec4(1.0f);
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

vec4 Material::GetColor() {
	return this->color;
}

void Material::bind() const {
	glBindTexture(GL_TEXTURE_2D, imagePtr->id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapMode(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLMinFilter(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLMagFilter(filter));
}

bool Material::loadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
	return shader.LoadShaders(vertexShaderFile, fragmentShaderFile);
}

void Material::SetShader(Shaders& shader) 
{
	this->shader = shader;
}

Shaders Material::GetShader() 
{
	return this->shader;
}


// Function to bind shaders
void Material::bindShaders() const {
	shader.Bind();
}

// Function to set shader uniforms
void Material::setShaderUniform(const std::string& name, int value) {
	shader.SetUniform(name, value);
}

void Material::setShaderUniform(const std::string& name, float value) {
	shader.SetUniform(name, value);
}

void Material::setShaderUniform(const std::string& name, const glm::vec3& value) {
	shader.SetUniform(name, value);
}

void Material::setShaderUniform(const std::string& name, const glm::vec4& value) {
	shader.SetUniform(name, value);
}

void Material::setShaderUniform(const std::string& name, const glm::mat4& value) {
	shader.SetUniform(name, value);
}

std::unordered_map<std::string, std::shared_ptr<Material>> materialCache;

void Material::SaveBinary(const std::string& filename) const {
	
	std::string fullPath = "Library/Materials/" + filename + ".mat";
	LOG(LogType::LOG_INFO, "Saving material to: %s", fullPath.c_str());

	if (!std::filesystem::exists("Library/Materials")) {

		std::filesystem::create_directory("Library/Materials");
	}

	std::ofstream fout(fullPath + ".mat", std::ios::binary);
	if (!fout.is_open()) {
		return;
	}

	fout.write(reinterpret_cast<const char*>(&wrapMode), sizeof(wrapMode));
	fout.write(reinterpret_cast<const char*>(&filter), sizeof(filter));
	fout.write(reinterpret_cast<const char*>(&color), sizeof(color));
	fout.write(reinterpret_cast<const char*>(&useShader), sizeof(useShader));

	if (imagePtr) {
		fout.write("IMG", 3);
		imagePtr->SaveBinary(filename);
	}
	else {
		fout.write("NOI", 3);
	}

	if (useShader) {
		//shader.SaveBinary(fout);
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
	fin.read(reinterpret_cast<char*>(&mat->useShader), sizeof(mat->useShader));

	char type[4];
	fin.read(type, 3);
	type[3] = '\0';

	if (strcmp(type, "IMG") == 0) {
		std::shared_ptr<Image> img = Image::LoadBinary(filename);
		mat->setImage(img);
	}

	if (mat->useShader) {
		//mat->shader = Shaders::LoadBinary(fin);
	}

	materialCache[fullPath] = mat;
	
	

	return mat;
}