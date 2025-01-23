#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/types.h"

class ShaderComponent : public Component{
public:
    ShaderComponent(GameObject* owner);
    ~ShaderComponent();

    void SetShader(Shaders* newShader);
    Shaders* GetShader() const;

    void SetShaderType(ShaderType newType);
    ShaderType GetShaderType() const;

    void SetOwnerMaterial(Material* newOwnerMaterial);
    Material* GetOwnerMaterial() const;

private:
    Shaders* shader;
    ShaderType type;
    Material* ownerMaterial;
};