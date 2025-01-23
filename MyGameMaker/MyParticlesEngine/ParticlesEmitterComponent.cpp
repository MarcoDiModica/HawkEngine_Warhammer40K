#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/quaternion.hpp>
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

void ParticlesEmitterComponent::EmitParticle1(const glm::vec3& speed) {
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
    newParticle->texture2 = nullptr;
    newParticle->Start();
	isSmoking = true;
    particles.push_back(*newParticle);
 
    std::cout << "Partícula generada" << std::endl;    
}

void ParticlesEmitterComponent::EmitParticle2(const glm::vec3& speed) {
   
    Particle* newParticle = new Particle();
    newParticle->position.push_back(this->position);  // Posición inicial basada en la posición actual del GameObject
    newParticle->SetParticleSpeed(speed); // Velocidad inicial
    newParticle->lifetime = 5.0f; // Duración de la partícula en segundos
    newParticle->rotation = 0.0f; // Rotación inicial
    newParticle->texture->LoadTexture(texturePath);
    newParticle->texture2->LoadTexture(texturePath2);// Cargar la textura
    newParticle->Start();
    particles.push_back(*newParticle);
    isSmoking = false;

    std::cout << "Partícula generada" << std::endl;
}
BillboardType ParticlesEmitterComponent::GetTypeEnum() const { return m_Type; }

void ParticlesEmitterComponent::SetType(BillboardType type) {
    m_Type = type;
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
       
       if (emitterParticle != nullptr && isSmoking == true) {
           /* emitterParticle->position.push_back(this->position);*/
           glm::vec3 desiredSpeed = glm::vec3(0.0f, 1.0f, 0.0f); // Aquí puedes configurar la velocidad deseada
           EmitParticle1(desiredSpeed);
       }
       else if (emitterParticle != nullptr && isSmoking == false) {
           glm::vec3 desiredSpeed = glm::vec3(0.0f, 1.0f, 0.0f); // Aquí puedes configurar la velocidad deseada
           EmitParticle2(desiredSpeed);
         
           }
        lastSpawnTime = now;
    }

    glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
    glm::vec3 cameraUp = Application->camera->GetTransform().GetUp();
        for (auto& particle : particles) {
            glm::mat4 transformMatrix;

            switch (m_Type) {
            case BillboardType::SCREEN_ALIGNED:
                transformMatrix = CalculateScreenAligned(cameraPosition, cameraUp);
                break;
            case BillboardType::WORLD_ALIGNED:
                transformMatrix = CalculateWorldAligned(cameraPosition, cameraUp);
                break;
            case BillboardType::AXIS_ALIGNED:
                transformMatrix = CalculateAxisAligned(cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            }

           
                particle.Update(deltaTime);
        /*        ApplyTransform(transformMatrix);*/
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


glm::mat4 ParticlesEmitterComponent::CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    glm::vec3 forward = glm::normalize(cameraPosition - this->position);
    glm::vec3 right = glm::normalize(glm::cross(cameraUp, forward));
    glm::vec3 up = glm::cross(forward, right);

    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(forward, 0.0f);
    billboardMatrix[3] = glm::vec4(this->position, 1.0f);

    return billboardMatrix;
}

glm::mat4 ParticlesEmitterComponent::CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[3] = glm::vec4(this->position, 1.0f);
    return billboardMatrix;
}

glm::mat4 ParticlesEmitterComponent::CalculateAxisAligned(const glm::vec3& cameraPosition, const glm::vec3& axis) {
    glm::vec3 forward = glm::normalize(cameraPosition - this->position);
    glm::vec3 right = glm::normalize(glm::cross(axis, forward));
    glm::vec3 up = glm::cross(forward, right);

    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(axis, 0.0f);
    billboardMatrix[3] = glm::vec4(this->position, 1.0f);

    return billboardMatrix;
}

//void ParticlesEmitterComponent::ApplyTransform(const glm::mat4& transformMatrix) {
//    for (auto& particle : particles) {
//        particle.position[0] = glm::vec3(transformMatrix * glm::vec4(particle.position[0], 1.0f));
//    }
//}

