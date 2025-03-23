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

    btRigidBody* collider = nullptr;
    PhysicsModule* physics;
    glm::vec3 size = glm::vec3(1.0f);
	glm::vec3 offset = glm::vec3(0.0f);

	bool snapToPosition = false;
 
    bool hasSnappedToInitialPosition = false;

    void CreateCollider();

protected:
	friend class SceneSerializer;

    YAML::Node encode() override
    {
        YAML::Node node;
        node["size"] = std::vector<float>{size.x, size.y, size.z};
        node["offset"] = std::vector<float>{offset.x, offset.y, offset.z};
        return node;
    }

    bool decode(const YAML::Node& node) override
    {
        if (node["size"]) {
            auto sizeVec = node["size"].as<std::vector<float>>();
            size = glm::vec3(sizeVec[0], sizeVec[1], sizeVec[2]);
        }
        if (node["offset"]) {
            auto offsetVec = node["offset"].as<std::vector<float>>();
            offset = glm::vec3(offsetVec[0], offsetVec[1], offsetVec[2]);
        }
        return true;
    }
};
 
