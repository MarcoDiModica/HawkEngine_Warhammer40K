#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

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
};

