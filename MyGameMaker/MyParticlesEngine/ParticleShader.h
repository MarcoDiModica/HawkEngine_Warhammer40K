#pragma once
#include "../MyGameEngine/Shaders.h"

class ParticleShader : public Shaders {
public:
	ParticleShader() {}

	ShaderType GetShaderType() const override { return ShaderType::PARTICLE; }

	bool Initialize() {
		return LoadShaders("Assets/Shaders/particle_vertex.glsl", "Assets/Shaders/particle_fragment.glsl");
	}

	void Bind() {
		Shaders::Bind();
	}

	void SetBillboardType(int type) {
		SetUniform("billboardType", type);
	}

	void SetCameraPosition(const glm::vec3& position) {
		SetUniformVec3("cameraPosition", position);
	}

	void SetCameraUp(const glm::vec3& up) {
		SetUniformVec3("cameraUp", up);
	}

	void SetBillboardAxis(const glm::vec3& axis) {
		SetUniformVec3("billboardAxis", axis);
	}

	void SetInstanceOffset(int offset) {
		SetUniform("instanceOffset", offset);
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