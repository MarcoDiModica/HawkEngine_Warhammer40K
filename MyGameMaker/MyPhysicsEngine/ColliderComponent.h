//#pragma once
//
//#include "../MyGameEngine/Component.h"
//#include "PhysicsModule.h"
//#include <glm/glm.hpp>
//#include <memory>
//
//class ColliderComponent : public Component {
//public:
//    ColliderComponent(GameObject* owner/*, PhysicsModule* physicsModule, bool isForStreet = false*/);
//    ~ColliderComponent() override;
//
//    /*ColliderComponent(const ColliderComponent& other, PhysicsModule* physicsModule);
//    ColliderComponent& operator=(const ColliderComponent& other);
//
//    ColliderComponent(ColliderComponent&& other, PhysicsModule* physicsModule) noexcept;
//    ColliderComponent& operator=(ColliderComponent&& other) noexcept;*/
//
//    void Start() override;
//    void Update(float deltaTime) override;
//    void Destroy() override;
//
//    ComponentType GetType() const override { return ComponentType::COLLIDER; }
//
//    // Clonar el componente
//    std::unique_ptr<Component> Clone(GameObject* new_owner) override;
//
//private:
//    btRigidBody* rigidBody; // Collider en Bullet
//    PhysicsModule* physics; // Referencia al m�dulo de f�sicas
//    glm::vec3 size; // Tama�o del Bounding Box
//    float mass; // Masa del colisionador
//    bool isForStreetLocal = false;
//    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMapForhouse;
//
//
//    void CreateCollider(bool isForStreet = false); // Funci�n para crear el colisionador
//};
//
