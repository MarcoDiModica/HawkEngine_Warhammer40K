#include "ParticlesEmitterComponent.h"
#include <chrono>

//Inicializar la �ltima vez que se gener� una part�cula
ParticlesEmitterComponent::ParticlesEmitterComponent() {
    lastSpawnTime = std::chrono::steady_clock::now();
}

void ParticlesEmitterComponent::Start() {
	emitterParticle = new Particle();
}

void ParticlesEmitterComponent::Update() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate) {
        // Crear una nueva part�cula
        if (emitterParticle != nullptr) {
			emitterParticle = SetParticleVariables(emitterParticle);
            emitterParticle->Spawn();
        }
        lastSpawnTime = now;
    }

    // Actualizar las part�culas existentes
    if (emitterParticle != nullptr) {
        emitterParticle->Update();
    }
}

//Destructor
ParticlesEmitterComponent::~ParticlesEmitterComponent()
{
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}

Particle* ParticlesEmitterComponent::SetParticleVariables(Particle* particle) {
	particle->position.push_back(position);
	particle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	particle->lifetime = 5.0f; // Duraci�n de la part�cula en segundos
	particle->rotation = 0.0f; // Rotaci�n inicial
	return particle;
}