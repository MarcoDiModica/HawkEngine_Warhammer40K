#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include <zlib.h>
#include <fstream>
#include "Image.h"
#include "Shaders.h"
#include "types.h"
#include "yaml-cpp/yaml.h"
#include "../MyGameEditor/Log.h"

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

	void SetID(size_t id) { matID = id; }
	size_t GetId() const { return matID; }
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

	size_t matID;

	void SetMatName(const std::string& name) { matName = name; }
	std::string GetMatName() const { return matName; }

	std::string matName = "";

	std::shared_ptr<Image> CheckImageResource(const std::string& path);

protected:
	static unsigned int next_id;
	

	void bindTexture(const std::shared_ptr<Image>& texture, GLenum textureUnit) const;

protected:
	friend class SceneSerializer;
	friend class MeshRenderer;

    YAML::Node encode() const {
        YAML::Node node;

        std::string name = matName;
        node["name"] = name;

		node["color"] = std::vector<float>{ color.r, color.g, color.b, color.a };


		SaveBinary(name);

        return node;
    }

    bool decode(const YAML::Node& node) {
		if (!node["name"])
			return false;

		std::string name = node["name"].as<std::string>();

		std::string fullPath = "Library/Materials/" + name + ".mat";

		std::ifstream fin(fullPath, std::ios::binary);
		if (!fin.is_open()) {
			return false;
			throw std::runtime_error("Error opening material file: " + fullPath);
		}

		matName = name;

		fin.read(reinterpret_cast<char*>(&wrapMode), sizeof(wrapMode));
		fin.read(reinterpret_cast<char*>(&filter), sizeof(filter));
		fin.read(reinterpret_cast<char*>(&color), sizeof(color));
		fin.read(reinterpret_cast<char*>(&shaderType), sizeof(shaderType));

		if (node["color"]) {
			auto colorValues = node["color"].as<std::vector<float>>();
			if (colorValues.size() == 4) {
				color = glm::vec4(colorValues[0], colorValues[1], colorValues[2], colorValues[3]);
			}
		}

		while (fin.peek() != EOF) {
			char type[4];
			fin.read(type, 3);
			type[3] = '\0';

			uint32_t pathLen;
			fin.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));

			std::string texturePath(pathLen, '\0');
			fin.read(&texturePath[0], pathLen);

			std::shared_ptr<Image> img = CheckImageResource(texturePath);

			if (img == nullptr)
			{
				img = Image::LoadBinary(texturePath);
			}

			if (strcmp(type, "IMG") == 0) {
				setImage(img);
			}
			else if (strcmp(type, "NML") == 0) {
				setNormalMap(img);
			}
			else if (strcmp(type, "MTL") == 0) {
				setMetallicMap(img);
			}
			else if (strcmp(type, "RGL") == 0) {
				setRoughnessMap(img);
			}
			else if (strcmp(type, "AOM") == 0) {
				setAoMap(img);
			}
		}

		//LOG(LogType::LOG_INFO, "Material loaded successfully: %s", fullPath.c_str());

        return true;
    }

};