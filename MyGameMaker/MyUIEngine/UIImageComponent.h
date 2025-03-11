#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"
#include <yaml-cpp/yaml.h>

#include <string>

class UIImageComponent : public Component
{
public:
	explicit UIImageComponent(GameObject* owner);
	~UIImageComponent() override = default;

	UIImageComponent(const UIImageComponent&) = delete;
	UIImageComponent& operator=(const UIImageComponent&) = delete;

	UIImageComponent(UIImageComponent&&) noexcept = default;
	UIImageComponent& operator=(UIImageComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::IMAGE; }
	const std::string& GetImagePath() const { return texturePath; }

	void SetTexture(std::string path);

	std::shared_ptr<Image> GetTexture() const { return texture; }

	void LoadMesh();

	void SetProjection(const glm::mat4& proj) { projection = proj; }

	glm::vec2 GetImageSize() const { return glm::vec2(texture->width(), texture->height()); }

private:
	//texture
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	Shaders * shader;
	glm::mat4 projection;
	bool resized = false;

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

		/*node["texture_path"] = texturePath;
		node["shader"] = shader;
		node["mesh"] = mesh;
		node["resized"] = resized;*/
		//node["light_type"] = static_cast<int>(type);
		////node["color"] = { color.r, color.g, color.b };
		//node["intensity"] = intensity;
		//node["radius"] = radius;
		//node["direction"] = { direction.x, direction.y, direction.z };

		return node;
	}

	bool decode(const YAML::Node& node) override {

		Component::decode(node);

		//type = static_cast<LightType>(node["light_type"].as<int>());
		////color = { node["color"][0].as<float>(), node["color"][1].as<float>(), node["color"][2].as<float>() };
		//intensity = node["intensity"].as<float>();
		//radius = node["radius"].as<float>();
		//direction = { node["direction"][0].as<float>(), node["direction"][1].as<float>(), node["direction"][2].as<float>() };

		return true;
	}
};

