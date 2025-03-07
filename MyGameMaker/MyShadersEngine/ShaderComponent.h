#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/types.h"

class ShaderComponent : public Component {
public:
	explicit ShaderComponent(GameObject* owner);
	~ShaderComponent() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	void SetShaderType(ShaderType type);
	ShaderType GetShaderType() const;

	void SetOwnerMaterial(Material* material);
	Material* GetOwnerMaterial() const;

	void SetFloat(const std::string& name, float value);
	void SetVec3(const std::string& name, const glm::vec3& value);
	void SetVec4(const std::string& name, const glm::vec4& value);
	void SetMatrix(const std::string& name, const glm::mat4& value);

	// Shader-specific properties that can be exposed to the editor
	// These will be moved to shader-specific components in the future
	float frequency = 2.0f;
	float amplitude = 0.2f;

	ComponentType GetType() const override { return ComponentType::SHADER; }

private:
	ShaderType shaderType;
	Material* ownerMaterial;
};