#include "UIButtonComponent.h"

#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Input.h"

UIButtonComponent::UIButtonComponent(GameObject* owner) : Component(owner)
{
	name = "UIButtonComponent";
}

void UIButtonComponent::Start()
{
}

void UIButtonComponent::Update(float deltaTime)
{
	auto width = owner->GetComponent<UIImageComponent>()->GetTexture()->width();
	auto height = owner->GetComponent<UIImageComponent>()->GetTexture()->height();

	auto Tposition = owner->GetComponent<UITransformComponent>()->GetPosition();
	auto Tscale = owner->GetComponent<UITransformComponent>()->GetScale();

	auto mousePosX = Application->input->GetMouseX();
	auto mousePosY = Application->input->GetMouseY();

	auto offsetX = 7.0f;
	auto offsetY = 56.0f;

	if (mousePosX >= Tposition.x + offsetX && mousePosX <= Tposition.x + offsetX + width * Tscale.x &&
		mousePosY >= Tposition.y + offsetY && mousePosY <= Tposition.y + offsetY + height * Tscale.y)
	{
		//std::cout << "Mouse is over the button" << std::endl;
	}

	//std::cout << "Mouse position: " << mousePosX << ", " << mousePosY << std::endl;
}

void UIButtonComponent::Destroy()
{
}

std::unique_ptr<Component> UIButtonComponent::Clone(GameObject* owner)
{
	return std::make_unique<UIButtonComponent>(owner);
}
