#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UIImageComponent : public Component
{
public:
	explicit UIImageComponent(GameObject* owner);
	~UIImageComponent() override = default;

	UIImageComponent(const UIImageComponent&) = delete;
	UIImageComponent& operator=(const UIImageComponent&) = delete;

	UIImageComponent(UIImageComponent&&) noexcept = default;
	UIImageComponent& operator=(UIImageComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::IMAGE; }

private:
	//texture
};

