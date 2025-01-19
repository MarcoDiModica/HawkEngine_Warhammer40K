#include "ParticlesEmitterComponent.h"
#include <chrono>

//Inicializar la última vez que se generó una partícula
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
        // Crear una nueva partícula
        if (emitterParticle != nullptr) {
			emitterParticle = SetParticleVariables(emitterParticle);
            emitterParticle->Spawn();
        }
        lastSpawnTime = now;
    }

    // Actualizar las partículas existentes
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
	particle->lifetime = 5.0f; // Duración de la partícula en segundos
	particle->rotation = 0.0f; // Rotación inicial
	return particle;
}