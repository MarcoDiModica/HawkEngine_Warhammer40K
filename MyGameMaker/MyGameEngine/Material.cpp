#include "Material.h"
#include "ShaderManager.h"
#include <GL/glew.h>
#include <fstream>
#include <zlib.h>
#include <unordered_map>
#include <filesystem>
#include "../MyGameEditor/Log.h"
#include "../MyGameEditor/App.h"
#include "ResourceManager.h"

unsigned int Material::next_id = 0;

Material::Material() : matID(next_id++) {
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	imagePtr = std::make_shared<Image>();
	shaderType = ShaderType::PBR;
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

void Material::bindTexture(const std::shared_ptr<Image>& texture, GLenum textureUnit) const {
	if (texture && texture->id() != 0) {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture->id());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapMode(wrapMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapMode(wrapMode));
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLMinFilter(filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLMagFilter(filter));
	}
}

void Material::bind() const {
	if (!imagePtr || imagePtr->id() == 0) return;

	if (!imagePtr || imagePtr->id() == 0) return;

	// Albedo texture (unit 0)
	bindTexture(imagePtr, GL_TEXTURE0);

	// Normal map (unit 1)
	bindTexture(normalMapPtr, GL_TEXTURE1);

	// Metallic map (unit 2)
	bindTexture(metallicMapPtr, GL_TEXTURE2);

	// Roughness map (unit 3)
	bindTexture(roughnessMapPtr, GL_TEXTURE3);

	// AO map (unit 4)
	bindTexture(aoMapPtr, GL_TEXTURE4);
}

void Material::unbind() const {
	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}

void Material::ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
	Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
	if (!shader) {
		return;
	}

	glUseProgram(0);

	shader->Bind();

	shader->SetUniform("model", model);
	shader->SetUniform("view", view);
	shader->SetUniform("projection", projection);

	shader->SetUniform("albedoColor", glm::vec4(color));

	if (shaderType == ShaderType::PBR) {
		shader->SetUniform("metallicFactor", metallic);
		shader->SetUniform("roughnessFactor", roughness);
		shader->SetUniform("aoFactor", ao);

		if (imagePtr && imagePtr->id() != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, imagePtr->id());
			shader->SetUniform("u_HasAlbedoMap", 1);
			shader->SetUniform("albedoMap", 0);
		}
		else {
			shader->SetUniform("u_HasAlbedoMap", 0);
		}

		if (normalMapPtr && normalMapPtr->id() != 0) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMapPtr->id());
			shader->SetUniform("u_HasNormalMap", 1);
			shader->SetUniform("normalMap", 1);
		}
		else {
			shader->SetUniform("u_HasNormalMap", 0);
		}

		if (metallicMapPtr && metallicMapPtr->id() != 0) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metallicMapPtr->id());
			shader->SetUniform("u_HasMetallicMap", 1);
			shader->SetUniform("metallicMap", 2);
		}
		else {
			shader->SetUniform("u_HasMetallicMap", 0);
		}

		if (roughnessMapPtr && roughnessMapPtr->id() != 0) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, roughnessMapPtr->id());
			shader->SetUniform("u_HasRoughnessMap", 1);
			shader->SetUniform("roughnessMap", 3);
		}
		else {
			shader->SetUniform("u_HasRoughnessMap", 0);
		}

		if (aoMapPtr && aoMapPtr->id() != 0) {
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, aoMapPtr->id());
			shader->SetUniform("u_HasAoMap", 1);
			shader->SetUniform("aoMap", 4);
		}
		else {
			shader->SetUniform("u_HasAoMap", 0);
		}

		shader->SetUniform("tonemapStrength", tonemapStrength);
	}
	else if (shaderType == ShaderType::UNLIT) {
		if (imagePtr && imagePtr->id() != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, imagePtr->id());
			shader->SetUniform("u_HasTexture", 1);
			shader->SetUniform("texture1", 0);
		}
		else {
			shader->SetUniform("u_HasTexture", 0);
		}
	}
}

void Material::SaveBinary(const std::string& filename) const {
	std::string fullPath = "Library/Materials/" + filename + ".mat";

	if (!std::filesystem::exists("Library/Materials")) {
		std::filesystem::create_directory("Library/Materials");
	}

	if (std::filesystem::exists(fullPath)) {
		return;
	}

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		return;
	}

	fout.write(reinterpret_cast<const char*>(&wrapMode), sizeof(wrapMode));
	fout.write(reinterpret_cast<const char*>(&filter), sizeof(filter));
	fout.write(reinterpret_cast<const char*>(&color), sizeof(color));
	fout.write(reinterpret_cast<const char*>(&shaderType), sizeof(shaderType));

	auto writeTexture = [&](const std::string& tag, const std::shared_ptr<Image>& img) {
		if (img && !img->image_name.empty()) {
			fout.write(tag.c_str(), 3); // Escribe tipo
			uint32_t len = img->image_name.size();
			fout.write(reinterpret_cast<char*>(&len), sizeof(len));
			fout.write(img->image_name.c_str(), len);
			img->SaveBinary(img->image_name);
		}
		};

	writeTexture("IMG", imagePtr);
	writeTexture("NML", normalMapPtr);
	writeTexture("MTL", metallicMapPtr);
	writeTexture("RGL", roughnessMapPtr);
	writeTexture("AOM", aoMapPtr);

	LOG(LogType::LOG_INFO, "Material saved to: %s", fullPath.c_str());
}

std::shared_ptr<Material> Material::LoadBinary(const std::string& filename) {
	std::string fullPath = "Library/Materials/" + filename + ".mat";
	
	if (Application->root->GetResourceManager()->GetMaterial(filename) != nullptr)
	{
		auto material = Application->root->GetResourceManager()->GetMaterial(filename);
		return material;
	}

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		throw std::runtime_error("Error opening material file: " + fullPath);
	}

	std::shared_ptr<Material> mat;

	mat = std::make_shared<Material>();

	mat->matID = std::stoull(filename);

	fin.read(reinterpret_cast<char*>(&mat->wrapMode), sizeof(mat->wrapMode));
	fin.read(reinterpret_cast<char*>(&mat->filter), sizeof(mat->filter));
	fin.read(reinterpret_cast<char*>(&mat->color), sizeof(mat->color));
	fin.read(reinterpret_cast<char*>(&mat->shaderType), sizeof(mat->shaderType));

	while (fin.peek() != EOF) {
		char type[4];
		fin.read(type, 3);
		type[3] = '\0';

		uint32_t pathLen;
		fin.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));

		std::string texturePath(pathLen, '\0');
		fin.read(&texturePath[0], pathLen);

		std::shared_ptr<Image> img = Image::LoadBinary(texturePath);

		if (strcmp(type, "IMG") == 0) {
			mat->setImage(img);
		}
		else if (strcmp(type, "NML") == 0) {
			mat->setNormalMap(img);
		}
		else if (strcmp(type, "MTL") == 0) {
			mat->setMetallicMap(img);
		}
		else if (strcmp(type, "RGL") == 0) {
			mat->setRoughnessMap(img);
		}
		else if (strcmp(type, "AOM") == 0) {
			mat->setAoMap(img);
		}
	}

	LOG(LogType::LOG_INFO, "Material loaded successfully: %s", fullPath.c_str());
	return mat;
}