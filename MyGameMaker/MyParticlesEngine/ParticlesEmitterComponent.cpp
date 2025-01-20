#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/App.h"
#include"../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <iostream>
#include <chrono>

ParticlesEmitterComponent* emmiter = nullptr;

//Inicializar la última vez que se generó una partícula
ParticlesEmitterComponent::ParticlesEmitterComponent(GameObject* owner) : Component(owner) 
{ 
    name = "ParticleEmmiter"; 
    lastSpawnTime = std::chrono::steady_clock::now();
    deltaTime = Application->GetDt();
    position = owner->GetComponent<Transform_Component>()->GetPosition();
}

void ParticlesEmitterComponent::Start() 
{
	emmiter = new ParticlesEmitterComponent(owner);
	emmiter->position = owner->GetComponent<Transform_Component>()->GetPosition();
	emitterParticle = new Particle();	
	SetParticleVariables(emitterParticle);
}

void ParticlesEmitterComponent::EmitParticle() {
    Particle* newParticle = new Particle();
    newParticle->Start();
    particles.push_back(*newParticle);

    std::cout << "Partícula generada" << std::endl;
}

void ParticlesEmitterComponent::Update(float deltaTime) {
    
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        // Crear una nueva partícula
        if (emitterParticle != nullptr) {
			EmitParticle();
        }
        lastSpawnTime = now;
    }
    // Actualizar las partículas existentes
    for (auto& particle : particles) {
        particle.Update(deltaTime);
        particle.Draw();
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
	delete emitterParticle;
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}

Particle* ParticlesEmitterComponent::SetParticleVariables(Particle* variablesParticle) {
	variablesParticle->position.push_back(emmiter->position); // Posición inicial
	variablesParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	variablesParticle->lifetime = 10.0f; // Duración de la partícula en segundos
	variablesParticle->rotation = 0.0f; // Rotación inicial
    
	return variablesParticle;
}