#pragma once
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/types.h"
#include "../MyGameEngine/Shaders.h"
#include "../MyGameEngine/Material.h"
#include <memory>
#include <string>

class Material;

class ShaderComponent : public Component {
public:
	explicit ShaderComponent(GameObject* owner);
	~ShaderComponent() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	ComponentType GetType() const override { return ComponentType::SHADER; }

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	void SetShaderType(ShaderType type);
	ShaderType GetShaderType() const;

	bool LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

	void SetOwnerMaterial(Material* material);

private:
	void LoadDefaultShaders();

	ShaderType shaderType;
	std::unique_ptr<Shaders> shaders;
	Material* ownerMaterial;
};