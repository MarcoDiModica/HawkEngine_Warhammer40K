#include "UIComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"

UIComponent::UIComponent(GameObject* owner)

	: Component(owner)
	, texturePath("../MyGameEditor/Assets/Textures/Default.png")
	, deltaTime(Application->GetDt())
	
{

	name = "UIComponent";
	position = owner->GetComponent<Transform_Component>()->GetPosition();
	rotation = owner->GetComponent<Transform_Component>()->GetRotation();
	scale = owner->GetComponent<Transform_Component>()->GetScale();
	
}

void UIComponent::Start()
{
	position = owner->GetComponent<Transform_Component>()->GetPosition();
}

void UIComponent::Update(float deltaTime)
{

}

void UIComponent::Destroy() 
{

}

std::unique_ptr<Component> UIComponent::Clone(GameObject* owner)
{
	return std::make_unique<UIComponent>(owner);
}

glm::vec3 UIComponent::GetPosition() const
{
	return position;
}

void UIComponent::SetTexture(const std::string& texturePath)
{
	this->texturePath = texturePath;
}

std::string UIComponent::GetTexture() const
{
	return texturePath;
}
