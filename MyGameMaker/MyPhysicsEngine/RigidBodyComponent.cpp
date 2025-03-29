
#include "RigidbodyComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <MyScriptingEngine/MonoManager.h>
#include <mono/metadata/debug-helpers.h>
#include "CapsuleColliderComponent.h"
#include "MeshColliderComponent.h"

RigidbodyComponent::RigidbodyComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner), physics(physicsModule)
{
    name = "RigidbodyComponent";
    Start();  
    updateInStop = true;
}

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

void RigidbodyComponent::Init() {
    if (!rigidBody) {
		Start();        
    }
    SetMass(mass);
    SetGravity(gravity);
    SetFriction(friction);
    SetKinematic(isKinematic);
    SetFreezeRotations(isFreezed);
	SetDamping(damping.x, damping.y);
}

void RigidbodyComponent::Start() {
    auto boxCollider = owner->GetComponent<BoxColliderComponent>();
    auto capsuleCollider = owner->GetComponent<CapsuleColliderComponent>();
    auto meshCollider = owner->GetComponent<MeshColliderComponent>();
    bool hasCollider = false;
    BaseColliderComponent* colliderComponent = nullptr;
    
    if (boxCollider) {
        hasCollider = true;
        colliderComponent = boxCollider;
    } else if (capsuleCollider) {
        hasCollider = true;
        colliderComponent = capsuleCollider;
    } else if (meshCollider) {
        hasCollider = true;
        colliderComponent = meshCollider;
    }

    if (!hasCollider) {
        colliderComponent = owner->AddComponent<BoxColliderComponent>(physics);
        colliderComponent->Start();
    }
    
    this->rigidBody = colliderComponent->GetRigidBody();
    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    Init();
}


void RigidbodyComponent::Update(float deltaTime) 
{
    
}

void RigidbodyComponent::Destroy() {}



void RigidbodyComponent::SetMass(float newMass) {
    mass = newMass;

    if (!rigidBody) return;

    btVector3 localInertia(0, 0, 0);
    btCollisionShape* shape = rigidBody->getCollisionShape();
    if (!shape) return;

    if (newMass > 0.0f) {
        shape->calculateLocalInertia(newMass, localInertia);
    }

    btTransform currentTransform;
    rigidBody->getMotionState()->getWorldTransform(currentTransform);



    physics->dynamicsWorld->removeRigidBody(rigidBody);

    rigidBody->setMassProps(newMass, localInertia);
    rigidBody->updateInertiaTensor();
    rigidBody->getMotionState()->setWorldTransform(currentTransform);
    rigidBody->setWorldTransform(currentTransform);

    physics->dynamicsWorld->addRigidBody(rigidBody);

    rigidBody->activate();
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
    return gravity;
}

void RigidbodyComponent::SetGravity(const glm::vec3& newGravity) {
	gravity = newGravity;  
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

void RigidbodyComponent::SetFriction(float newFriction) {
	friction = newFriction;
    if (rigidBody) {
        rigidBody->setFriction(friction);
    }
}

float RigidbodyComponent::GetFriction() const {
    return friction;
}

void RigidbodyComponent::SetDamping(float linearDamping, float angularDamping) {
    damping = glm::vec2(linearDamping, angularDamping);
    if (rigidBody) {
        rigidBody->setDamping(linearDamping, angularDamping);
    }
}

glm::vec2 RigidbodyComponent::GetDamping() const {
    return damping;
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

    return monoObject;
}

