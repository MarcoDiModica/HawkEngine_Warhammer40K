#pragma once
#include <chrono>
#include <vector>
#include <memory>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ParticleData.h"
#include "ParticleMaterial.h"
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

enum class EmitterShape {
	POINT,      // Particles emit from a single point
	SPHERE,     // Particles emit in all directions from a sphere
	CONE,       // Particles emit in a cone
	BOX,        // Particles emit from a box volume
	CIRCLE      // Particles emit from a circle surface
};

struct ParticlePreset {
	ParticleType type;
	glm::vec3 colorStart;
	glm::vec3 colorEnd;
	float alphaStart;
	float alphaEnd;
	float sizeStart;
	float sizeEnd;
	float minLifetime;
	float maxLifetime;
	float minSpeed;
	float maxSpeed;
	float gravity;
	float rotationSpeed;
	float emissionRate;
	EmitterShape shape;
	float shapeParam1;  // Radius for sphere/cone/circle, width for box
	float shapeParam2;  // Height for cone/box
	float shapeParam3;  // Depth for box, angle for cone
};

namespace ParticlePresets {
	extern const ParticlePreset Smoke;
	extern const ParticlePreset Fire;
	extern const ParticlePreset MuzzleFlash;
	extern const ParticlePreset Dust;
}

class ParticleFX : public Component {
public:
	explicit ParticleFX(GameObject* owner);
	~ParticleFX() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;
	ComponentType GetType() const override { return ComponentType::PARTICLES_EMITTER; }

	void SetTexture(const std::string& texturePath);

	void SetColorGradient(const std::string& texturePath);

	void ApplyPreset(const ParticlePreset& preset);

	void EmitBurst(int count);

	void Play();
	void Stop();
	void Pause();
	bool IsPlaying() const;

	void SetEmitterShape(EmitterShape shape);
	void SetEmissionRate(float particlesPerSecond);
	void SetParticleLifetime(float min, float max);
	void SetParticleSpeed(float min, float max);
	void SetParticleSize(float startSize, float endSize);
	void SetParticleColor(const glm::vec3& startColor, const glm::vec3& endColor);
	void SetParticleAlpha(float startAlpha, float endAlpha);
	void SetParticleRotation(float rotationSpeed);
	void SetGravity(float gravity);
	void SetBillboardType(int billboardType);
	void SetShapeParameters(float param1, float param2 = 0.0f, float param3 = 0.0f);
	void SetOneShot(bool oneShot);
	void SetSoftness(float value) { if (material) material->SetSoftness(value); }
	void SetParticleType(ParticleType type) { if (material) material->SetParticleType(type); }
	void DisableColorGradient() { if (material) material->DisableColorGradient(); }

	float GetEmissionRate() const { return emissionRate; }
	float GetMinLifetime() const { return minLifetime; }
	float GetMaxLifetime() const { return maxLifetime; }
	float GetMinSpeed() const { return minSpeed; }
	float GetMaxSpeed() const { return maxSpeed; }
	float GetStartSize() const { return startSize; }
	float GetEndSize() const { return endSize; }
	glm::vec3 GetStartColor() const { return startColor; }
	glm::vec3 GetEndColor() const { return endColor; }
	float GetStartAlpha() const { return startAlpha; }
	float GetEndAlpha() const { return endAlpha; }
	float GetRotationSpeed() const { return rotationSpeed; }
	float GetGravity() const { return gravity; }
	int GetBillboardType() const { return material ? material->GetBillboardType() : 0; }
	EmitterShape GetEmitterShape() const { return emitterShape; }
	ParticleType GetParticleType() const { return material ? static_cast<ParticleType>(material->GetParticleType()) : ParticleType::DEFAULT; }
	float GetSoftness() const { return material ? material->GetSoftness() : 0.0f; }
	float GetShapeParam1() const { return shapeParam1; }
	float GetShapeParam2() const { return shapeParam2; }
	float GetShapeParam3() const { return shapeParam3; }
	bool IsOneShot() const { return isOneShot; }
	std::string GetTexturePath() const { return material && material->getImg() ? material->getImg()->image_path : ""; }
	std::string GetGradientPath() const { return material && material->GetColorGradientMap() ? material->GetColorGradientMap()->image_path : ""; }
	int GetMaxParticles() const { return renderer ? static_cast<int>(renderer->GetMaxParticles()) : 0; }

	void ConfigureSmoke();
	void ConfigureFire();
	void ConfigureMuzzleFlash();
	void ConfigureDust();

private:
	void EmitParticle();

	glm::vec3 GenerateRandomPosition();

	glm::vec3 GenerateRandomVelocity();

	std::shared_ptr<ParticleMaterial> material;
	std::unique_ptr<ParticleInstancedRenderer> renderer;

	// Emitter configurations
	EmitterShape emitterShape;
	float emissionRate;       
	float particlesPerSecond; 
	float timeSinceLastEmit;

	// Particle configurations
	float minLifetime, maxLifetime;
	float minSpeed, maxSpeed;
	float startSize, endSize;
	glm::vec3 startColor, endColor;
	float startAlpha, endAlpha;
	float rotationSpeed;
	float gravity;

	// Shape parameters
	float shapeParam1;  // Radius for sphere/cone/circle, width for box
	float shapeParam2;  // Height for cone/box
	float shapeParam3;  // Depth for box, angle for cone

	// Control flags
	bool isPlaying;
	bool isPaused;
	bool isOneShot;
	bool burstEmitted;

	// Random number generator
	std::mt19937 rng;
	std::uniform_real_distribution<float> dist01;  // 0-1 distribution

	// Transform cache
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

protected:
	friend class SceneSerializer;

	YAML::Node encode() override
	{
		YAML::Node node = Component::encode();
		return node;
	}

	bool decode(const YAML::Node& node) override
	{
		return true;
	}
};