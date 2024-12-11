#pragma once

#include "Component.h"
#include "GameObject.h"

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

    vec3 GetAmbient() const;
    vec3 GetDiffuse() const;
    vec3 GetSpecular() const;
    float GetConstant() const;
    float GetLinear() const;
    float GetQuadratic() const;
    float GetRadius() const;
    float GetIntensity() const;

  
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

    vec3 ambient = glm::vec3(0.2f, 0.8f, 0.2f);
    vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float radius = 1.0f;
    float intensity = 2.5f;
};