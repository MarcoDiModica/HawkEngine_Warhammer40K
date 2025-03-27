#pragma once

#include "../MyGameEngine/Component.h"
#include "BaseColliderComponent.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class BoxColliderComponent : public BaseColliderComponent {
public:
    BoxColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~BoxColliderComponent() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void CreateCollider() override;

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;
};
 
