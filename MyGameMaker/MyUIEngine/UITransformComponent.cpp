#include "UITransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"

UITransformComponent::UITransformComponent(GameObject* owner) : Component(owner)
{
	name = "UITransformComponent";
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void UITransformComponent::Start()
{

}

void UITransformComponent::Update(float deltaTime)
{
	
}

void UITransformComponent::Destroy()
{

}

std::unique_ptr<Component> UITransformComponent::Clone(GameObject* owner)
{
	return std::make_unique<UITransformComponent>(owner);
}

void UITransformComponent::Translate(const glm::dvec3& translation)
{
	position += translation;
}

void UITransformComponent::Rotate(double rads, const glm::dvec3& axis)
{
	rotation += glm::degrees(glm::eulerAngles(glm::quat(rads, axis)));
}

void UITransformComponent::Scale(const glm::dvec3& scale)
{
    if (scale.x != 0.0f && scale.y != 0.0f && scale.z != 0.0f) {
        this->scale *= scale;
    }
}

void UITransformComponent::SetTransform(const glm::vec3& position, const glm::vec3& scale)
{
	this->scale = scale;
	this->position = position;
	resized = true;
}

MonoObject* UITransformComponent::GetSharp()
{
	if (CsharpReference) {
		return CsharpReference;
	}

	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "UITransform");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.UITransform:.ctor(uintptr,HawkEngine.GameObject)", true);
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
