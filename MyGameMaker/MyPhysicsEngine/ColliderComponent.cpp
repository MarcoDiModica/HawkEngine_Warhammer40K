#include "ColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "RigidBodyComponent.h"

ColliderComponent::ColliderComponent(GameObject* owner, PhysicsModule* physicsModule) : Component(owner)
{
    name = "ColliderComponent";
    physics = physicsModule;
}

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Start() {
	//si tiene collider, lo destruye y crea uno nuevo 
    // TODO: Revisar
    if (collider) {
        physics->dynamicsWorld->removeRigidBody(collider);
        delete collider->getMotionState();
        delete collider;
        collider = nullptr;
    }
    CreateCollider();
}

//OnCollisions y triggers 
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
    if (owner) {
       SnapToPosition();
    }
}


void ColliderComponent::Destroy() {
    if (collider) {
        physics->dynamicsWorld->removeRigidBody(collider);
        delete collider->getMotionState();
        delete collider;
        collider = nullptr;
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
    if (collider) {
        if (trigger) {
            collider->setCollisionFlags(collider->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            collider->setCollisionFlags(collider->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

bool ColliderComponent::IsTrigger() const {
    return (collider && (collider->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
}


//Refactoring UIinspector 
// Ofset, size
glm::vec3 ColliderComponent::GetColliderPos() {

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat ColliderComponent::GetColliderRotation() {

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void ColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    if (!collider || !collider->getMotionState()) {
        return;
    }

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);

    // Convert to btQuaternion 
    btQuaternion btRot(rotation.x, rotation.y, rotation.z, rotation.w);
    trans.setRotation(btRot);

    collider->getMotionState()->setWorldTransform(trans);
    collider->setWorldTransform(trans);

    // Ensure activation in case the object is sleeping
    collider->activate();
}

void ColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    collider->getMotionState()->setWorldTransform(trans);
    collider->setCenterOfMassTransform(trans);
}

glm::vec3 ColliderComponent::GetSize() {
    if (collider) {
        btCollisionShape* shape = collider->getCollisionShape();
        if (shape) {
            btVector3 scale = shape->getLocalScaling();

            Transform_Component* transform = owner->GetTransform();
            if (transform) {
                glm::vec3 worldScale = transform->GetScale();
                return glm::vec3(scale.getX() * worldScale.x,
                    scale.getY() * worldScale.y,
                    scale.getZ() * worldScale.z);
            }

            return glm::vec3(scale.getX(), scale.getY(), scale.getZ());
        }
    }
    return size;
}


glm::vec3 ColliderComponent::GetOffset() {
    if (!collider || !owner) return glm::vec3(0.0f);

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return glm::vec3(0.0f);

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btVector3 rbPos = trans.getOrigin();

    glm::vec3 ownerPos = transform->GetPosition();
    return glm::vec3(rbPos.getX(), rbPos.getY(), rbPos.getZ()) - ownerPos;
}

void ColliderComponent::SetOffset(const glm::vec3& newoffset) {
    if (offset != newoffset) {
        offset = newoffset;
    }
    if (!collider || !owner) {
        offset = glm::vec3(0.0f);
        return;
    }

    Transform_Component* transform = owner->GetTransform();
    if (!transform) {
        offset = glm::vec3(0.0f);
        return;
    }

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btVector3 rbPos = trans.getOrigin();

    glm::vec3 ownerPos = transform->GetPosition();
    offset = glm::vec3(rbPos.getX(), rbPos.getY(), rbPos.getZ()) - ownerPos;
}

void ColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (collider) {
        btCollisionShape* shape = collider->getCollisionShape();
        if (shape) {
            btVector3 newBtSize(newSize.x * 0.5f, newSize.y * 0.5f, newSize.z * 0.5f);
            shape->setLocalScaling(newBtSize);
        }
    }
}
void ColliderComponent::SetMass(float newMass) {
    mass = newMass;
    if (collider) {
        physics->dynamicsWorld->removeRigidBody(collider);
        delete collider->getMotionState();
        delete collider;
        collider = nullptr;
    }
    CreateCollider();
}

void ColliderComponent::SetActive(bool active) {
    if (collider) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(collider);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(collider);
        }
    }
}
void ColliderComponent::SnapToPosition() {
    if (!owner || !collider) return;
    if (owner->HasComponent<RigidbodyComponent>()) return;

    Transform_Component* goTransform = owner->GetTransform();
    if (!goTransform) return;
    // Obtener el Bounding Box en espacio local (sin escala aplicada)
    BoundingBox localBBox = owner->localBoundingBox();
    glm::vec3 localCenter = localBBox.center();
    glm::vec3 localSize = localBBox.size();

    glm::vec3 worldScale = goTransform->GetScale();
    glm::vec3 parentScale(1.0f);
    if (owner->GetParent()) {
        parentScale = owner->GetParent()->GetTransform()->GetScale();
    }
    glm::vec3 finalScale = worldScale * parentScale;

    glm::vec3 worldPosition = goTransform->GetPosition();
    glm::quat worldRotation = goTransform->GetRotation();
    glm::vec3 adjustedPosition = worldPosition + worldRotation * (localCenter * finalScale);

    glm::vec3 targetPosition = adjustedPosition + offset;

    btTransform currentTransform;
    collider->getMotionState()->getWorldTransform(currentTransform);
    btVector3 currentOrigin = currentTransform.getOrigin();

    if (glm::distance(glm::vec3(currentOrigin.x(), currentOrigin.y(), currentOrigin.z()), targetPosition) < 0.001f) {
        return;
    }
    // Aplicar la transformación al rigidBody
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(adjustedPosition.x, adjustedPosition.y, adjustedPosition.z));
    transform.setRotation(btQuaternion(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w));


    if (collider->getMotionState()) {
        collider->getMotionState()->setWorldTransform(transform);
    }
    else {
        collider->setWorldTransform(transform);
    }
    collider->setCenterOfMassTransform(transform);

    if (collider->getCollisionShape()) {
        btVector3 btScale(finalScale.x, finalScale.y, finalScale.z);
        collider->getCollisionShape()->setLocalScaling(btScale);
    }
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

    if (size.x == 0.0f && size.y == 0.0f && size.z == 0.0f) {
        size = glm::vec3(1.0f, 1.0f, 1.0f);
        bboxCenter = transform->GetLocalPosition();
    }

    shape = new btBoxShape(btVector3(size.x * 0.5, size.y * 0.5, size.z * 0.5));
    glm::vec3 localPosition = transform->GetLocalPosition();
    startTransform.setOrigin(btVector3(bboxCenter.x, bboxCenter.y, bboxCenter.z));
    glm::dquat localRot = transform->GetRotation();
    btQuaternion btRot(
        static_cast<btScalar>(localRot.x),
        static_cast<btScalar>(localRot.y),
        static_cast<btScalar>(localRot.z),
        static_cast<btScalar>(localRot.w)
    );
    startTransform.setRotation(btRot);
    glm::vec3 scale = transform->GetScale();
    glm::vec3 parentScale(1.0f);
    if (owner->GetParent()) {
        parentScale = owner->GetParent()->GetTransform()->GetScale();
    }
    glm::vec3 finalScale = scale * parentScale;
    shape->setLocalScaling(btVector3(finalScale.x, finalScale.y, finalScale.z));

    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    collider = new btRigidBody(rbInfo);

    // Add the collider to the physics world
    physics->dynamicsWorld->addRigidBody(collider);
    physics->gameObjectRigidBodyMap[owner] = collider;
}

