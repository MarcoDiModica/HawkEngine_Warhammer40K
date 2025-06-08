#include "LightComponent.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Shaders.h"
#include <algorithm>

LightComponent::LightComponent(GameObject* owner) : Component(owner) {
	name = "LightComponent";
	CalculateAttenuation();
}

void LightComponent::Awake() {
}

void LightComponent::Start() {
	UpdateLightData();
}

void LightComponent::Update(float deltaTime) {
	UpdateLightData();
}

void LightComponent::Destroy() {
}

std::unique_ptr<Component> LightComponent::Clone(GameObject* owner) {
	auto light = std::make_unique<LightComponent>(*this);
	light->owner = owner;
	return light;
}

void LightComponent::SetLightType(LightType newType) {
	this->type = newType;

	switch (type) {
	case LightType::DIRECTIONAL:
		owner->SetName("DirectionalLight");
		break;
	case LightType::POINT:
		owner->SetName("PointLight");
		break;
	case LightType::SPOT:
		owner->SetName("SpotLight");
		break;
	default:
		break;
	}

	UpdateLightData();
}

LightType LightComponent::GetLightType() const {
	return type;
}

void LightComponent::SetColor(const vec3& newColor) {
	this->color = newColor;
	UpdateLightData();
}

vec3 LightComponent::GetColor() const {
	return color;
}

void LightComponent::SetIntensity(float newIntensity) {
	this->intensity = std::max(0.0f, newIntensity);
	UpdateLightData();
}

float LightComponent::GetIntensity() const {
	return intensity;
}

void LightComponent::SetRange(float newRange) {
	this->range = std::max(0.1f, newRange);
	CalculateAttenuation();
	UpdateLightData();
}

float LightComponent::GetRange() const {
	return range;
}

void LightComponent::SetInnerConeAngle(float angle) {
	this->innerConeAngle = std::clamp(angle, 0.0f, 90.0f);
	if (innerConeAngle > outerConeAngle) {
		outerConeAngle = innerConeAngle;
	}
	UpdateLightData();
}

float LightComponent::GetInnerConeAngle() const {
	return innerConeAngle;
}

void LightComponent::SetOuterConeAngle(float angle) {
	this->outerConeAngle = std::clamp(angle, 0.0f, 90.0f);
	if (outerConeAngle < innerConeAngle) {
		innerConeAngle = outerConeAngle;
	}
	UpdateLightData();
}

float LightComponent::GetOuterConeAngle() const {
	return outerConeAngle;
}

void LightComponent::CalculateAttenuation() {
	constant = 1.0f;
	linear = 4.5f / range;
	quadratic = 75.0f / (range * range);
}

void LightComponent::UpdateLightData() {
	if (!owner || !owner->GetComponent<Transform_Component>()) return;

	auto transform = owner->GetComponent<Transform_Component>();

	switch (type) {
	case LightType::POINT:
		pointLight.position = transform->GetPosition();
		pointLight.color = color;
		pointLight.intensity = intensity;
		pointLight.range = range;
		pointLight.constant = constant;
		pointLight.linear = linear;
		pointLight.quadratic = quadratic;
		break;

	case LightType::DIRECTIONAL:
		directionalLight.direction = glm::normalize(transform->GetForward());
		directionalLight.color = color;
		directionalLight.intensity = intensity;
		break;

	case LightType::SPOT:
		spotLight.position = transform->GetPosition();
		spotLight.direction = glm::normalize(transform->GetForward());
		spotLight.color = color;
		spotLight.intensity = intensity;
		spotLight.range = range;
		spotLight.innerCone = glm::cos(glm::radians(innerConeAngle));
		spotLight.outerCone = glm::cos(glm::radians(outerConeAngle));
		spotLight.constant = constant;
		spotLight.linear = linear;
		spotLight.quadratic = quadratic;
		break;

	default:
		break;
	}
}

PointLight LightComponent::GetPointLight() const {
	return pointLight;
}

DirectionalLight LightComponent::GetDirectionalLight() const {
	return directionalLight;
}

SpotLight LightComponent::GetSpotLight() const {
	return spotLight;
}