#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UICanvasComponent : public Component
{
public:
	explicit UICanvasComponent(GameObject* owner);
	~UICanvasComponent() override = default;

	UICanvasComponent(const UICanvasComponent&) = delete;
	UICanvasComponent& operator=(const UICanvasComponent&) = delete;

	UICanvasComponent(UICanvasComponent&&) noexcept = default;
	UICanvasComponent& operator=(UICanvasComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::CANVAS; }

private:
	//Render Mode: screenspace - overlay
	GLuint fboUI = -1;
	GLuint fboTextureUI = 0;
	GLuint rboUI = 0;
};