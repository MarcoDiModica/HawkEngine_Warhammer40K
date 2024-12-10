#include "LightComponent.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Shaders.h"

LightComponent::LightComponent(GameObject* owner) : Component(owner) { name = "Light"; }

void LightComponent::Start() {
    // init aqui
}

void LightComponent::Update(float deltaTime) {
    switch (type) 
    {
        case LightType::POINT:
            UpdatePointLight();
            break;
        case LightType::DIRECTIONAL:
            UpdateDirectionalLight();
            break;
        case LightType::NONE:
			break;
        default:
			break;
    }
}

void LightComponent::Destroy() {
	// limpia aqui
}

std::unique_ptr<Component> LightComponent::Clone(GameObject* owner) {
	auto light = std::make_unique<LightComponent>(*this);
	light->owner = owner;
	return light;
}

void LightComponent::SetLightType(LightType type) {
	this->type = type;
}

LightType LightComponent::GetLightType() const {
	return type;
}

void LightComponent::SetColor(const glm::vec3& color) {
	this->color = color;
}

glm::vec3 LightComponent::GetColor() const {
	return color;
}

void LightComponent::SetIntensity(float intensity) {
	this->intensity = intensity;
}

float LightComponent::GetIntensity() const {
	return intensity;
}

void LightComponent::SetRadius(float radius) {
	this->radius = radius;
}

float LightComponent::GetRadius() const {
	return radius;
}

void LightComponent::SetDirection(const glm::vec3& direction) {
	this->direction = direction;
}

glm::vec3 LightComponent::GetDirection() const {
	return direction;
}

void LightComponent::UpdatePointLight() {
	//aqui logica
}

void LightComponent::UpdateDirectionalLight() {
	//aqui logica
}