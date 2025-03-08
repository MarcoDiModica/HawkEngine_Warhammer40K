#include "UICanvasComponent.h"

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"

#include "UIImageComponent.h"
#include "UIButtonComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>


UICanvasComponent::UICanvasComponent(GameObject* owner)	: Component(owner)
{
	name = "UICanvasComponent";
}

void UICanvasComponent::Start()
{
    SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	winWidth = dm.w;
	winHeight = dm.h;
}

void UICanvasComponent::Update(float deltaTime)
{

    glDisable(GL_DEPTH_TEST);

    for (size_t i = 0; i < owner->GetChildren().size(); ++i) {
        GameObject* object = owner->GetChildren()[i].get();

		if (object->HasComponent<UIImageComponent>())
		{
			object->GetComponent<UIImageComponent>()->Update(deltaTime);
		}

		if (object->HasComponent<UIButtonComponent>())
		{
			object->GetComponent<UIButtonComponent>()->Update(deltaTime);
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