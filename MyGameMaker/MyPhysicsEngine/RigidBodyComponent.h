
// RigidbodyComponent.h
#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include "BoxColliderComponent.h"
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

class RigidbodyComponent : public Component {
public:
    RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~RigidbodyComponent() override;

	void Awake() override;
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    void Init();

    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    ComponentType GetType() const override { return ComponentType::RIGIDBODY; }

    void SetMass(float newMass);
    float GetMass() const;

    bool IsFreezed() const { return isFreezed; }
    void SetFreezeRotations(bool freeze);

    float GetFriction() const;
    void SetFriction(float friction);

    bool IsKinematic() const;
    void SetKinematic(bool isKinematic);

    void AddForce(const glm::vec3& force);

    glm::vec3 GetGravity() const;
    void SetGravity(const glm::vec3& gravity);
    
    glm::vec2 GetDamping() const;
    void SetDamping(float linearDamping, float angularDamping);
    void EnableContinuousCollision();

    btRigidBody* GetRigidBody() const { return rigidBody; }

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;


    void DecodeRigidbody();
private:
    void CreateRigidbody();
    btRigidBody* rigidBody;
    btMotionState* motionState;
    PhysicsModule* physics;

    //Propierties
    float mass = 1.0f;
    bool isKinematic = false;
    bool isFreezed = true;
    float friction = 0.5f;
    glm::vec3 gravity = glm::vec3(0, -9.8f, 0);
    glm::vec2 damping = glm::vec2(0.0f, 0.0f);

protected:
    friend class SceneSerializer;

    YAML::Node encode() override {
        YAML::Node node;
        node["mass"] = mass;
        node["isKinematic"] = isKinematic;
        node["isFreezed"] = isFreezed;
        node["friction"] = friction;
        node["gravity"]["x"] = gravity.x;
        node["gravity"]["y"] = gravity.y;
        node["gravity"]["z"] = gravity.z;
        node["damping"]["linear"] = damping.x; 
        node["damping"]["angular"] = damping.y; 
        return node;
    }

    bool decode(const YAML::Node& node) override {
        if (node["mass"].IsDefined()) {
            mass = node["mass"].as<float>();
        }
        if (node["isKinematic"].IsDefined()) {
            isKinematic = node["isKinematic"].as<bool>();
        }
        if (node["isFreezed"].IsDefined()) {
            isFreezed = node["isFreezed"].as<bool>();
        }
        if (node["friction"].IsDefined()) {
            friction = node["friction"].as<float>();
        }
        if (node["gravity"].IsDefined()) {
            gravity.x = node["gravity"]["x"].as<float>();
            gravity.y = node["gravity"]["y"].as<float>();
            gravity.z = node["gravity"]["z"].as<float>();
        }
        if (node["damping"].IsDefined()) {
            damping.x = node["damping"]["linear"].as<float>(); 
            damping.y = node["damping"]["angular"].as<float>(); 
        }
        Init();
        return true;
    }
};