#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include <chrono>
#include <vector>
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

class ParticlesEmitterComponent : public Component{
public:
    ParticlesEmitterComponent(GameObject* owner);
    ~ParticlesEmitterComponent();
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    void EmitParticle();
	
    std::unique_ptr<Component> Clone(GameObject* owner) override;
    ComponentType GetType() const override { return ComponentType::PARTICLES_EMITTER; }
    

private:
    glm::vec3 position;
	Particle* emitterParticle = nullptr;
	int maxParticles = 100;
    float deltaTime;
	// Variables para el spawn de part�culas
    float spawnRate = 1.0f; 
    std::chrono::time_point<std::chrono::steady_clock> lastSpawnTime;
    std::vector<Particle> particles; // Vector para almacenar las part�culas

    Particle* SetParticleVariables(Particle* variablesParticle);
};;