#pragma once

#include "Component.h"
#include "GameObject.h"

class SceneSerializer;

enum class LightType {
	NONE,
	POINT,
	DIRECTIONAL,
	SPOT
};

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	float range;
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float range;
	float innerCone;
	float outerCone;
	float constant;
	float linear;
	float quadratic;
};

class LightComponent : public Component {
public:
	LightComponent(GameObject* owner);

	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::LIGHT; }

	void SetLightType(LightType type);
	LightType GetLightType() const;

	void SetColor(const vec3& color);
	vec3 GetColor() const;

	void SetIntensity(float intensity);
	float GetIntensity() const;

	void SetRange(float range);
	float GetRange() const;

	void SetInnerConeAngle(float angle);
	float GetInnerConeAngle() const;

	void SetOuterConeAngle(float angle);
	float GetOuterConeAngle() const;

	PointLight GetPointLight() const;
	DirectionalLight GetDirectionalLight() const;
	SpotLight GetSpotLight() const;

private:
	void UpdateLightData();
	void CalculateAttenuation();

	LightType type = LightType::POINT;

	vec3 color = vec3(1.0f, 1.0f, 1.0f);
	float intensity = 1.0f;
	float range = 10.0f;
	float innerConeAngle = 30.0f;
	float outerConeAngle = 45.0f;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	PointLight pointLight;
	DirectionalLight directionalLight;
	SpotLight spotLight;

protected:
	friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node = Component::encode();

		node["light_type"] = static_cast<int>(type);
		node["color"] = YAML::Node(YAML::NodeType::Sequence);
		node["color"].push_back(color.r);
		node["color"].push_back(color.g);
		node["color"].push_back(color.b);
		node["intensity"] = intensity;
		node["range"] = range;
		node["inner_cone_angle"] = innerConeAngle;
		node["outer_cone_angle"] = outerConeAngle;

		return node;
	}

	bool decode(const YAML::Node& node) override {
		Component::decode(node);

		type = static_cast<LightType>(node["light_type"].as<int>());

		if (node["color"] && node["color"].IsSequence() && node["color"].size() == 3) {
			color = { node["color"][0].as<float>(), node["color"][1].as<float>(), node["color"][2].as<float>() };
		}
		else if (node["diffuse"] && node["diffuse"].IsSequence() && node["diffuse"].size() == 3) {
			color = { node["diffuse"][0].as<float>(), node["diffuse"][1].as<float>(), node["diffuse"][2].as<float>() };
		}
		else {
			color = vec3(1.0f, 1.0f, 1.0f);
		}

		if (node["intensity"]) {
			intensity = node["intensity"].as<float>();
		}
		else {
			intensity = 1.0f;
		}

		if (node["range"]) {
			range = node["range"].as<float>();
		}
		else if (node["radius"]) {
			range = node["radius"].as<float>();
		}
		else {
			range = 10.0f;
		}

		if (node["inner_cone_angle"]) {
			innerConeAngle = node["inner_cone_angle"].as<float>();
		}
		else {
			innerConeAngle = 30.0f;
		}

		if (node["outer_cone_angle"]) {
			outerConeAngle = node["outer_cone_angle"].as<float>();
		}
		else {
			outerConeAngle = 45.0f;
		}

		if (node["constant"]) {
			constant = node["constant"].as<float>();
		}
		if (node["linear"]) {
			linear = node["linear"].as<float>();
		}
		if (node["quadratic"]) {
			quadratic = node["quadratic"].as<float>();
		}

		CalculateAttenuation();

		return true;
	}
};