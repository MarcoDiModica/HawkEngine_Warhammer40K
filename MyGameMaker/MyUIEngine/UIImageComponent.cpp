#include "UIImageComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"

UIImageComponent::UIImageComponent(GameObject* owner) : Component(owner)
{
	name = "UICanvasComponent";
}

void UIImageComponent::Start()
{

}

void UIImageComponent::Update(float deltaTime)
{

}

void UIImageComponent::Destroy()
{

}

std::unique_ptr<Component> UIImageComponent::Clone(GameObject* owner)
{
	return std::make_unique<UIImageComponent>(owner);
}
