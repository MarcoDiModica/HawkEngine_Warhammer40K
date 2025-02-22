#include "ColliderComponent.h"
#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include <bullet/btBulletDynamicsCommon.h>

ColliderComponent::ColliderComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner)
    , physics(physicsModule)
    , collisionShape(nullptr)
    , rigidBody(nullptr)
    , isTrigger(false)
    , size(1.0f)
    , initialized(false)
    , isStatic(true) 
{
    name = "ColliderComponent";
}

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Initialize() {
	if (!initialized) {
		CreateCollisionBody();
		initialized = true;
	}
}

void ColliderComponent::Start() {
    Initialize();
}

void ColliderComponent::Update(float deltaTime) {
    if (isStatic) {
        UpdateTransform();
    }
}

void ColliderComponent::Destroy() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
    else if (collisionShape) {
        delete collisionShape;
        collisionShape = nullptr;
    }
}

void ColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (collisionShape) {
        btVector3 halfExtents(size.x, size.y, size.z);
        static_cast<btBoxShape*>(collisionShape)->setImplicitShapeDimensions(halfExtents);

        if (rigidBody && !isStatic) {
            btVector3 localInertia(0, 0, 0);
            collisionShape->calculateLocalInertia(rigidBody->getMass(), localInertia);
            rigidBody->setMassProps(rigidBody->getMass(), localInertia);
        }
    }
}

void ColliderComponent::SetTrigger(bool trigger) {
    isTrigger = trigger;
    if (rigidBody) {
        if (trigger) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

void ColliderComponent::CreateCollisionBody() {
    if (!owner) return;

    BoundingBox bbox = owner->boundingBox();
    size = bbox.size();

    btVector3 halfExtents(size.x, size.y, size.z);
    collisionShape = new btBoxShape(halfExtents);

    btTransform startTransform;
    startTransform.setIdentity();

    auto transform = owner->GetTransform();
    if (transform) {
        glm::vec3 position = transform->GetPosition();
        glm::quat rotation = transform->GetRotation();

        startTransform.setOrigin(btVector3(position.x, position.y, position.z));
        startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, collisionShape, btVector3(0, 0, 0));
    rigidBody = new btRigidBody(rbInfo);

    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

    if (isTrigger) {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    physics->dynamicsWorld->addRigidBody(rigidBody);
    physics->gameObjectRigidBodyMap[owner] = rigidBody;
}

void ColliderComponent::UpdateTransform() {
    if (!rigidBody || !owner) return;

    auto transform = owner->GetTransform();
    if (!transform) return;

    btTransform btTrans;
    rigidBody->getMotionState()->getWorldTransform(btTrans);

    glm::vec3 position = transform->GetPosition();
    glm::quat rotation = transform->GetRotation();

    btTrans.setOrigin(btVector3(position.x, position.y, position.z));
    btTrans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    rigidBody->getMotionState()->setWorldTransform(btTrans);
    rigidBody->setWorldTransform(btTrans);
}

void ColliderComponent::SetColliderPosition(const glm::vec3& position) {
    if (rigidBody) {
        btTransform transform;
        rigidBody->getMotionState()->getWorldTransform(transform);
        transform.setOrigin(btVector3(position.x, position.y, position.z));
        rigidBody->getMotionState()->setWorldTransform(transform);
        rigidBody->setWorldTransform(transform);
    }
}

void ColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    if (rigidBody) {
        btTransform transform;
        rigidBody->getMotionState()->getWorldTransform(transform);
        transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
        rigidBody->getMotionState()->setWorldTransform(transform);
        rigidBody->setWorldTransform(transform);
    }
}

glm::vec3 ColliderComponent::GetColliderPosition() const {
    if (rigidBody) {
        btTransform transform;
        rigidBody->getMotionState()->getWorldTransform(transform);
        btVector3 pos = transform.getOrigin();
        return glm::vec3(pos.x(), pos.y(), pos.z());
    }
    return glm::vec3(0.0f);
}

glm::quat ColliderComponent::GetColliderRotation() const {
    if (rigidBody) {
        btTransform transform;
        rigidBody->getMotionState()->getWorldTransform(transform);
        btQuaternion rot = transform.getRotation();
        return glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
    }
    return glm::quat();
}

std::unique_ptr<Component> ColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<ColliderComponent>(new_owner, physics);
}