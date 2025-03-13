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

    void SetMetallic(float value);
    void SetRoughness(float value);
    void SetAO(float value);

    void SetNormalMap(const std::string& path);
    void SetMetallicMap(const std::string& path);
    void SetRoughnessMap(const std::string& path);
    void SetAOMap(const std::string& path);

    float GetMetallic() const;
    float GetRoughness() const;
    float GetAO() const;

    bool HasNormalMap() const;
    bool HasMetallicMap() const;
    bool HasRoughnessMap() const;
    bool HasAOMap() const;

    ComponentType GetType() const override { return ComponentType::SHADER; }

private:
    ShaderType shaderType;
    Material* ownerMaterial;

protected:
    friend class SceneSerializer;

    //todo encode decode functions
};