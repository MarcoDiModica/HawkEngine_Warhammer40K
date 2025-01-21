#pragma once

#include "Component.h"
#include "../MyGameEditor/PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~ColliderComponent() override;

    /*ColliderComponent(const ColliderComponent& other, PhysicsModule* physicsModule);
    ColliderComponent& operator=(const ColliderComponent& other);

    ColliderComponent(ColliderComponent&& other, PhysicsModule* physicsModule) noexcept;
    ColliderComponent& operator=(ColliderComponent&& other) noexcept;*/

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }

    // Clonar el componente
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

private:
    btRigidBody* rigidBody; // Collider en Bullet
    PhysicsModule* physics; // Referencia al módulo de físicas
    glm::vec3 size; // Tamaño del Bounding Box
    float mass; // Masa del colisionador

    void CreateCollider(); // Función para crear el colisionador
};
