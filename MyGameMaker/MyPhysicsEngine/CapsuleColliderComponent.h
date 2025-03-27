#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include "BaseColliderComponent.h"
#include <glm/glm.hpp>
#include <memory>

class CapsuleColliderComponent : public BaseColliderComponent {
public:
    CapsuleColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~CapsuleColliderComponent() override;

    void Start() override;
    void Update(float deltaTime) override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

    void CreateCollider() override;
};