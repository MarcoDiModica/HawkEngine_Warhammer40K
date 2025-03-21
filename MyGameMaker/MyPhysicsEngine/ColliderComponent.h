#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~ColliderComponent() override;

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
    virtual void OnCollisionEnter(ColliderComponent* other);
    virtual void OnCollisionStay(ColliderComponent* other);
    virtual void OnCollisionExit(ColliderComponent* other); 
    //OnTrigger
    virtual void OnTriggerEnter(ColliderComponent* other);
    virtual void OnTriggerStay(ColliderComponent* other);
    virtual void OnTriggerExit(ColliderComponent* other);


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
 
