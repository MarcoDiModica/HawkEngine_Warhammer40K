#pragma once
#include "../MyGameEngine/Shaders.h"

class ParticleShader : public Shaders {
public:
	ParticleShader() {}

	ShaderType GetShaderType() const override { return ShaderType::PARTICLE; }

	bool Initialize() {
		return LoadShaders("Assets/Shaders/particle_vertex.glsl", "Assets/Shaders/particle_fragment.glsl");
	}

	void SetBillboardType(int type) {
		SetUniform("billboardType", type);
	}

	void SetCameraPosition(const glm::vec3& position) {
		SetUniform("cameraPosition", position);
	}

	void SetCameraUp(const glm::vec3& up) {
		SetUniform("cameraUp", up);
	}

	void SetBillboardAxis(const glm::vec3& axis) {
		SetUniform("billboardAxis", axis);
	}

	void SetParticleType(int type) {
		SetUniform("particleType", type);
	}

	void SetSoftness(float softness) {
		SetUniform("softness", softness);
	}

	void SetUseColorGradient(bool use) {
		SetUniform("useColorGradient", use ? 1 : 0);
	}
};