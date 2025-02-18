#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject* owner, PhysicsModule* physicsModule, bool isForStreet = false);
    ~ColliderComponent() override;

    /*ColliderComponent(const ColliderComponent& other, PhysicsModule* physicsModule);
    ColliderComponent& operator=(const ColliderComponent& other);

    ColliderComponent(ColliderComponent&& other, PhysicsModule* physicsModule) noexcept;
    ColliderComponent& operator=(ColliderComponent&& other) noexcept;*/

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    glm::vec3 GetSize() { return size; };
    void SetSize(const glm::vec3& newSize);

    float GetMass() { return mass; };
    glm::vec3 GetColliderPos();

    glm::quat GetColliderRotation();

    void SetColliderRotation(const glm::quat& rotation);

    void SetColliderPos(const glm::vec3& position);

    void SetMass(float newMass);

    void SetActive(bool active);

    void SetTrigger(bool trigger);
    bool IsTrigger() const;

private:
    btRigidBody* rigidBody; // Collider
    PhysicsModule* physics; // Referencia al módulo de físicas
    glm::vec3 size; // Tamaño del Bounding Box
    float mass; 
    bool isForStreetLocal;
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMapForhouse;


    void CreateCollider(bool isForStreet = false); 
};

