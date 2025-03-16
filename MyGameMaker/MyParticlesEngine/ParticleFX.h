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

	void RenderGameView();

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

		node["name"] = name;

		node["emitterShape"] = static_cast<int>(emitterShape);
		node["emissionRate"] = emissionRate;
		node["particlesPerSecond"] = particlesPerSecond;

		node["minLifetime"] = minLifetime;
		node["maxLifetime"] = maxLifetime;
		node["minSpeed"] = minSpeed;
		node["maxSpeed"] = maxSpeed;
		node["startSize"] = startSize;
		node["endSize"] = endSize;

		node["startColor"] = YAML::Node();
		node["startColor"].push_back(startColor.r);
		node["startColor"].push_back(startColor.g);
		node["startColor"].push_back(startColor.b);

		node["endColor"] = YAML::Node();
		node["endColor"].push_back(endColor.r);
		node["endColor"].push_back(endColor.g);
		node["endColor"].push_back(endColor.b);

		node["startAlpha"] = startAlpha;
		node["endAlpha"] = endAlpha;

		node["rotationSpeed"] = rotationSpeed;
		node["gravity"] = gravity;

		node["shapeParam1"] = shapeParam1;
		node["shapeParam2"] = shapeParam2;
		node["shapeParam3"] = shapeParam3;

		node["isOneShot"] = isOneShot;

		if (material) {
			node["material"] = YAML::Node();
			node["material"]["billboardType"] = material->GetBillboardType();
			node["material"]["particleType"] = material->GetParticleType();
			node["material"]["softness"] = material->GetSoftness();
			node["material"]["useColorGradient"] = material->GetUseColorGradient();

			if (material->getImg()) {
				node["material"]["texturePath"] = material->getImg()->image_path;
			}

			if (material->GetColorGradientMap()) {
				node["material"]["gradientPath"] = material->GetColorGradientMap()->image_path;
			}

			if (material->GetBillboardType() == 2) {
				auto axis = material->GetBillboardAxis();
				node["material"]["billboardAxis"] = YAML::Node();
				node["material"]["billboardAxis"].push_back(axis.x);
				node["material"]["billboardAxis"].push_back(axis.y);
				node["material"]["billboardAxis"].push_back(axis.z);
			}
		}

		return node;
	}

	bool decode(const YAML::Node& node) override
	{
		if (!Component::decode(node)) {
			return false;
		}

		try {
			if (node["name"]) {
				name = node["name"].as<std::string>();
			}

			if (node["emitterShape"]) {
				emitterShape = static_cast<EmitterShape>(node["emitterShape"].as<int>());
			}
			if (node["emissionRate"]) {
				emissionRate = node["emissionRate"].as<float>();
			}
			if (node["particlesPerSecond"]) {
				particlesPerSecond = node["particlesPerSecond"].as<float>();
			}

			if (node["minLifetime"]) {
				minLifetime = node["minLifetime"].as<float>();
			}
			if (node["maxLifetime"]) {
				maxLifetime = node["maxLifetime"].as<float>();
			}
			if (node["minSpeed"]) {
				minSpeed = node["minSpeed"].as<float>();
			}
			if (node["maxSpeed"]) {
				maxSpeed = node["maxSpeed"].as<float>();
			}
			if (node["startSize"]) {
				startSize = node["startSize"].as<float>();
			}
			if (node["endSize"]) {
				endSize = node["endSize"].as<float>();
			}

			if (node["startColor"] && node["startColor"].IsSequence() && node["startColor"].size() == 3) {
				startColor.r = node["startColor"][0].as<float>();
				startColor.g = node["startColor"][1].as<float>();
				startColor.b = node["startColor"][2].as<float>();
			}

			if (node["endColor"] && node["endColor"].IsSequence() && node["endColor"].size() == 3) {
				endColor.r = node["endColor"][0].as<float>();
				endColor.g = node["endColor"][1].as<float>();
				endColor.b = node["endColor"][2].as<float>();
			}

			if (node["startAlpha"]) {
				startAlpha = node["startAlpha"].as<float>();
			}
			if (node["endAlpha"]) {
				endAlpha = node["endAlpha"].as<float>();
			}

			if (node["rotationSpeed"]) {
				rotationSpeed = node["rotationSpeed"].as<float>();
			}
			if (node["gravity"]) {
				gravity = node["gravity"].as<float>();
			}

			if (node["shapeParam1"]) {
				shapeParam1 = node["shapeParam1"].as<float>();
			}
			if (node["shapeParam2"]) {
				shapeParam2 = node["shapeParam2"].as<float>();
			}
			if (node["shapeParam3"]) {
				shapeParam3 = node["shapeParam3"].as<float>();
			}

			if (node["isOneShot"]) {
				isOneShot = node["isOneShot"].as<bool>();
			}

			if (node["material"]) {
				if (!material) {
					material = std::make_shared<ParticleMaterial>();
				}

				if (node["material"]["billboardType"]) {
					material->SetBillboardType(node["material"]["billboardType"].as<int>());
				}

				if (node["material"]["particleType"]) {
					material->SetParticleType(static_cast<ParticleType>(node["material"]["particleType"].as<int>()));
				}

				if (node["material"]["softness"]) {
					material->SetSoftness(node["material"]["softness"].as<float>());
				}

				if (node["material"]["texturePath"]) {
					std::string texturePath = node["material"]["texturePath"].as<std::string>();
					if (!texturePath.empty()) {
						SetTexture(texturePath);
					}
				}

				if (node["material"]["gradientPath"]) {
					std::string gradientPath = node["material"]["gradientPath"].as<std::string>();
					if (!gradientPath.empty()) {
						SetColorGradient(gradientPath);
					}
				}
				else if (node["material"]["useColorGradient"] && !node["material"]["useColorGradient"].as<bool>()) {
					material->DisableColorGradient();
				}

				if (node["material"]["billboardAxis"] && node["material"]["billboardAxis"].IsSequence()
					&& node["material"]["billboardAxis"].size() == 3) {
					glm::vec3 axis(
						node["material"]["billboardAxis"][0].as<float>(),
						node["material"]["billboardAxis"][1].as<float>(),
						node["material"]["billboardAxis"][2].as<float>()
					);
					material->SetBillboardAxis(axis);
				}
			}

			return true;
		}
		catch (const YAML::Exception& e) {
			LOG(LogType::LOG_ERROR, "YAML Exception in ParticleFX::decode: %s", e.what());
			return false;
		}
	}
};

//cosas a mejorar: Sub-emisores, Movimiento basado en ruido, Efectos de estela/cinta, particula suaves

//Simulación en GPU Mueve más cálculos al GPU usando compute shaders Aumentará significativamente el número de partículas posibles