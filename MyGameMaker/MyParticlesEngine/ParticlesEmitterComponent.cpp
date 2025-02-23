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
    , m_Type(BillboardType::AXIS_ALIGNED)
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
                GLuint id = newParticle.texture->id();
                if (id == 0) {
                    LOG(LogType::LOG_ERROR, "Texture loaded but ID is 0 for: %s", texturePath.c_str());
                    return;
                }
                newParticle.textureID = id;
            }
            catch (const std::exception& e) {
                LOG(LogType::LOG_ERROR, "Failed to load texture: %s", e.what());
                return;
            }
        }
        else {
            LOG(LogType::LOG_WARNING, "No texture path specified!");
            return;
        }

        newParticle.Start();
        isSmoking = true;
        particles.push_back(std::move(newParticle));
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Error generating particle: %s", e.what());
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

    glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
    glm::vec3 cameraUp = Application->camera->GetTransform().GetUp();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    for (auto it = particles.begin(); it != particles.end();) {
        glm::mat4 billboardMatrix;

        switch (m_Type) {
        case BillboardType::SCREEN_ALIGNED:
            billboardMatrix = CalculateScreenAligned(cameraPosition, cameraUp);
            break;
        case BillboardType::WORLD_ALIGNED:
            billboardMatrix = CalculateWorldAligned(cameraPosition, cameraUp);
            break;
        case BillboardType::AXIS_ALIGNED:
            billboardMatrix = CalculateAxisAligned(cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        }

        if (!it->position.empty()) {
            billboardMatrix[3] = glm::vec4(it->position[0], 1.0f);
        }

        it->Update(deltaTime);

        if (it->lifetime > 0.0f) {
            if (isSmoking) {
                it->Draw(billboardMatrix);
            }
            else {
                it->Draw2(billboardMatrix);
            }
            ++it;
        }
        else {
            it = particles.erase(it);
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

glm::mat4 ParticlesEmitterComponent::CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) const {
    glm::vec3 look = glm::normalize(cameraPosition - position);
    glm::vec3 right = glm::normalize(glm::cross(cameraUp, look));
    glm::vec3 up = glm::cross(look, right);

    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(look, 0.0f);
    billboardMatrix[3] = glm::vec4(position, 1.0f);

    return billboardMatrix;
}

glm::mat4 ParticlesEmitterComponent::CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) const {
    glm::vec3 look = glm::normalize(cameraPosition - position);
    look.y = 0.0f;
    look = glm::normalize(look);

    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), look));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(look, 0.0f);
    billboardMatrix[3] = glm::vec4(position, 1.0f);

    return billboardMatrix;
}

glm::mat4 ParticlesEmitterComponent::CalculateAxisAligned(const glm::vec3& cameraPosition, const glm::vec3& axis) const {
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