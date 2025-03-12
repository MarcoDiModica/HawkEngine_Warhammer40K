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

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
	//texture
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	glm::vec4 color = glm::vec4(1.0f);
	Shaders * shader;
	glm::mat4 projection;

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

		node["texture_path"] = texturePath;
		/*node["shader"] = shader;*/
		/*node["mesh"] = mesh;*/

		return node;
	}

	bool decode(const YAML::Node& node) override {

		Component::decode(node);

		texturePath = node["texture_path"].as<std::string>();
		/*shader = node["shader"].as<Shaders*>();*/
		/*mesh = node["mesh"].as<std::shared_ptr<Mesh>>();*/

		return true;
	}
};

