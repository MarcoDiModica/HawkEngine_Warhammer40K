#pragma once

#include <memory>
#include <vector>
#include "Image.h"
#include "Shaders.h"
#include "types.h"
#include "yaml-cpp/yaml.h"

namespace YAML {
    template <>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }
        static bool decode(const Node& node, glm::vec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
}

class Material
{
public:
	Material();

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

	void bind() const;
	void unbind() const;

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

	void ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

	void SaveBinary(const std::string& filename) const;
	static std::shared_ptr<Material> LoadBinary(const std::string& filename);

	void SetTonemapStrength(float strength) { tonemapStrength = strength; }
	float GetTonemapStrength() const { return tonemapStrength; }

private:
	unsigned int gid;
	static unsigned int next_id;
	std::string image_path;

	void bindTexture(const std::shared_ptr<Image>& texture, GLenum textureUnit) const;

protected:
	friend class SceneSerializer;
	friend class MeshRenderer;

    YAML::Node encode() const {
        YAML::Node node;

        std::string wrapModeStr;
        switch (wrapMode) {
        case Repeat:          wrapModeStr = "Repeat"; break;
        case MirroredRepeat:  wrapModeStr = "MirroredRepeat"; break;
        case Clamp:           wrapModeStr = "Clamp"; break;
        default:              wrapModeStr = "Repeat"; break;
        }
        node["wrap_mode"] = wrapModeStr;

        std::string filterStr;
        switch (filter) {
        case Nearest: filterStr = "Nearest"; break;
        case Linear:  filterStr = "Linear"; break;
        default:      filterStr = "Nearest"; break;
        }
        node["filter"] = filterStr;

        node["color"] = std::vector<float>{ color.x, color.y, color.z, color.w };

        node["metallic"] = metallic;
        node["roughness"] = roughness;
        node["ao"] = ao;
        node["tonemap_strength"] = tonemapStrength;

        std::string shaderTypeStr;
        switch (shaderType) {
        case ShaderType::PBR: shaderTypeStr = "PBR"; break;
		case ShaderType::UNLIT: shaderTypeStr = "UNLIT"; break;
        }
        node["shader_type"] = shaderTypeStr;

        node["image"] = image_path;

        return node;
    }

    bool decode(const YAML::Node& node) {
        if (!node["wrap_mode"] || !node["filter"] || !node["color"] ||
            !node["metallic"] || !node["roughness"] || !node["ao"] ||
            !node["tonemap_strength"] || !node["shader_type"])
        {
            return false;
        }

        std::string wrapModeStr = node["wrap_mode"].as<std::string>();
        if (wrapModeStr == "Repeat")
            wrapMode = Repeat;
        else if (wrapModeStr == "MirroredRepeat")
            wrapMode = MirroredRepeat;
        else if (wrapModeStr == "Clamp")
            wrapMode = Clamp;
        else
            wrapMode = Repeat;

        std::string filterStr = node["filter"].as<std::string>();
        if (filterStr == "Nearest")
            filter = Nearest;
        else if (filterStr == "Linear")
            filter = Linear;
        else
            filter = Nearest;

        auto colorVec = node["color"].as<std::vector<float>>();
        if (colorVec.size() == 4)
            color = glm::vec4(colorVec[0], colorVec[1], colorVec[2], colorVec[3]);
        else
            return false;

        metallic = node["metallic"].as<float>();
        roughness = node["roughness"].as<float>();
        ao = node["ao"].as<float>();
        tonemapStrength = node["tonemap_strength"].as<float>();

        std::string shaderTypeStr = node["shader_type"].as<std::string>();
        if (shaderTypeStr == "PBR")
            shaderType = ShaderType::PBR;
        else
            shaderType = ShaderType::UNLIT;

        if (node["image"]) {
            image_path = node["image"].as<std::string>();
            if (!image_path.empty()) {
                imagePtr = std::make_shared<Image>();
                imagePtr->LoadTextureLocalPath(image_path);
            }
        }
        return true;
    }

};