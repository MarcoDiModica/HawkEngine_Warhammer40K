#include "UISliderComponent.h"

#include "UITransformComponent.h"
#include "UIImageComponent.h"

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Root.h"

UISliderComponent::UISliderComponent(GameObject* owner) : Component(owner)
{
	name = "UISliderComponent";
}

void UISliderComponent::Awake()
{
}

void UISliderComponent::Start()
{
}

void UISliderComponent::Update(float deltaTime)
{
	auto uiSliderTransform = owner->GetComponent<UITransformComponent>();
	auto sliderButton = owner->GetChildren()[0].get();

	if (sliderButton->HasComponent<UIImageComponent>())
	{
		auto uiButtonTransform = sliderButton->GetComponent<UITransformComponent>();
		uiButtonTransform->SetCanvasPosition(uiSliderTransform->GetCanvasPosition());
		uiButtonTransform->SetCanvasSize(uiSliderTransform->GetCanvasSize());

		auto uiButtonPos = uiButtonTransform->GetPosition();
		uiButtonTransform->setPos(glm::vec3(uiSliderTransform->GetPosition().x + (uiSliderTransform->GetPosition().x * uiSliderTransform->GetScale().x),
			uiSliderTransform->GetPosition().y, uiSliderTransform->GetPosition().z));

		auto uiImage = sliderButton->GetComponent<UIImageComponent>();

		uiImage->SetProjection(projection);
		uiImage->Update(deltaTime);
	}
}

void UISliderComponent::Destroy()
{
}

std::unique_ptr<Component> UISliderComponent::Clone(GameObject* owner)
{
	return std::make_unique<UISliderComponent>(owner);
}

void UISliderComponent::SetTexture(std::string path)
{
	/*texture = std::make_shared<Image>();
	texture->LoadTexture(path);
	shader = &Application->root->shaders[0];
	LoadMesh();*/
}

void UISliderComponent::LoadMesh()
{
	mesh = Mesh::CreatePlane();
}