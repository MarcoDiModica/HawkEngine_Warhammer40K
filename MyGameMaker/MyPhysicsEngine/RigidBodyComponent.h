#pragma once
#include "../MyGameEngine/Component.h"
#include <glm/glm.hpp>
#include <memory>

class PhysicsModule;
class btRigidBody;
class ColliderComponent;

class RigidbodyComponent : public Component {
public:
    RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~RigidbodyComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::RIGIDBODY; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void SetMass(float newMass);
    float GetMass() const { return mass; }

    void SetGravity(bool useGravity);
    bool UsesGravity() const { return useGravity; }

    void SetKinematic(bool isKinematic);
    bool IsKinematic() const { return kinematic; }

    void AddForce(const glm::vec3& force);
    void AddTorque(const glm::vec3& torque);
    void SetVelocity(const glm::vec3& velocity);

    // TODO: define these
    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3& position);
    glm::quat GetRotation() const;
    void SetRotation(const glm::quat& rotation);

private:
    void InitializeRigidbody();
    ColliderComponent* GetOrCreateCollider();

    btRigidBody* rigidBody;
    PhysicsModule* physics;
    float mass;
    bool useGravity;
    bool kinematic;
    bool initialized;
};