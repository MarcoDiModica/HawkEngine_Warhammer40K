// MeshColliderComponent.h
#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class MeshColliderComponent : public Component {
public:
    MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~MeshColliderComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    void SetTrigger(bool trigger);

    bool IsTrigger() const;

    glm::vec3 GetColliderPos();

    glm::quat GetColliderRotation();

    void SetColliderRotation(const glm::quat& rotation);

    void SetColliderPos(const glm::vec3& position);

    glm::vec3 GetSize();

    void SetSize(const glm::vec3& newSize);

    void SetMass(float newMass);

    void SetActive(bool active);

    void SnapToPosition();

    void SetColliderMesh();

    btRigidBody* GetRigidBody() const { return rigidBody; }


private:
    btRigidBody* rigidBody;
    PhysicsModule* physics;

    glm::vec3 size;
    float mass;
    bool isForStreetLocal;

    bool snapToPosition = false;
    bool isFromDecode = false;
    void CreateMeshCollider();

protected:
    friend class SceneSerializer;

    YAML::Node encode() override
    {
        return YAML::Node();
    }

    bool decode(const YAML::Node& node) override
    {
        //Start();
		isFromDecode = true;
        return false;
    }
};
