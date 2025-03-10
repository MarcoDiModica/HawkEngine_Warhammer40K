#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"

#include <string>

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
	const std::string& GetImagePath() const { return texturePath; }

	void SetTexture(std::string path);

	std::shared_ptr<Image> GetTexture() const { return texture; }

	void LoadMesh();

	void SetProjection(const glm::mat4& proj) { projection = proj; }

	glm::vec2 GetImageSize() const { return glm::vec2(texture->width(), texture->height()); }

private:
	//texture
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	Shaders * shader;
	glm::mat4 projection;
	bool resized = false;
};

