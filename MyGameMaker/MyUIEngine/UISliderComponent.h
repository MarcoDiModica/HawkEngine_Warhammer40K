#pragma once

#include "../MyGameEngine/Component.h"

class UISliderComponent :  public Component
{
public:
	explicit UISliderComponent(GameObject* owner);
	~UISliderComponent() override = default;

	UISliderComponent(const UISliderComponent&) = delete;
	UISliderComponent& operator=(const UISliderComponent&) = delete;

	UISliderComponent(UISliderComponent&&) noexcept = default;
	UISliderComponent& operator=(UISliderComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::SLIDER; }
};

