#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/App.h"
#include"../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <iostream>
#include <chrono>



//Inicializar la última vez que se generó una partícula
ParticlesEmitterComponent::ParticlesEmitterComponent(GameObject* owner) : Component(owner)//TODO investigar que hay que poner aqui
{ 
    name = "ParticleEmmiter"; 
    lastSpawnTime = std::chrono::steady_clock::now();
    deltaTime = Application->GetDt();
    position = owner->GetComponent<Transform_Component>()->GetPosition();
	rotation = owner->GetComponent<Transform_Component>()->GetRotation();
	scale = owner->GetComponent<Transform_Component>()->GetScale();
    emitterParticle = new Particle();
}

void ParticlesEmitterComponent::Start() 
{
	//new ParticlesEmitterComponent(owner);
	this->position = owner->GetComponent<Transform_Component>()->GetPosition();
	if (emitterParticle == nullptr)
    {
     SetParticleVariables(emitterParticle);
    }  
}

void ParticlesEmitterComponent::SetTexture(const std::string& newtexturePath) {
    this->texturePath = newtexturePath;
    // Aquí puedes añadir lógica adicional para cargar la textura si es necesario
}

std::string ParticlesEmitterComponent::GetTexture() const {
    return texturePath;
}

std::unique_ptr<Component> ParticlesEmitterComponent::Clone(GameObject* owner) {
    return std::make_unique<ParticlesEmitterComponent>(owner);
}

void ParticlesEmitterComponent::EmitParticle(const glm::vec3& speed) {
   /* Particle* newParticle = new Particle();
	newParticle = SetParticleVariables(emmiterParticle);
    newParticle->Start();
    particles.push_back(*newParticle);

    std::cout << "Partícula generada" << std::endl;*/

	//__________NO ENTIENDO PORQUE ESTO FUNCIONA Y LO DE ARRIBA NO, PERO FUNCIONA___________
    Particle* newParticle = new Particle();
    newParticle->position.push_back(this->position);  // Posición inicial basada en la posición actual del GameObject
    newParticle->SetParticleSpeed(speed); // Velocidad inicial
    newParticle->lifetime = 5.0f; // Duración de la partícula en segundos
    newParticle->rotation = 0.0f; // Rotación inicial
    newParticle->texture->LoadTexture(texturePath); // Cargar la textura
    newParticle->Start();
    particles.push_back(*newParticle);
 
    std::cout << "Partícula generada" << std::endl;    
}

void ParticlesEmitterComponent::Update(float deltaTime) {
    // Actualizar la posición, rotación y escala del emisor de partículas
    this->position = owner->GetComponent<Transform_Component>()->GetPosition();
    this->rotation = owner->GetComponent<Transform_Component>()->GetRotation();
    this->scale = owner->GetComponent<Transform_Component>()->GetScale();

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        // Crear una nueva partícula
        if (emitterParticle != nullptr) {
           /* emitterParticle->position.push_back(this->position);*/
            glm::vec3 desiredSpeed = glm::vec3(1.0f, 1.0f, 0.0f); // Aquí puedes configurar la velocidad deseada
            EmitParticle(desiredSpeed);
        }
        lastSpawnTime = now;
    }

    // Actualizar las partículas existentes
    for (auto& particle : particles) {
        particle.Update(deltaTime);
        // Actualizar la posición de la partícula en función de la nueva posición del GameObject
        /*particle.position.push_back(this->position);*/
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
    Destroy();
}

void ParticlesEmitterComponent::Destroy() {
    
    if (emitterParticle != nullptr) {
        delete emitterParticle;
        emitterParticle = nullptr;
    }
    
    particles.clear();
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}

Particle* ParticlesEmitterComponent::SetParticleVariables(Particle* variablesParticle) {
	variablesParticle->position.push_back(this->position); // Posición inicial
	variablesParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	variablesParticle->lifetime = 10.0f; // Duración de la partícula en segundos
	variablesParticle->rotation = 0.0f; // Rotación inicial
    
	return variablesParticle;
}