#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/types.h"

class ShaderComponent {
public:
    ShaderComponent();
    ~ShaderComponent();

    void SetShader(Shaders* newShader);
    Shaders* GetShader() const;

    void SetType(ShaderType newType);
    ShaderType GetType() const;

    void SetOwnerMaterial(Material* newOwnerMaterial);
    Material* GetOwnerMaterial() const;

private:
    Shaders* shader;
    ShaderType type;
    Material* ownerMaterial;
};