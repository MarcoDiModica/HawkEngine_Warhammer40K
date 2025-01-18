//#pragma once
//#include "../MyGameEngine/Component.h"
//
//
//class PhysicsModule;
//
//class RigidbodyComponent : public Component {
//public:
//    explicit Rigidbody_Component(GameObject* owner, float mass = 1.0f)
//        : Component(owner), mass(mass), rigidBody(nullptr) {
//        name = "Rigidbody_Component";
//    }
//
//    ~Rigidbody_Component() override {
//        Destroy();
//    }
//
//    void Start() override {
//        if (auto* physicsModule = GetPhysicsModule()) {
//            physicsModule->CreatePhysicsForGameObject(*owner, mass);
//            rigidBody = physicsModule->GetRigidBodyForGameObject(*owner);
//        }
//    }
//
//    void Update(float deltaTime) override {
//        // No-op: la sincronización la maneja PhysicsModule::SyncTransforms
//    }
//
//    void Destroy() override {
//        if (rigidBody) {
//            if (auto* physicsModule = GetPhysicsModule()) {
//                physicsModule->RemoveRigidBodyForGameObject(*owner);
//            }
//            rigidBody = nullptr;
//        }
//    }
//
//    ComponentType GetType() const override {
//        return ComponentType::NONE; // O un nuevo tipo específico si es necesario
//    }
//
//    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
//        return std::make_unique<Rigidbody_Component>(new_owner, mass);
//    }
//
//    float GetMass() const { return mass; }
//    void SetMass(float newMass) { mass = newMass; }
//
//private:
//    float mass;
//    btRigidBody* rigidBody;
//
//    PhysicsModule* GetPhysicsModule() const {
//        // Suponiendo que App tiene acceso al PhysicsModule.
//        return owner->GetApp()->GetModule<PhysicsModule>();
//    }
//};
