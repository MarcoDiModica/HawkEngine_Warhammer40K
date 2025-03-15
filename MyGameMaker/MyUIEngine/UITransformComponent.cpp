#include "UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"

UITransformComponent::UITransformComponent(GameObject* owner) : Component(owner)
{
	name = "UITransformComponent";
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void UITransformComponent::Start()
{

}

void UITransformComponent::Update(float deltaTime)
{
	
}

void UITransformComponent::Destroy()
{

}

std::unique_ptr<Component> UITransformComponent::Clone(GameObject* owner)
{
	return std::make_unique<UITransformComponent>(owner);
}

void UITransformComponent::Translate(const glm::dvec3& translation)
{
	position += translation;
}

void UITransformComponent::Rotate(double rads, const glm::dvec3& axis)
{
	rotation += glm::degrees(glm::eulerAngles(glm::quat(rads, axis)));
}

void UITransformComponent::Scale(const glm::dvec3& scale)
{
    if (scale.x != 0.0f && scale.y != 0.0f && scale.z != 0.0f) {
        this->scale *= scale;
    }
}

void UITransformComponent::SetTransform(const glm::vec3& position, const glm::vec3& scale)
{
	this->scale = scale;
	this->position = position;
	resized = true;
}