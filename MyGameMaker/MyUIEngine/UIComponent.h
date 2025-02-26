#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class UIComponent : public Component
{
public:
	explicit UIComponent(GameObject* owner);
	~UIComponent() override = default;

	UIComponent(const UIComponent&) = delete;
	UIComponent& operator=(const UIComponent&) = delete;

	UIComponent(UIComponent&&) noexcept = default;
	UIComponent& operator=(UIComponent&&) noexcept = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;
	
	glm::vec3 GetPosition() const;
	void SetTexture(const std::string& texturePath);
	std::string GetTexture() const;

private:
	std::string texturePath;
	float deltaTime;
	glm::vec3 position;
	glm::dquat rotation;
	glm::vec3 scale;

};

