#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/types.h"

class ShaderComponent : public Component {
public:
    ShaderComponent();
    ~ShaderComponent();

    void SetShader(Shaders * newShader);
	Shaders* GetShader() const;

	void SetType(ShaderType newType);
	ShaderType GetType() const;

	void SetOwnerMaterial(Material* newOwnerMaterial);
	Material* GetOwnerMaterial() const;

private:
    ShaderType type;
    Shaders* shader;
	Material* ownerMaterial;
};