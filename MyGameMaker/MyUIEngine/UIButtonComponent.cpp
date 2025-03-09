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
	auto rectTransform = owner->GetComponent<UITransformComponent>();

	auto buttonSize = rectTransform->getCanvasSize() * rectTransform->GetScale();
	auto buttonPos = rectTransform->getCanvasPosition() + (rectTransform->GetPosition() * rectTransform->getCanvasSize());

	auto mousePosX = Application->input->GetMouseX();
	auto mousePosY = Application->input->GetMouseY();

	auto offsetX = 7.0f;
	auto offsetY = 56.0f;

	if (mousePosX >= buttonPos.x + offsetX && mousePosX <= buttonPos.x + offsetX + buttonSize.x &&
		mousePosY >= buttonPos.y + offsetY && mousePosY <= buttonPos.y + offsetY + buttonSize.y)
	{
		std::cout << "Mouse is over the button" << std::endl;
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
