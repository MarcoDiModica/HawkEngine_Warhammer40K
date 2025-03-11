
// RigidbodyComponent.h
#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include "ColliderComponent.h"
#include <glm/glm.hpp>

class RigidbodyComponent : public Component {
public:
    RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~RigidbodyComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

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


private:
    btRigidBody* rigidBody;
    btMotionState* motionState;
    PhysicsModule* physics;
    float mass;
    void CreateRigidbody();

	//bools for propierties;
    bool isKinematic;
    bool isFreezed;

    void SetRigidBody(btRigidBody* rigidBody);

protected:
	friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node;
		node["mass"] = mass;
		node["isKinematic"] = isKinematic;
		node["isFreezed"] = isFreezed;
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
		return true;
	}
};