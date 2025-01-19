#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include <chrono>
#include <vector>

class ParticlesEmitterComponent {
public:
    ParticlesEmitterComponent();
    ~ParticlesEmitterComponent();
    void Update();
    void Start();

	
    glm::vec3 GetPosition() const;

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