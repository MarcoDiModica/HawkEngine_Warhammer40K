#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class CapsuleColliderComponent : public Component {
public:
    CapsuleColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~CapsuleColliderComponent() override;

    void Start() override;
    void SetTrigger(bool trigger);
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::COLLIDER; }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    glm::vec3 GetSize();

    glm::vec3 GetOffset();

    void SetOffset(const glm::vec3& newoffset);

    void SetSize(const glm::vec3& newSize);

    bool IsTrigger() const;
    glm::vec3 GetColliderPos();

    glm::quat GetColliderRotation();

    void SetColliderRotation(const glm::quat& rotation);

    void SetColliderPos(const glm::vec3& position);

    void SetActive(bool active);

    bool GetSnapToPosition() const { return snapToPosition; }
    void SetSnapToPosition(bool value) { snapToPosition = value; }
    void SnapToPosition();


    //OnCollision
    virtual void OnCollisionEnter(CapsuleColliderComponent* other);
    virtual void OnCollisionStay(CapsuleColliderComponent* other);
    virtual void OnCollisionExit(CapsuleColliderComponent* other);
    //OnTrigger
    virtual void OnTriggerEnter(CapsuleColliderComponent* other);
    virtual void OnTriggerStay(CapsuleColliderComponent* other);
    virtual void OnTriggerExit(CapsuleColliderComponent* other);


    btRigidBody* GetRigidBody() const { return collider; }

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

private:

    glm::vec3 offset;
    btRigidBody* collider = nullptr;
    PhysicsModule* physics;
    glm::vec3 size;
    glm::vec3 scaleFactor;

    bool snapToPosition = false;

    void CreateCollider();

protected:
    friend class SceneSerializer;

    YAML::Node encode() override
    {
        return YAML::Node();
    }

    bool decode(const YAML::Node& node) override
    {
        Start();
        SnapToPosition();
        return false;
    }
};