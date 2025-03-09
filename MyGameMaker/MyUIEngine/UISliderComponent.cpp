#include "UISliderComponent.h"

UISliderComponent::UISliderComponent(GameObject* owner) : Component(owner)
{
	name = "UISliderComponent";
}

void UISliderComponent::Start()
{
}

void UISliderComponent::Update(float deltaTime)
{
}

void UISliderComponent::Destroy()
{
}

std::unique_ptr<Component> UISliderComponent::Clone(GameObject* owner)
{
	return std::make_unique<UISliderComponent>(owner);
}
