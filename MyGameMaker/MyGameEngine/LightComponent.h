#pragma once

#include "Component.h"
#include "GameObject.h"

class SceneSerializer;

enum class LightType {
    NONE,
    POINT,
    DIRECTIONAL
};

class LightComponent : public Component {
public:
    LightComponent(GameObject* owner);

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    std::unique_ptr<Component> Clone(GameObject* owner) override;

    ComponentType GetType() const override { return ComponentType::LIGHT; }

    void SetLightType(LightType type);
    LightType GetLightType() const;

    void SetColor(const glm::vec3& color);
    glm::vec3 GetColor() const;

    void SetIntensity(float intensity);
    float GetIntensity() const;

    void SetRadius(float radius);
    float GetRadius() const;

    void SetDirection(const glm::vec3& direction);
    glm::vec3 GetDirection() const;

private:
    void UpdatePointLight();
    void UpdateDirectionalLight();

    LightType type = LightType::NONE;
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;

    float radius = 1.0f;

    glm::vec3 direction = { -1.0f, -1.0f, 0.0f };

protected:

    friend class SceneSerializer;

    YAML::Node encode() override {

		YAML::Node node = Component::encode();

		node["light_type"] = static_cast<int>(type);
		//node["color"] = { color.r, color.g, color.b };
		node["intensity"] = intensity;
		node["radius"] = radius;
		//node["direction"] = { direction.x, direction.y, direction.z };

		return node;
	}

    bool decode(const YAML::Node& node) override {

		Component::decode(node);

		type = static_cast<LightType>(node["light_type"].as<int>());
		//color = { node["color"][0].as<float>(), node["color"][1].as<float>(), node["color"][2].as<float>() };
		intensity = node["intensity"].as<float>();
		radius = node["radius"].as<float>();
		//direction = { node["direction"][0].as<float>(), node["direction"][1].as<float>(), node["direction"][2].as<float>() };

		return true;
	}
};