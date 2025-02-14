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
    glm::vec3 GetColliderPos() { 
        
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();
        return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
    }

    void SetColliderPos(const glm::vec3& position) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(btVector3(position.x, position.y, position.z));
        rigidBody->getMotionState()->setWorldTransform(trans);
        rigidBody->setCenterOfMassTransform(trans);
    }

    void SetMass(float newMass);

    void SetActive(bool active);

private:
    btRigidBody* rigidBody; // Collider en Bullet
    PhysicsModule* physics; // Referencia al m�dulo de f�sicas
    glm::vec3 size; // Tama�o del Bounding Box
    float mass; // Masa del colisionador
    bool isForStreetLocal;
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMapForhouse;


    void CreateCollider(bool isForStreet = false); // Funci�n para crear el colisionador
};

