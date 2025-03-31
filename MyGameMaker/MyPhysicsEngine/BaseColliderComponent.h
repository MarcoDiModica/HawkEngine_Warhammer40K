#pragma once

#include "../MyGameEngine/Component.h"
#include "PhysicsModule.h"
#include <glm/glm.hpp>
#include <memory>

class BaseColliderComponent : public Component {
public:
    BaseColliderComponent(GameObject* owner, PhysicsModule* physicsModule);
    ~BaseColliderComponent() override;

	void Awake() override;
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    //Size and offset
    glm::vec3 GetSize();
    glm::vec3 GetOffset();
    void SetOffset(const glm::vec3& newoffset);
    void SetSize(const glm::vec3& newSize);

    //TriggerFuncions
    void SetTrigger(bool trigger);
    bool IsTrigger() const;

    //Transform Functions
    glm::vec3 GetColliderPos();
    glm::quat GetColliderRotation();
    void SetColliderRotation(const glm::quat& rotation);
    void SetColliderPos(const glm::vec3& position);

    void SnapToPosition();

    void SetActive(bool active);

    //OnCollision
    virtual void OnCollisionEnter(BaseColliderComponent* other);
    virtual void OnCollisionStay(BaseColliderComponent* other);
    virtual void OnCollisionExit(BaseColliderComponent* other);
    //OnTrigger
    virtual void OnTriggerEnter(BaseColliderComponent* other);
    virtual void OnTriggerStay(BaseColliderComponent* other);
    virtual void OnTriggerExit(BaseColliderComponent* other);


    btRigidBody* GetRigidBody() const { return collider; }

    MonoObject* CsharpReference = nullptr;

public:
    PhysicsModule* physics;


protected:

    btRigidBody* collider = nullptr;
    glm::vec3 size = glm::vec3(1.0f);
    glm::vec3 offset = glm::vec3(0.0f);

    bool hasSnappedToInitialPosition = false;


    virtual void CreateCollider() = 0;

protected:
    friend class SceneSerializer;

    YAML::Node encode() override
    {
        YAML::Node node;
        node["size"] = std::vector<float>{ size.x, size.y, size.z };
        node["offset"] = std::vector<float>{ offset.x, offset.y, offset.z };
        node["isTrigger"] = IsTrigger();
        return node;
    }

    bool decode(const YAML::Node& node) override
    {
        if (node["size"]) {
            auto sizeVec = node["size"].as<std::vector<float>>();
            size = glm::vec3(sizeVec[0], sizeVec[1], sizeVec[2]);
            SetSize(size);
        }
        if (node["offset"]) {
            auto offsetVec = node["offset"].as<std::vector<float>>();
            offset = glm::vec3(offsetVec[0], offsetVec[1], offsetVec[2]);
        }
        if (node["isTrigger"]) {
            SetTrigger(node["isTrigger"].as<bool>());
        }
        return true;
    }
};

