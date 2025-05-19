#include "UICanvasComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UIGameView.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "../MyGameEngine/ShaderManager.h"
#include "UIImageComponent.h"
#include "UIButtonComponent.h"
#include "UITransformComponent.h"
#include "UISliderComponent.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

UICanvasComponent::UICanvasComponent(GameObject* owner) : Component(owner)
{
	name = "UICanvasComponent";
}

void UICanvasComponent::Awake()
{
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
#endif

	glm::vec3 canvasPos(owner->GetComponent<UITransformComponent>()->GetPosition().x * width,
		owner->GetComponent<UITransformComponent>()->GetPosition().y * height,
		0.0f);

	glm::vec3 canvasSize(width * owner->GetComponent<UITransformComponent>()->GetScale().x,
		height * owner->GetComponent<UITransformComponent>()->GetScale().y,
		0.0f);

	for (size_t i = 0; i < owner->GetChildren().size(); ++i) {
		GameObject* object = owner->GetChildren()[i].get();

		if (!object->IsActive()) {
			continue;
		}

		auto rectTransform = object->GetComponent<UITransformComponent>();
		if (rectTransform) {
			rectTransform->SetCanvasPosition(canvasPos);
			rectTransform->SetCanvasSize(canvasSize);
		}
	}
}

void UICanvasComponent::Destroy()
{
}

std::unique_ptr<Component> UICanvasComponent::Clone(GameObject* owner)
{
	return std::make_unique<UICanvasComponent>(owner);
}

void UICanvasComponent::RenderCanvas(int viewportWidth, int viewportHeight)
{
	SetupUIRendering(viewportWidth, viewportHeight);

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(viewportWidth),
		static_cast<float>(viewportHeight), 0.0f,
		-1.0f, 1.0f
	);

	glm::mat4 view = glm::mat4(1.0f);

	for (const auto& child : owner->GetChildren()) {
		if (child && child->IsActive()) {
			RenderUIElement(child.get(), projection, view);
		}
	}

	RestoreRenderingState();
}

void UICanvasComponent::SetupUIRendering(int viewportWidth, int viewportHeight)
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	glGetBooleanv(GL_DEPTH_TEST, &lastDepthTest);
	glGetBooleanv(GL_CULL_FACE, &lastCullFace);
	glGetBooleanv(GL_BLEND, &lastBlend);
	glGetIntegerv(GL_BLEND_SRC_RGB, &lastBlendSrcRGB);
	glGetIntegerv(GL_BLEND_DST_RGB, &lastBlendDstRGB);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &lastBlendSrcAlpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &lastBlendDstAlpha);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UICanvasComponent::RestoreRenderingState()
{
	glUseProgram(lastProgram);
	if (lastDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (lastCullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (lastBlend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	glBlendFuncSeparate(lastBlendSrcRGB, lastBlendDstRGB, lastBlendSrcAlpha, lastBlendDstAlpha);
}

void UICanvasComponent::RenderUIElement(GameObject* element, const glm::mat4& projection, const glm::mat4& view)
{
	if (!element || !element->IsActive()) return;

	if (element->HasComponent<UIImageComponent>()) {
		auto* image = element->GetComponent<UIImageComponent>();
		image->RenderUI(projection, view);
	}

	for (const auto& child : element->GetChildren()) {
		RenderUIElement(child.get(), projection, view);
	}
}

MonoObject* UICanvasComponent::GetSharp()
{
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

	return monoObject;
}
