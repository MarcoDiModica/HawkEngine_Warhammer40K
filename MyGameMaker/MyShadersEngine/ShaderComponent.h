#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/types.h"

class ShaderComponent : public Component{
public:
    explicit ShaderComponent(GameObject* owner);
    ~ShaderComponent() override = default;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    std::unique_ptr<Component> Clone(GameObject* owner) override;

    void SetShader(Shaders* newShader);
    Shaders* GetShader() const;

    void SetShaderType(ShaderType newType);
    ShaderType GetShaderType() const;

    void SetOwnerMaterial(Material* newOwnerMaterial);
    Material* GetOwnerMaterial() const;

    ComponentType GetType() const override { return ComponentType::SHADER; }

	float frequency = 2.0f;
    float amplitude = 0.2f;

private:
    Shaders* shader;
    ShaderType type;
    Material* ownerMaterial;

protected:
	friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node = Component::encode();

		node["shaderType"] = static_cast<int>(type);

		node["frequency"] = frequency;
		node["amplitude"] = amplitude;

		return node;
	}

	bool decode(const YAML::Node& node) override {
		if (!Component::decode(node)) {
			return false;
		}

		if (node["frequency"]) {
			frequency = node["frequency"].as<float>();
		}

		if (node["amplitude"]) {
			amplitude = node["amplitude"].as<float>();
		}

		return true;
	}
};