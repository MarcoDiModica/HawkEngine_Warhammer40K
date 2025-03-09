#include "UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"

UITransformComponent::UITransformComponent(GameObject* owner) : Component(owner)
{
	name = "UITansformComponent";
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	position = glm::vec3(0.0f, 0.0f, 0.0f);
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

void UITransformComponent::SetTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    auto parent = owner->GetParent();
    if (parent)
    {
        auto parentTransform = parent->GetTransform();
        if (parentTransform)
        {
            // Ensure the position is within the parent's bounding box
            auto parentBoundingBox = parent->boundingBox();
            auto newPosition = position;

            if (newPosition.x < parentBoundingBox.v000().x) newPosition.x = parentBoundingBox.v000().x;
            if (newPosition.y < parentBoundingBox.v000().y) newPosition.y = parentBoundingBox.v000().y;
            if (newPosition.z < parentBoundingBox.v000().z) newPosition.z = parentBoundingBox.v000().z;

            if (newPosition.x > parentBoundingBox.v111().x) newPosition.x = parentBoundingBox.v111().x;
            if (newPosition.y > parentBoundingBox.v111().y) newPosition.y = parentBoundingBox.v111().y;
            if (newPosition.z > parentBoundingBox.v111().z) newPosition.z = parentBoundingBox.v111().z;

            owner->GetTransform()->SetPosition(newPosition);
            owner->GetTransform()->SetRotation(rotation);
            owner->GetTransform()->SetScale(scale);
        }
    }
    else
    {
        owner->GetTransform()->SetPosition(position);
        owner->GetTransform()->SetRotation(rotation);
        owner->GetTransform()->SetScale(scale);
    }
}