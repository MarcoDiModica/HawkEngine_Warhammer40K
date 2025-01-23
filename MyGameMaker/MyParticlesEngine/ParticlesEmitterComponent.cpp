#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/App.h"
#include"../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <iostream>
#include <chrono>



//Inicializar la �ltima vez que se gener� una part�cula
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
    // Aqu� puedes a�adir l�gica adicional para cargar la textura si es necesario
}

std::string ParticlesEmitterComponent::GetTexture() const {
    return texturePath;
}

std::unique_ptr<Component> ParticlesEmitterComponent::Clone(GameObject* owner) {
    return std::make_unique<ParticlesEmitterComponent>(owner);
}

void ParticlesEmitterComponent::EmitParticle1(const glm::vec3& speed) {
   /* Particle* newParticle = new Particle();
	newParticle = SetParticleVariables(emmiterParticle);
    newParticle->Start();
    particles.push_back(*newParticle);

    std::cout << "Part�cula generada" << std::endl;*/

	//__________NO ENTIENDO PORQUE ESTO FUNCIONA Y LO DE ARRIBA NO, PERO FUNCIONA___________
    Particle* newParticle = new Particle();
    newParticle->position.push_back(this->position);  // Posici�n inicial basada en la posici�n actual del GameObject
    newParticle->SetParticleSpeed(speed); // Velocidad inicial
    newParticle->lifetime = 5.0f; // Duraci�n de la part�cula en segundos
    newParticle->rotation = 0.0f; // Rotaci�n inicial
    newParticle->texture->LoadTexture(texturePath); // Cargar la textura
    newParticle->texture2 = nullptr;
    newParticle->Start();
    particles.push_back(*newParticle);
 
    std::cout << "Part�cula generada" << std::endl;    
}

void ParticlesEmitterComponent::EmitParticle2(const glm::vec3& speed) {
   
    Particle* newParticle = new Particle();
    newParticle->position.push_back(this->position);  // Posici�n inicial basada en la posici�n actual del GameObject
    newParticle->SetParticleSpeed(speed); // Velocidad inicial
    newParticle->lifetime = 5.0f; // Duraci�n de la part�cula en segundos
    newParticle->rotation = 0.0f; // Rotaci�n inicial
    newParticle->texture->LoadTexture(texturePath);
    newParticle->texture2->LoadTexture(texturePath2);// Cargar la textura
    newParticle->Start();
    particles.push_back(*newParticle);
    isSmoking = true;

    std::cout << "Part�cula generada" << std::endl;
}

void ParticlesEmitterComponent::Update(float deltaTime) {
    // Actualizar la posici�n, rotaci�n y escala del emisor de part�culas
    this->position = owner->GetComponent<Transform_Component>()->GetPosition();
    this->rotation = owner->GetComponent<Transform_Component>()->GetRotation();
    this->scale = owner->GetComponent<Transform_Component>()->GetScale();

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        // Crear una nueva part�cula
       
       if (emitterParticle != nullptr && isSmoking == true) {
           /* emitterParticle->position.push_back(this->position);*/
           glm::vec3 desiredSpeed = glm::vec3(0.0f, 1.0f, 0.0f); // Aqu� puedes configurar la velocidad deseada
           EmitParticle1(desiredSpeed);
       }
       else if (emitterParticle != nullptr && isSmoking == false) {
           glm::vec3 desiredSpeed = glm::vec3(0.0f, 1.0f, 0.0f); // Aqu� puedes configurar la velocidad deseada
           EmitParticle2(desiredSpeed);
         
           }
        lastSpawnTime = now;
    }

    // Actualizar las part�culas existentes
    for (auto& particle : particles) {
        particle.Update(deltaTime);
        // Actualizar la posici�n de la part�cula en funci�n de la nueva posici�n del GameObject
        /*particle.position.push_back(this->position);*/
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
	variablesParticle->position.push_back(this->position); // Posici�n inicial
	variablesParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial
	variablesParticle->lifetime = 10.0f; // Duraci�n de la part�cula en segundos
	variablesParticle->rotation = 0.0f; // Rotaci�n inicial
    
	return variablesParticle;
}



float ParticlesEmitterComponent::getSpawnRate() const
{
    return spawnRate;
}

void ParticlesEmitterComponent::setSpawnRate(float newSpawnRate)
{
    spawnRate = newSpawnRate;
}

int ParticlesEmitterComponent::getMaxParticles() const {
    return maxParticles;
}

void ParticlesEmitterComponent::setMaxParticles(int newMaxParticles) {
    maxParticles = newMaxParticles;
}