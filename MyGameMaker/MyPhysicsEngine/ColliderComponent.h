#pragma once
#include "../MyGameEngine/Component.h"
#include <glm/glm.hpp>
#include <memory>

class PhysicsModule;
class btCollisionShape;
class btRigidBody;

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~ColliderComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void Initialize();
    bool IsInitialized() const { return initialized; }

    void SetTrigger(bool isTrigger);
    bool IsTrigger() const { return isTrigger; }

    void SetSize(const glm::vec3& newSize);
    glm::vec3 GetSize() const { return size; }

    btCollisionShape* GetCollisionShape() const { return collisionShape; }
    btRigidBody* GetRigidBody() const { return rigidBody; }

    void SetColliderPosition(const glm::vec3& position);
    void SetColliderRotation(const glm::quat& rotation);
    glm::vec3 GetColliderPosition() const;
    glm::quat GetColliderRotation() const;

    void SetStatic(bool isStatic) { this->isStatic = isStatic; }
    bool IsStatic() const { return isStatic; }

private:
    void CreateCollisionBody();
    void UpdateTransform();

    PhysicsModule* physics;
    btCollisionShape* collisionShape;
    btRigidBody* rigidBody;
    glm::vec3 size;
    bool isTrigger;
    bool isStatic;
    bool initialized;
};