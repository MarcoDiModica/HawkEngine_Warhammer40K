#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UITransformComponent : public Component
{
public:
	explicit UITransformComponent(GameObject* owner);
	~UITransformComponent() override = default;

	UITransformComponent(const UITransformComponent&) = delete;
	UITransformComponent& operator=(const UITransformComponent&) = delete;

	UITransformComponent(UITransformComponent&&) noexcept = default;
	UITransformComponent& operator=(UITransformComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::UITRANSFORM; }

private:
	//posicion, rotacion, escala
};

