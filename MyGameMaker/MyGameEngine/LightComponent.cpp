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

vec3 LightComponent::GetAmbient() const {
    return ambient;
}

vec3 LightComponent::GetDiffuse() const {
    return diffuse;
}

vec3 LightComponent::GetSpecular() const {
    return specular;
}

float LightComponent::GetConstant() const {
    return constant;
}

float LightComponent::GetLinear() const {
    return linear;
}

float LightComponent::GetQuadratic() const {
    return quadratic;
}

float LightComponent::GetRadius() const {
    return radius;
}

glm::vec3 LightComponent::GetDirection() const {
    return direction;
}

float LightComponent::GetIntensity() const {
    return intensity;
}

void LightComponent::SetAmbient(const vec3& ambient) {
    this->ambient = ambient;
}

void LightComponent::SetDiffuse(const vec3& diffuse) {
    this->diffuse = diffuse;
}

void LightComponent::SetSpecular(const vec3& specular) {
    this->specular = specular;
}

void LightComponent::SetConstant(float constant) {
    this->constant = constant;
}

void LightComponent::SetLinear(float linear) {
    this->linear = linear;
}

void LightComponent::SetQuadratic(float quadratic) {
    this->quadratic = quadratic;
}

void LightComponent::SetRadius(float radius) {
    this->radius = radius;
}

void LightComponent::SetDirection(const glm::vec3& direction) {
	this->direction = direction;
}

void LightComponent::SetIntensity(float intensity) {
    this->intensity = intensity;
}

void LightComponent::UpdatePointLight() {
    pointLight.position = owner->GetComponent<Transform_Component>()->GetPosition();
	pointLight.ambient = ambient;
	pointLight.diffuse = diffuse;
	pointLight.specular = specular;
	pointLight.constant = constant;
	pointLight.linear = linear;
	pointLight.quadratic = quadratic;
	pointLight.radius = radius;
	pointLight.intensity = intensity;
}

void LightComponent::UpdateDirectionalLight() {
	dirLight.direction = owner->GetComponent<Transform_Component>()->GetForward();
	dirLight.ambient = ambient;
	dirLight.diffuse = diffuse;
	dirLight.specular = specular;
	dirLight.intensity = intensity;
}