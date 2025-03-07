#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include <glm/glm.hpp>
#include <memory>

class Mesh;
class Material;
class Image;
class Shaders;

class MeshRenderer : public Component {
public:
	explicit MeshRenderer(GameObject* owner);
	~MeshRenderer() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	ComponentType GetType() const override { return ComponentType::MESH_RENDERER; }

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	void SetMesh(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh() const;

	void SetMaterial(std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial() const;

	void SetImage(std::shared_ptr<Image> image);

	void SetColor(const glm::vec3& color);
	glm::vec3 GetColor() const;

	void Render() const;
	void RenderMainCamera() const;

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Material> material = std::make_shared<Material>();
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	void RenderWithUnlitShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const;
	void RenderWithPBRShader(Shaders* shader, const glm::mat4& view, const glm::mat4& projection) const;
};