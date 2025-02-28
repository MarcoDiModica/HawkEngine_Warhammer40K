
#include "RigidbodyComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <MyScriptingEngine/MonoManager.h>
#include <mono/metadata/debug-helpers.h>

RigidbodyComponent::RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner), physics(physicsModule), mass(1.0f) {}

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

void RigidbodyComponent::Start() {
    ColliderComponent* collider = owner->GetComponent<ColliderComponent>();
    if (!collider) {
        owner->AddComponent<ColliderComponent>(physics);
        collider = owner->GetComponent<ColliderComponent>();
    }
	collider->SetMass(mass);
    SetRigidBody(collider->GetRigidBody());
    
}

void RigidbodyComponent::SetRigidBody(btRigidBody* rigidBody) {
	this->rigidBody = rigidBody;
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void RigidbodyComponent::Update(float deltaTime) {}

void RigidbodyComponent::Destroy() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void RigidbodyComponent::SetMass(float newMass) {
	mass = newMass;
    ColliderComponent* collider = owner->GetComponent<ColliderComponent>();
	collider->SetMass(newMass);
    SetRigidBody(collider->GetRigidBody());
	SetFreezeRotations(false);
}

float RigidbodyComponent::GetMass() const {
    return mass;
}

void RigidbodyComponent::AddForce(const glm::vec3& force) {
    if (rigidBody) {
        rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
    }
}

glm::vec3 RigidbodyComponent::GetGravity() const {
    if (rigidBody) {
        btVector3 gravity = rigidBody->getGravity();
        return glm::vec3(gravity.getX(), gravity.getY(), gravity.getZ());
    }
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

void RigidbodyComponent::SetGravity(const glm::vec3& gravity) {
    if (rigidBody) {
        rigidBody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    }
}

void RigidbodyComponent::SetFreezeRotations(bool freeze) {
    if (rigidBody) {
        if (freeze) 
        {
            rigidBody->setAngularFactor(btVector3(0, 0, 0));
        } 
        else 
        {
            rigidBody->setAngularFactor(btVector3(1, 1, 1));
        }
    }
	isFreezed = freeze;
}

void RigidbodyComponent::SetFriction(float friction) {
    if (rigidBody) {
        rigidBody->setFriction(friction);
    }
}

float RigidbodyComponent::GetFriction() const {
    if (rigidBody) {
        return rigidBody->getFriction();
    }
    return 0.0f;
}

void RigidbodyComponent::SetDamping(float linearDamping, float angularDamping) {
    
    if (rigidBody) {
        rigidBody->setDamping(linearDamping, angularDamping);
    }
}

glm::vec2 RigidbodyComponent::GetDamping() const {
    if (rigidBody) {
        return glm::vec2(rigidBody->getLinearDamping(), rigidBody->getAngularDamping());
    }
    return glm::vec2(0.0f, 0.0f);
}

void RigidbodyComponent::SetKinematic(bool isKinematic) {
    if (rigidBody) {
        if (isKinematic) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        }
    }
    this->isKinematic = isKinematic;
}

bool RigidbodyComponent::IsKinematic() const {
    return isKinematic;
}

void RigidbodyComponent::EnableContinuousCollision() {
    if (rigidBody) {
        rigidBody->setCcdMotionThreshold(0.01);
        rigidBody->setCcdSweptSphereRadius(0.05);
    }
}


std::unique_ptr<Component> RigidbodyComponent::Clone(GameObject* new_owner) {
	return std::make_unique<RigidbodyComponent>(new_owner, physics);
}


MonoObject* RigidbodyComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Rigidbody");
    if (!klass) {
        return nullptr;
    }

    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        return nullptr;
    }

    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.Rigidbody:.ctor(uintptr,HawkEngine.GameObject)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    if (!method)
    {
        return nullptr;
    }

    uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
    MonoObject* ownerGo = owner->GetSharp();
    if (!ownerGo)
    {
        return nullptr;
    }

    void* args[2]{};
    args[0] = &componentPtr;
    args[1] = ownerGo;

    mono_runtime_invoke(method, monoObject, args, nullptr);

    CsharpReference = monoObject;
    return CsharpReference;
}
