#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/App.h"
#include"../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <iostream>
#include <chrono>

//Inicializar la �ltima vez que se gener� una part�cula
ParticlesEmitterComponent::ParticlesEmitterComponent(GameObject* owner) : Component(owner) 
{ 
    name = "ParticleEmmiter"; 
    lastSpawnTime = std::chrono::steady_clock::now();
}

void ParticlesEmitterComponent::Start() {
	emitterParticle = new Particle();
	deltaTime = Application->GetDt();
	position = owner->GetComponent<Transform_Component>()->GetPosition();
	SetParticleVariables(emitterParticle);
}

void ParticlesEmitterComponent::EmitParticle() {
    Particle* newParticle = new Particle();
    newParticle->Start();
    particles.push_back(*newParticle);

    std::cout << "Part�cula generada" << std::endl;
}

void ParticlesEmitterComponent::Update(float deltaTime) {
    
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        // Crear una nueva part�cula
        if (emitterParticle != nullptr) {
			EmitParticle();
        }
        lastSpawnTime = now;
    }
    // Actualizar las part�culas existentes
    for (auto& particle : particles) {
        particle.Update(deltaTime);
        particle.Draw();
    }
    // Eliminar part�culas que han terminado su vida �til
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.lifetime <= 0.0f; // Condici�n para eliminar la part�cula
            }),
        particles.end() // Eliminar desde el nuevo final hasta el final original
    );
}

//Destructor
ParticlesEmitterComponent::~ParticlesEmitterComponent()
{
	delete emitterParticle;
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}

Particle* ParticlesEmitterComponent::SetParticleVariables(Particle* variablesParticle) {
	variablesParticle->position.push_back(position);
	variablesParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	variablesParticle->lifetime = 5.0f; // Duraci�n de la part�cula en segundos
	variablesParticle->rotation = 0.0f; // Rotaci�n inicial
	return variablesParticle;
}