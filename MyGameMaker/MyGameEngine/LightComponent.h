#pragma once

#include "Component.h"
#include "GameObject.h"

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
};