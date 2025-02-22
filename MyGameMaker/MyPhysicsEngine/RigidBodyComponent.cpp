#include "RigidbodyComponent.h"
#include "ColliderComponent.h"
#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include <bullet/btBulletDynamicsCommon.h>

RigidbodyComponent::RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner)
    , physics(physicsModule)
    , rigidBody(nullptr)
    , mass(10.0f)
    , useGravity(true)
    , kinematic(false)
    , initialized(false) 
{
    name = "RigidbodyComponent";
}

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

void RigidbodyComponent::Start() {
    if (!initialized) {
        ColliderComponent* collider = GetOrCreateCollider();
        if (collider) {
            collider->Initialize();
            if (collider->IsInitialized()) {
                InitializeRigidbody();
                initialized = true;
            }
        }
    }
}

void RigidbodyComponent::InitializeRigidbody() {
    ColliderComponent* collider = GetOrCreateCollider();
    if (!collider || !collider->IsInitialized() || !collider->GetCollisionShape()) {
        std::cout << "Failed to initialize RigidbodyComponent: ColliderComponent not ready" << std::endl;
        return;
    }

    collider->Destroy();

    auto transform = owner->GetTransform();
    if (!transform) return;

    btTransform startTransform;
    startTransform.setIdentity();

    glm::vec3 position = transform->GetPosition();
    startTransform.setOrigin(btVector3(position.x, position.y, position.z));

    glm::quat rotation = transform->GetRotation();
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f && !kinematic) {
        collider->GetCollisionShape()->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider->GetCollisionShape(), localInertia);
    rigidBody = new btRigidBody(rbInfo);

    rigidBody->setGravity(useGravity ? btVector3(0, -9.81f, 0) : btVector3(0, 0, 0));
    if (kinematic) {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    }

    physics->dynamicsWorld->addRigidBody(rigidBody);
    physics->gameObjectRigidBodyMap[owner] = rigidBody;
}

ColliderComponent* RigidbodyComponent::GetOrCreateCollider() {
    if (owner->HasComponent<ColliderComponent>()) {
		return owner->GetComponent<ColliderComponent>();
	}
    else
    {
        ColliderComponent* collider = owner->AddComponent<ColliderComponent>(physics);
        collider->Initialize();
        return collider;
    }

    return nullptr;
}

void RigidbodyComponent::SetMass(float newMass) {
    mass = newMass;
    if (rigidBody) {
        btVector3 localInertia(0, 0, 0);
        if (mass > 0.0f && !kinematic) {
            rigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
        }
        rigidBody->setMassProps(mass, localInertia);
    }
}

void RigidbodyComponent::SetGravity(bool enable) {
    useGravity = enable;
    if (rigidBody) {
        rigidBody->setGravity(useGravity ? btVector3(0, -9.81f, 0) : btVector3(0, 0, 0));
    }
}

void RigidbodyComponent::SetKinematic(bool isKinematic) {
    kinematic = isKinematic;
    if (rigidBody) {
        if (kinematic) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        }
    }
}

void RigidbodyComponent::AddForce(const glm::vec3& force) {
    if (rigidBody && !kinematic) {
        rigidBody->activate(true);
        rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
    }
}

void RigidbodyComponent::AddTorque(const glm::vec3& torque) {
    if (rigidBody && !kinematic) {
        rigidBody->activate(true);
        rigidBody->applyTorque(btVector3(torque.x, torque.y, torque.z));
    }
}

void RigidbodyComponent::SetVelocity(const glm::vec3& velocity) {
    if (rigidBody && !kinematic) {
        rigidBody->activate(true); 
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void RigidbodyComponent::Update(float deltaTime) {
}

void RigidbodyComponent::Destroy() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
}

std::unique_ptr<Component> RigidbodyComponent::Clone(GameObject* new_owner) {
    return std::make_unique<RigidbodyComponent>(new_owner, physics);
}