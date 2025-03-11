#include "UIButtonComponent.h"

#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Input.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UIGameView.h"

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

	auto buttonSize = rectTransform->GetCanvasSize() * rectTransform->GetScale();
	auto buttonPos = rectTransform->GetCanvasPosition() + (rectTransform->GetPosition() * rectTransform->GetCanvasSize());

	buttonPos -= rectTransform->GetPivotOffset() * buttonSize;

	auto mousePosX = Application->input->GetMouseX();
	auto mousePosY = Application->input->GetMouseY();

	double offsetX = Application->gui->UIGameViewPanel->GetViewportPos().x;
    double offsetY = Application->gui->UIGameViewPanel->GetViewportPos().y;

    if (mousePosX >= buttonPos.x + offsetX && mousePosX <= buttonPos.x + offsetX + buttonSize.x &&
        mousePosY >= buttonPos.y + offsetY && mousePosY <= buttonPos.y + offsetY + buttonSize.y)
    {
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (GetState() == ButtonState::HOVERED)
            {
                SetState(ButtonState::CLICKED);
                /*std::cout << "button is clicked" << std::endl;*/
            }
        }
        else
        {
            SetState(ButtonState::HOVERED);
            std::cout << "button is hovering" << std::endl;
        }
    }
    else
    {
        SetState(ButtonState::DEFAULT);
        /*std::cout << "button is default" << std::endl;*/
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
