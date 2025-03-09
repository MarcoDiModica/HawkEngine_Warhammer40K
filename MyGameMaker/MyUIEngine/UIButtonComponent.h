#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

enum class ButtonState {
	DEFAULT,
	HOVERED,
	CLICKED
};

class UIButtonComponent : public Component
{
public:
	explicit UIButtonComponent(GameObject* owner);
	~UIButtonComponent() override = default;

	UIButtonComponent(const UIButtonComponent&) = delete;
	UIButtonComponent& operator=(const UIButtonComponent&) = delete;

	UIButtonComponent(UIButtonComponent&&) noexcept = default;
	UIButtonComponent& operator=(UIButtonComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::BUTTON; }
	
	ButtonState GetState() const { return state; }
	void SetState(ButtonState newState) { state = newState; }

private:
	ButtonState state = ButtonState::DEFAULT;

};

