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
	glm::vec3 emissiveColor = vec3(0.0f, 0.0f, 0.0f);
	float emissiveIntensity = 0.0f;
	float heightScale = 0.0f;

	float tonemapStrength = 1.0f;

	ShaderType shaderType = ShaderType::PBR;

	std::shared_ptr<Image> imagePtr = std::make_shared<Image>();  // Main texture (albedo/diffuse)
	std::shared_ptr<Image> normalMapPtr = nullptr;                // Normal map
	std::shared_ptr<Image> metallicMapPtr = nullptr;              // Metallic map
	std::shared_ptr<Image> roughnessMapPtr = nullptr;             // Roughness map
	std::shared_ptr<Image> aoMapPtr = nullptr;                    // Ambient occlusion map
	std::shared_ptr<Image> heightMapPtr = nullptr;                // Height map
	std::shared_ptr<Image> emissiveMapPtr = nullptr;              // Emissive map

	void SetID(size_t id) { matID = id; }
	size_t GetId() const { return matID; }
	unsigned int id() const { return imagePtr ? imagePtr->id() : 0; }

	virtual void bind() const;
	virtual void unbind() const;

    //setters
	void setImage(const std::shared_ptr<Image>& img_ptr) { imagePtr = img_ptr; }
	void setNormalMap(const std::shared_ptr<Image>& img_ptr) { normalMapPtr = img_ptr; }
	void setMetallicMap(const std::shared_ptr<Image>& img_ptr) { metallicMapPtr = img_ptr; }
	void setRoughnessMap(const std::shared_ptr<Image>& img_ptr) { roughnessMapPtr = img_ptr; }
	void setAoMap(const std::shared_ptr<Image>& img_ptr) { aoMapPtr = img_ptr; }
	void setHeightMap(const std::shared_ptr<Image>& img_ptr) { heightMapPtr = img_ptr; }
	void setEmissiveMap(const std::shared_ptr<Image>& img_ptr) { emissiveMapPtr = img_ptr; }

    //getters
    const std::shared_ptr<Image>& getImage() const { return imagePtr; }
    const std::shared_ptr<Image>& getNormalMap() const { return normalMapPtr; }
    const std::shared_ptr<Image>& getMetallicMap() const { return metallicMapPtr; }
    const std::shared_ptr<Image>& getRoughnessMap() const { return roughnessMapPtr; }
    const std::shared_ptr<Image>& getAoMap() const { return aoMapPtr; }
	const std::shared_ptr<Image>& getHeightMap() const { return heightMapPtr; }
	const std::shared_ptr<Image>& getEmissiveMap() const { return emissiveMapPtr; }

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

	void SetEmissiveColor(const vec3& color) { emissiveColor = color; }
	void SetEmissiveIntensity(float intensity) { emissiveIntensity = intensity; }
	void SetHeightScale(float scale) { heightScale = scale; }

	vec3 GetEmissiveColor() const { return emissiveColor; }
	float GetEmissiveIntensity() const { return emissiveIntensity; }
	float GetHeightScale() const { return heightScale; }

	//ui shit
	glm::vec2 spriteOffset = glm::vec2(0.0f, 0.0f);
	glm::vec2 spriteSize = glm::vec2(1.0f, 1.0f);
	glm::vec2 sheetSize = glm::vec2(1.0f, 1.0f);

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
	friend class UIImageComponent;

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

		matName = node["name"].as<std::string>();

		std::shared_ptr<Material> loadedMaterial = LoadBinary(matName);
		if (!loadedMaterial) {
			return false;
		}

		color = loadedMaterial->color;
		metallic = loadedMaterial->metallic;
		roughness = loadedMaterial->roughness;
		ao = loadedMaterial->ao;
		emissiveColor = loadedMaterial->emissiveColor;
		emissiveIntensity = loadedMaterial->emissiveIntensity;
		heightScale = loadedMaterial->heightScale;
		tonemapStrength = loadedMaterial->tonemapStrength;
		shaderType = loadedMaterial->shaderType;
		wrapMode = loadedMaterial->wrapMode;
		filter = loadedMaterial->filter;
		spriteOffset = loadedMaterial->spriteOffset;
		spriteSize = loadedMaterial->spriteSize;
		sheetSize = loadedMaterial->sheetSize;

		imagePtr = loadedMaterial->imagePtr;
		normalMapPtr = loadedMaterial->normalMapPtr;
		metallicMapPtr = loadedMaterial->metallicMapPtr;
		roughnessMapPtr = loadedMaterial->roughnessMapPtr;
		aoMapPtr = loadedMaterial->aoMapPtr;
		heightMapPtr = loadedMaterial->heightMapPtr;
		emissiveMapPtr = loadedMaterial->emissiveMapPtr;

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