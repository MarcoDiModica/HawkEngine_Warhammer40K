#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"

class UISliderComponent :  public Component
{
public:
	explicit UISliderComponent(GameObject* owner);
	~UISliderComponent() override = default;

	UISliderComponent(const UISliderComponent&) = delete;
	UISliderComponent& operator=(const UISliderComponent&) = delete;

	UISliderComponent(UISliderComponent&&) noexcept = default;
	UISliderComponent& operator=(UISliderComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::SLIDER; }

	void SetTexture(std::string path);

	void LoadMesh();

	void SetProjection(const glm::mat4& proj) { projection = proj; }

private:
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	Shaders* shader;
	glm::mat4 projection;
	bool resized = false;

protected:

	friend class SceneSerializer;

	YAML::Node encode() override {

		YAML::Node node = Component::encode();

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

