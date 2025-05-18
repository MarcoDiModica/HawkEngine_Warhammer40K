#pragma once

#include "Component.h"
#include "GameObject.h"

class SceneSerializer;

enum class LightType {
    NONE,
    POINT,
    DIRECTIONAL
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float radius;
    float intensity;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
	float darknessFallback;
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

    void SetDirection(const glm::vec3& direction);
    glm::vec3 GetDirection() const;

    vec3 GetAmbient() const;
    vec3 GetDiffuse() const;
    vec3 GetSpecular() const;
    float GetConstant() const;
    float GetLinear() const;
    float GetQuadratic() const;
    float GetRadius() const;
    float GetIntensity() const;
	float GetDarknessFallback() const { return darknessFallback; }
	void SetDarknessFallback(float darkness) { darknessFallback = darkness; }

  
    void SetAmbient(const vec3& ambient);
    void SetDiffuse(const vec3& diffuse);
    void SetSpecular(const vec3& specular);
    void SetConstant(float constant);
    void SetLinear(float linear);
    void SetQuadratic(float quadratic);
    void SetRadius(float radius);
    void SetIntensity(float intensity);

private:
    void UpdatePointLight();
    void UpdateDirectionalLight();

    LightType type = LightType::POINT;
	PointLight pointLight;
	DirLight dirLight;

    vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    vec3 specular = glm::vec3(-0.2f, -1.0f, -0.3f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float radius = 1.0f;
    float intensity = 3.0f;
    glm::vec3 direction = { -1.0f, -1.0f, 0.0f };
	float darknessFallback = 0.00001f; // Fallback darkness value for the light

protected:

    friend class SceneSerializer;
YAML::Node encode() override {

    YAML::Node node = Component::encode();

    node["light_type"] = static_cast<int>(type);
    node["ambient"] = YAML::Node(YAML::NodeType::Sequence);
    node["ambient"].push_back(ambient.r);
    node["ambient"].push_back(ambient.g);
    node["ambient"].push_back(ambient.b);
    node["diffuse"] = YAML::Node(YAML::NodeType::Sequence);
    node["diffuse"].push_back(diffuse.r);
    node["diffuse"].push_back(diffuse.g);
    node["diffuse"].push_back(diffuse.b);
    node["specular"] = YAML::Node(YAML::NodeType::Sequence);
    node["specular"].push_back(specular.r);
    node["specular"].push_back(specular.g);
    node["specular"].push_back(specular.b);
    node["constant"] = constant;
    node["linear"] = linear;
    node["quadratic"] = quadratic;
    node["radius"] = radius;
    node["intensity"] = intensity;
    node["direction"] = YAML::Node(YAML::NodeType::Sequence);
    node["direction"].push_back(direction.x);
    node["direction"].push_back(direction.y);
    node["direction"].push_back(direction.z);

    return node;
}

    bool decode(const YAML::Node& node) override {

        Component::decode(node);

        type = static_cast<LightType>(node["light_type"].as<int>());
        ambient = { node["ambient"][0].as<float>(), node["ambient"][1].as<float>(), node["ambient"][2].as<float>() };
        diffuse = { node["diffuse"][0].as<float>(), node["diffuse"][1].as<float>(), node["diffuse"][2].as<float>() };
        specular = { node["specular"][0].as<float>(), node["specular"][1].as<float>(), node["specular"][2].as<float>() };
        constant = node["constant"].as<float>();
        linear = node["linear"].as<float>();
        quadratic = node["quadratic"].as<float>();
        radius = node["radius"].as<float>();
        intensity = node["intensity"].as<float>();
        direction = { node["direction"][0].as<float>(), node["direction"][1].as<float>(), node["direction"][2].as<float>() };

        return true;
    }
};