#include "UIButtonComponent.h"

#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Input.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyGameEditor/Input.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"

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
	auto imageComponent = owner->GetComponent<UIImageComponent>();

	auto buttonSize = rectTransform->GetCanvasSize() * rectTransform->GetScale();
	auto buttonPos = rectTransform->GetCanvasPosition() + (rectTransform->GetPosition() * rectTransform->GetCanvasSize());

	buttonPos -= rectTransform->GetPivotOffset() * buttonSize;

	auto mousePosX = Application->input->GetMouseX();
	auto mousePosY = Application->input->GetMouseY();

#ifndef _BUILD
	double offsetX = Application->gui->UIGameViewPanel->GetViewportPos().x;
	double offsetY = Application->gui->UIGameViewPanel->GetViewportPos().y;
#else
	double offsetX = 0;
	double offsetY = 0;
#endif // !_BUILD

	

	if (GetState() == ButtonState::DISABLED) {
		imageComponent->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		return;
	}

    if (mousePosX >= buttonPos.x + offsetX && mousePosX <= buttonPos.x + offsetX + buttonSize.x &&
        mousePosY >= buttonPos.y + offsetY && mousePosY <= buttonPos.y + offsetY + buttonSize.y)
    {

		SetState(ButtonState::HOVERED);
		imageComponent->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		if (Application->input->GetMouseButton(1) == KEY_STATE::KEY_REPEAT)
        {
			SetState(ButtonState::PRESSED);
            imageComponent->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else if (Application->input->GetMouseButton(1) == KEY_STATE::KEY_UP)
		{
			SetState(ButtonState::CLICKED);
			imageComponent->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
    }
    else
    {
        SetState(ButtonState::DEFAULT);
		imageComponent->SetColor(glm::vec4(1.0f));
    }
}

void UIButtonComponent::Destroy()
{
}

std::unique_ptr<Component> UIButtonComponent::Clone(GameObject* owner)
{
	return std::make_unique<UIButtonComponent>(owner);
}

MonoObject* UIButtonComponent::GetSharp()
{
	if (CsharpReference) {
		return CsharpReference;
	}

	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "UIButton");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.UIButton:.ctor(uintptr,HawkEngine.GameObject)", true);
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
