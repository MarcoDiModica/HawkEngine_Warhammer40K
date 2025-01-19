#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include <chrono>

class ParticlesEmitterComponent {
public:
    ParticlesEmitterComponent();
    ~ParticlesEmitterComponent();
    void Update();
    void Start();
	Particle* SetParticleVariables(Particle* particle);
    glm::vec3 GetPosition() const;

private:
    glm::vec3 position;
	Particle* emitterParticle = nullptr;
	int maxParticles = 100;

	// Variables para el spawn de partículas
    float spawnRate = 1.0f; 
    std::chrono::time_point<std::chrono::steady_clock> lastSpawnTime;
};;