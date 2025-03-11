#include "UICanvasComponent.h"

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UIGameView.h"


#include "UIImageComponent.h"
#include "UIButtonComponent.h"
#include "UITransformComponent.h"
#include "UISliderComponent.h"

#include <SDL2/SDL.h>


UICanvasComponent::UICanvasComponent(GameObject* owner)	: Component(owner)
{
	name = "UICanvasComponent";
}

void UICanvasComponent::Start()
{
    SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	monitorWidth = dm.w;
	monitorHeight = dm.h;
}

void UICanvasComponent::Update(float deltaTime)
{

	glm::vec3 canvasPos(owner->GetComponent<UITransformComponent>()->GetPosition().x * Application->gui->UIGameViewPanel->GetWidth(),
		owner->GetComponent<UITransformComponent>()->GetPosition().y * Application->gui->UIGameViewPanel->GetWidth(),
		0.0f);

	glm::vec3 canvasSize(Application->gui->UIGameViewPanel->GetWidth() * owner->GetComponent<UITransformComponent>()->GetScale().x
		, Application->gui->UIGameViewPanel->GetHeight() * owner->GetComponent<UITransformComponent>()->GetScale().y
		, 0.0f);

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(Application->gui->UIGameViewPanel->GetWidth()),
		static_cast<float>(Application->gui->UIGameViewPanel->GetHeight()), 0.0f,
		-1.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);

    for (size_t i = 0; i < owner->GetChildren().size(); ++i) {
        GameObject* object = owner->GetChildren()[i].get();

		auto rectTransform = object->GetComponent<UITransformComponent>();

		rectTransform->SetCanvasPosition(canvasPos);
		rectTransform->SetCanvasSize(canvasSize);

		if (object->HasComponent<UIImageComponent>())
		{
			object->GetComponent<UIImageComponent>()->SetProjection(projection);
			object->GetComponent<UIImageComponent>()->Update(deltaTime);
		}

		if (object->HasComponent<UIButtonComponent>())
		{
			object->GetComponent<UIButtonComponent>()->Update(deltaTime);
		}

		if (object->HasComponent<UISliderComponent>())
		{
			object->GetComponent<UISliderComponent>()->SetProjection(projection);
			object->GetComponent<UISliderComponent>()->Update(deltaTime);
		}

    }

    glEnable(GL_DEPTH_TEST);
}

void UICanvasComponent::Destroy()
{

}

std::unique_ptr<Component> UICanvasComponent::Clone(GameObject* owner)
{
	return std::make_unique<UICanvasComponent>(owner);
}