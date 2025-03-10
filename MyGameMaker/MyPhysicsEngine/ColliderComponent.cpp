
#pragma once

#include "ColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"

ColliderComponent::ColliderComponent(GameObject* owner, PhysicsModule* physicsModule, bool isForStreet) : Component(owner) 
{ 
    name = "ColliderComponent"; 
    physics = physicsModule; 
    isForStreetLocal = isForStreet;
    snapToPosition = false;
}

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Start() {
	if (rigidBody) {
		physics->dynamicsWorld->removeRigidBody(rigidBody);
		delete rigidBody->getMotionState();
		delete rigidBody;
		rigidBody = nullptr;
	}
    CreateCollider();
}


void ColliderComponent::OnCollisionEnter(ColliderComponent* other) {
    std::cout << "EnterCollision" << std::endl;
}

void ColliderComponent::OnCollisionStay(ColliderComponent* other) {
    std::cout << "StayColliding" << std::endl;
}

void ColliderComponent::OnCollisionExit(ColliderComponent* other) {
    std::cout << "ExitCollision" << std::endl;
}

void ColliderComponent::OnTriggerEnter(ColliderComponent* other) {
    std::cout << "EnterCollisionTriggered" << std::endl;
}

void ColliderComponent::OnTriggerStay(ColliderComponent* other) {
    std::cout << "StayCollidingTriggered" << std::endl;
}

void ColliderComponent::OnTriggerExit(ColliderComponent* other) {
    std::cout << "ExitCollisionTriggered" << std::endl;
}


void ColliderComponent::Update(float deltaTime) {
    if (snapToPosition && owner && !physics->linkPhysicsToScene) {
		SnapToPosition();
    }
}


void ColliderComponent::Destroy() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }

    if (physics->gameObjectRigidBodyMap.find(owner) != physics->gameObjectRigidBodyMap.end()) {
        physics->gameObjectRigidBodyMap.erase(owner);
    }
}

std::unique_ptr<Component> ColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<ColliderComponent>(new_owner, physics);
}



MonoObject* ColliderComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Collider");
    if (!klass) {
        return nullptr;
    }

    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        return nullptr;
    }

    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.Collider:.ctor(uintptr,HawkEngine.GameObject)", true);
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



void ColliderComponent::SetTrigger(bool trigger) {
    if (rigidBody) {
        if (trigger) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

bool ColliderComponent::IsTrigger() const {
    return (rigidBody && (rigidBody->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
}

glm::vec3 ColliderComponent::GetColliderPos() {

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat ColliderComponent::GetColliderRotation() {

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void ColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
	trans.setRotation(btQuaternion(rotation.w, rotation.x, rotation.y, rotation.z));

    btVector3 currentPosition = trans.getOrigin();
    trans.setOrigin(currentPosition);

    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setWorldTransform(trans);
}

void ColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setCenterOfMassTransform(trans);
}

glm::vec3 ColliderComponent::GetSize() {
    if (rigidBody) {
        btCollisionShape* shape = rigidBody->getCollisionShape();
        if (shape) {
            btVector3 scale = shape->getLocalScaling();
            return glm::vec3(scale.getX() * 2.0f, scale.getY() * 2.0f, scale.getZ() * 2.0f);
        }
    }
    return size;
}

void ColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (rigidBody) {
        btCollisionShape* shape = rigidBody->getCollisionShape();
        if (shape) {
            btVector3 newBtSize(newSize.x * 0.5f, newSize.y * 0.5f, newSize.z * 0.5f);
            shape->setLocalScaling(newBtSize);
        }
    }
}
void ColliderComponent::SetMass(float newMass) {
    mass = newMass;
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
    CreateCollider();
}

void ColliderComponent::SetActive(bool active) {
    if (rigidBody) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(rigidBody);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(rigidBody);
        }
    }
}void ColliderComponent::SnapToPosition() {
    if (!owner) return;

    Transform_Component* goTransform = owner->GetTransform();
    if (!goTransform) return;

    // Obtener el Bounding Box en espacio local (sin escala aplicada)
    BoundingBox localBBox = owner->localBoundingBox();
    glm::vec3 localCenter = localBBox.center();
    glm::vec3 localSize = localBBox.size();

    // Obtener la escala real del objeto
    glm::vec3 worldScale = goTransform->GetScale();

    // **Calcular el tamaño real del collider sin depender de la escala del GameObject**
    glm::vec3 adjustedSize = localSize * worldScale;

    // Obtener la posición en el mundo y ajustar el centro del collider
    glm::vec3 worldPosition = goTransform->GetPosition();
    glm::quat worldRotation = goTransform->GetLocalRotation();
    glm::vec3 adjustedPosition = worldPosition + worldRotation * (localCenter * worldScale);

    // Aplicar la transformación al rigidBody
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(adjustedPosition.x, adjustedPosition.y, adjustedPosition.z));
    transform.setRotation(btQuaternion(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w));

    if (rigidBody->getMotionState()) {
        rigidBody->getMotionState()->setWorldTransform(transform);
    }
    else {
        rigidBody->setWorldTransform(transform);
    }
    rigidBody->setCenterOfMassTransform(transform);

    // **Aplicar la escala al collider basándose en la magnitud real del bounding box**
    btVector3 adjustedScale(adjustedSize.x * 0.5f,
        adjustedSize.y * 0.5f,
        adjustedSize.z * 0.5f);

    if (rigidBody->getCollisionShape()) {
        //rigidBody->getCollisionShape()->setLocalScaling(adjustedScale);
    }

    std::cout << "Collider adjusted position: (" << adjustedPosition.x << ", " << adjustedPosition.y << ", " << adjustedPosition.z
        << ") with adjusted scale: (" << adjustedSize.x << ", " << adjustedSize.y << ", " << adjustedSize.z << ")\n";
}






//Local BBox Adjusted (doesnt works with the blocking)

void ColliderComponent::CreateCollider() {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    BoundingBox bbox = owner->localBoundingBox();
    size = bbox.size();

    glm::vec3 bboxCenter = owner->boundingBox().center();

    btCollisionShape* shape;
    btTransform startTransform;
    startTransform.setIdentity();

    glm::quat localRotation = transform->GetLocalRotation();
    btQuaternion btLocalRotation(localRotation.x, localRotation.y, localRotation.z, localRotation.w);

    shape = new btBoxShape(btVector3(size.x * 0.5, size.y * 0.5, size.z * 0.5));

    startTransform.setOrigin(btVector3(bboxCenter.x, bboxCenter.y, bboxCenter.z));
    startTransform.setRotation(btLocalRotation);

    glm::vec3 scale = transform->GetScale();
    shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    rigidBody = new btRigidBody(rbInfo);

    // Add the collider to the physics world
    physics->dynamicsWorld->addRigidBody(rigidBody);
    physics->gameObjectRigidBodyMap[owner] = rigidBody;
}




//Previous Create Collider (doesn´t adjust to local bbx
//
//
//void ColliderComponent::CreateCollider() {
//	if (!owner) return;
//
//	Transform_Component* transform = owner->GetTransform();
//	if (!transform) return;
//
//	BoundingBox localBBox = owner->boundingBox();
//	size = localBBox.size();
//	glm::dvec3 localCenter = localBBox.center();
//
//	glm::dvec3 worldPosition = transform->GetPosition();
//	glm::dquat worldRotation = transform->GetLocalRotation();
//	glm::dvec3 worldScale = transform->GetScale();
//
//	btCollisionShape* shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
//	//shape->setLocalScaling(btVector3(worldScale.x, worldScale.y, worldScale.z));
//
//	glm::vec3 rotatedCenter = worldPosition + worldRotation * (localCenter - transform->GetLocalPosition());
//
//	btTransform startTransform;
//	startTransform.setIdentity();
//	startTransform.setOrigin(btVector3(rotatedCenter.x, rotatedCenter.y, rotatedCenter.z));
//	startTransform.setRotation(btQuaternion(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w));
//
//	btVector3 localInertia(0, 0, 0);
//	if (mass > 0.0f) {
//		shape->calculateLocalInertia(mass, localInertia);
//	}
//
//	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
//	rigidBody = new btRigidBody(rbInfo);
//
//	physics->dynamicsWorld->addRigidBody(rigidBody);
//	physics->gameObjectRigidBodyMap[owner] = rigidBody;
//}