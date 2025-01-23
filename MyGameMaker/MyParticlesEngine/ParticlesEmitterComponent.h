#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include <chrono>
#include <vector>
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

enum class BillboardType
{
    SCREEN_ALIGNED,  // Aligned to the camera screen
    WORLD_ALIGNED,   // Aligned to the world
    AXIS_ALIGNED     // Aligned to a specific axis (typically the Y-axis)
};
class ParticlesEmitterComponent : public Component{
public:
    ParticlesEmitterComponent(GameObject* owner);
    ~ParticlesEmitterComponent();
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    void EmitParticle1(const glm::vec3& speed);
    void EmitParticle2(const glm::vec3& speed);
	
    std::unique_ptr<Component> Clone(GameObject* owner) override;
    ComponentType GetType() const override { return ComponentType::PARTICLES_EMITTER; }
    glm::vec3 GetPosition() const;
    void SetTexture(const std::string& texturePath);
    std::string GetTexture() const;


    float getSpawnRate() const;
    void setSpawnRate(float spawnRate);

    int getMaxParticles() const;
    void setMaxParticles(int maxParticles);
    bool isSmoking;

    glm::mat4 CalculateScreenAligned(const glm::vec3& particlePosition, const glm::vec3& cameraPosition);

    glm::mat4 CalculateWorldAligned(const glm::vec3& particlePosition, const glm::vec3& cameraPosition);

    glm::mat4 CalculateAxisAligned(const glm::vec3& cameraPosition, const glm::vec3& axis);

    void ApplyTransform(const glm::mat4& transformMatrix);

    BillboardType m_Type;
    BillboardType GetTypeEnum() const;
    void SetType(BillboardType type);

private:
    std::string texturePath;
    std::string texturePath2 = "../MyGameEditor/Assets/Textures/Firework.png";
    glm::vec3 position;
	glm::dquat rotation;
	glm::vec3 scale;
	Particle* emitterParticle = nullptr;
	int maxParticles = 10;
    float deltaTime;
    
	// Variables para el spawn de partículas
    float spawnRate = 1.0f; 
    std::chrono::time_point<std::chrono::steady_clock> lastSpawnTime;
    std::vector<Particle> particles; // Vector para almacenar las partículas

    Particle* SetParticleVariables(Particle* variablesParticle);
};;