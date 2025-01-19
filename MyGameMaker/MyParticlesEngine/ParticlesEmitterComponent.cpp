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

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        // Crear una nueva partícula
        if (emitterParticle != nullptr) {
            Particle newParticle = *SetParticleVariables(emitterParticle);
            newParticle.Spawn();
            particles.push_back(newParticle);
        }
        lastSpawnTime = now;
    }

    // Actualizar las partículas existentes
    for (auto& particle : particles) {
        particle.Update();
    }
    // Eliminar partículas que han terminado su vida útil
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.lifetime <= 0.0f; // Condición para eliminar la partícula
            }),
        particles.end() // Eliminar desde el nuevo final hasta el final original
    );
}

//Destructor
ParticlesEmitterComponent::~ParticlesEmitterComponent()
{
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}

Particle* ParticlesEmitterComponent::SetParticleVariables(Particle* variablesParticle) {
	variablesParticle->position.push_back(position);
	variablesParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	variablesParticle->lifetime = 5.0f; // Duración de la partícula en segundos
	variablesParticle->rotation = 0.0f; // Rotación inicial
	return variablesParticle;
}