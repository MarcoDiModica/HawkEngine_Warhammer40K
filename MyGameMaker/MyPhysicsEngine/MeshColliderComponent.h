// MeshColliderComponent.h
#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include "BaseColliderComponent.h"
#include <glm/glm.hpp>
#include <memory>

class MeshColliderComponent : public BaseColliderComponent {
public:
    MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~MeshColliderComponent() override;

    void Start() override;
    void Update(float deltaTime) override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void CreateCollider() override;

    MonoObject* GetSharp() override;
};
