#pragma once
#include <chrono>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Particle.h"

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"

enum class BillboardType {
    SCREEN_ALIGNED,
    WORLD_ALIGNED,
    AXIS_ALIGNED
};

class ParticlesEmitterComponent : public Component {
public:
    explicit ParticlesEmitterComponent(GameObject* owner);
    ~ParticlesEmitterComponent() override = default;

    ParticlesEmitterComponent(const ParticlesEmitterComponent&) = delete;
    ParticlesEmitterComponent& operator=(const ParticlesEmitterComponent&) = delete;

    ParticlesEmitterComponent(ParticlesEmitterComponent&&) noexcept = default;
    ParticlesEmitterComponent& operator=(ParticlesEmitterComponent&&) noexcept = default;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    void EmitParticle1(const glm::vec3& speed);
    void EmitParticle2(const glm::vec3& speed);

    std::unique_ptr<Component> Clone(GameObject* owner) override;
    ComponentType GetType() const override { return ComponentType::PARTICLES_EMITTER; }

    //getset
    glm::vec3 GetPosition() const;
    void SetTexture(const std::string& texturePath);
    std::string GetTexture() const;
    float getSpawnRate() const;
    void setSpawnRate(float spawnRate);
    int getMaxParticles() const;
    void setMaxParticles(int maxParticles);

    //billboard
    glm::mat4 CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    glm::mat4 CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    glm::mat4 CalculateAxisAligned(const glm::vec3& cameraPosition, const glm::vec3& axis);

    BillboardType GetTypeEnum() const;
    void SetType(BillboardType type);

    bool isSmoking;

private:
    std::string texturePath;
    std::string texturePath2 = "../MyGameEditor/Assets/Textures/Firework.png";
    glm::vec3 position;
    glm::dquat rotation;
    glm::vec3 scale;
    float deltaTime;

    int maxParticles = 10;
    float spawnRate = 1.0f;
    std::chrono::steady_clock::time_point lastSpawnTime;

    std::vector<Particle> particles;
    BillboardType m_Type;
};