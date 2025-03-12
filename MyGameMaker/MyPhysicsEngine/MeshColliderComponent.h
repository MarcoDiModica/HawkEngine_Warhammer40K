// MeshColliderComponent.h
#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class MeshColliderComponent : public Component {
public:
    MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~MeshColliderComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void SetColliderMesh();

    btRigidBody* GetRigidBody() const { return rigidBody; }

private:
    btRigidBody* rigidBody;
    PhysicsModule* physics;
    void CreateMeshCollider();
};