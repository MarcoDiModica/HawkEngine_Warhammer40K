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

    void SetActive(bool active);

    void SnapToPosition();

    glm::vec3 GetOffset();

    void SetOffset(const glm::vec3& newoffset);

    btRigidBody* GetCollider() const { return meshCollider; }


private:
    btRigidBody* meshCollider;
    PhysicsModule* physics;
    glm::vec3 size = glm::vec3(1.0f);
    glm::vec3 offset = glm::vec3(0.0f);

    bool hasSnappedToInitialPosition = false;

    void CreateMeshCollider();


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
