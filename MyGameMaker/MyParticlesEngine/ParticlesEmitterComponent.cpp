#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/quaternion.hpp>
#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"

ParticlesEmitterComponent::ParticlesEmitterComponent(GameObject* owner)
    : Component(owner)
    , lastSpawnTime(std::chrono::steady_clock::now())
    , deltaTime(Application->GetDt())
    , isSmoking(true)
    , m_Type(BillboardType::WORLD_ALIGNED)
{
    name = "ParticleEmitter";
    position = owner->GetComponent<Transform_Component>()->GetPosition();
    rotation = owner->GetComponent<Transform_Component>()->GetRotation();
    scale = owner->GetComponent<Transform_Component>()->GetScale();
}

void ParticlesEmitterComponent::Start() 
{
    position = owner->GetComponent<Transform_Component>()->GetPosition();
}

void ParticlesEmitterComponent::EmitParticle1(const glm::vec3& speed) {
    if (particles.size() >= maxParticles) return;

    try {
        Particle newParticle;
        newParticle.position.push_back(position);
        newParticle.SetParticleSpeed(speed);
        newParticle.lifetime = 5.0f;
        newParticle.rotation = 0.0f;

        if (!texturePath.empty()) {
            try {
                newParticle.texture->LoadTexture(texturePath);
                std::cout << "Texture loaded successfully from: " << texturePath << std::endl;
                std::cout << "Texture ID: " << newParticle.texture->id() << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to load texture: " << e.what() << std::endl;
                //default texture here
            }
        }
        else {
            std::cerr << "No texture path specified!" << std::endl;
        }

        newParticle.Start();
        isSmoking = true;
        particles.push_back(std::move(newParticle));

        std::cout << "Particle generated successfully at position: "
            << position.x << ", " << position.y << ", " << position.z << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error generating particle: " << e.what() << std::endl;
    }
}

void ParticlesEmitterComponent::EmitParticle2(const glm::vec3& speed) {
    if (particles.size() >= maxParticles) return;

    Particle newParticle;
    newParticle.position.push_back(position);
    newParticle.SetParticleSpeed(speed);
    newParticle.lifetime = 5.0f;
    newParticle.rotation = 0.0f;
    if (newParticle.texture) {
        newParticle.texture->LoadTexture(texturePath);
    }
    if (newParticle.texture2) {
        newParticle.texture2->LoadTexture(texturePath2);
    }
    newParticle.Start();
    isSmoking = false;
    particles.push_back(std::move(newParticle));
}

void ParticlesEmitterComponent::SetTexture(const std::string& newtexturePath) {
    this->texturePath = newtexturePath;
    //more logic if needed
}

std::string ParticlesEmitterComponent::GetTexture() const {
    return texturePath;
}

std::unique_ptr<Component> ParticlesEmitterComponent::Clone(GameObject* owner) {
    return std::make_unique<ParticlesEmitterComponent>(owner);
}

BillboardType ParticlesEmitterComponent::GetTypeEnum() const { return m_Type; }

void ParticlesEmitterComponent::SetType(BillboardType type) {
    m_Type = type;
}

void ParticlesEmitterComponent::Update(float deltaTime) 
{
    auto transformComponent = owner->GetComponent<Transform_Component>();
    if (transformComponent) {
        position = transformComponent->GetPosition();
        rotation = transformComponent->GetRotation();
        scale = transformComponent->GetScale();
    }

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = now - lastSpawnTime;

    if (elapsedTime.count() >= spawnRate && particles.size() < maxParticles) {
        glm::vec3 desiredSpeed(0.0f, 1.0f, 0.0f);
        if (isSmoking) {
            EmitParticle1(desiredSpeed);
        }
        else {
            EmitParticle2(desiredSpeed);
        }
        lastSpawnTime = now;
    }

    std::cout << "Active particles: " << particles.size() << std::endl;

    glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
    glm::vec3 cameraUp = Application->camera->GetTransform().GetUp();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    for (auto it = particles.begin(); it != particles.end();) {
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

        it->Update(deltaTime);

        if (it->lifetime <= 0.0f) {
            it = particles.erase(it);
        }
        else {
            ++it;
        }
    }

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void ParticlesEmitterComponent::Destroy() {
    particles.clear();
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
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