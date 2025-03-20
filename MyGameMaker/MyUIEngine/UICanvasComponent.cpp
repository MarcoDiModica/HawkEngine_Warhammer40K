#include "UICanvasComponent.h"

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UIGameView.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"

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
#ifndef _BUILD
	int width = Application->gui->UIGameViewPanel->GetWidth();
	int height = Application->gui->UIGameViewPanel->GetHeight();
#else
	int width = Application->window->width();
	int height = Application->window->height();
#endif // !_BUILD

	

	glm::vec3 canvasPos(owner->GetComponent<UITransformComponent>()->GetPosition().x * width,
		owner->GetComponent<UITransformComponent>()->GetPosition().y * height,
		0.0f);

	glm::vec3 canvasSize(width * owner->GetComponent<UITransformComponent>()->GetScale().x
		, height * owner->GetComponent<UITransformComponent>()->GetScale().y
		, 0.0f);

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(width),
		static_cast<float>(height), 0.0f,
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

MonoObject* UICanvasComponent::GetSharp()
{
	if (CsharpReference) {
		return CsharpReference;
	}

	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "UICanvas");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.UICanvas:.ctor(uintptr,HawkEngine.GameObject)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	if (!method)
	{
		return nullptr;
	}

	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner->GetSharp();
	if (!ownerGo)
	{
		return nullptr;
	}

	void* args[2];
	args[0] = &componentPtr;
	args[1] = ownerGo;

	mono_runtime_invoke(method, monoObject, args, nullptr);

	CsharpReference = monoObject;
	return CsharpReference;
}