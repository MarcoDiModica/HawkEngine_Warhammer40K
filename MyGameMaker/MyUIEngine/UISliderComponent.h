#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"

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

	void SetTexture(std::string path);

	void LoadMesh();

	void SetProjection(const glm::mat4& proj) { projection = proj; }

private:
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	Shaders* shader;
	glm::mat4 projection;
	bool resized = false;
};

