#include "UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"

UITransformComponent::UITransformComponent(GameObject* owner) : Component(owner)
{
	name = "UICanvasComponent";
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
