#include "BaseColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "RigidBodyComponent.h"

BaseColliderComponent::BaseColliderComponent(GameObject* owner, PhysicsModule* physicsModule) : Component(owner)
{
	
}

BaseColliderComponent::~BaseColliderComponent() {
    Destroy();
}


void BaseColliderComponent::Start() {
    if (!collider) {
        CreateCollider();
    }
}

void BaseColliderComponent::Update(float deltaTime) {
    if (owner) {
        SnapToPosition();
    }
}

void BaseColliderComponent::Destroy() {
    if (collider) {
        physics->dynamicsWorld->removeRigidBody(collider);
        delete collider->getMotionState();
        delete collider;
        collider = nullptr;
        if (physics->gameObjectRigidBodyMap.find(owner) != physics->gameObjectRigidBodyMap.end()) {
            physics->gameObjectRigidBodyMap.erase(owner);
        }
    }
    
}

//OnCollisions y triggers 
void BaseColliderComponent::OnCollisionEnter(BaseColliderComponent* other) {
}

void BaseColliderComponent::OnCollisionStay(BaseColliderComponent* other) {
}

void BaseColliderComponent::OnCollisionExit(BaseColliderComponent* other) {
}

void BaseColliderComponent::OnTriggerEnter(BaseColliderComponent* other) {
}

void BaseColliderComponent::OnTriggerStay(BaseColliderComponent* other) {
}

void BaseColliderComponent::OnTriggerExit(BaseColliderComponent* other) {
}


void BaseColliderComponent::SetTrigger(bool trigger) {
    if (collider) {
        if (trigger) {
            collider->setCollisionFlags(collider->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            collider->setCollisionFlags(collider->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

bool BaseColliderComponent::IsTrigger() const {
    return (collider && (collider->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
}

glm::vec3 BaseColliderComponent::GetColliderPos() {

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat BaseColliderComponent::GetColliderRotation() {

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void BaseColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    if (!collider || !collider->getMotionState()) {
        return;
    }

    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);

    btQuaternion btRot(rotation.x, rotation.y, rotation.z, rotation.w);
    trans.setRotation(btRot);

    collider->getMotionState()->setWorldTransform(trans);
    collider->setWorldTransform(trans);

    collider->activate();
}

void BaseColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    collider->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    collider->getMotionState()->setWorldTransform(trans);
    collider->setCenterOfMassTransform(trans);
}

glm::vec3 BaseColliderComponent::GetSize() {
    return size;
}


glm::vec3 BaseColliderComponent::GetOffset() {
    return offset;
}

void BaseColliderComponent::SetOffset(const glm::vec3& newoffset) {
    if (offset != newoffset) {
        offset = newoffset;
    }
}


void BaseColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (collider) {
        btCollisionShape* shape = collider->getCollisionShape();
        if (shape) {
            btVector3 newBtSize(size.x * owner->GetTransform()->GetScale().x, size.y * owner->GetTransform()->GetScale().y, size.z * owner->GetTransform()->GetScale().z);
            shape->setLocalScaling(newBtSize);
        }
    }
}
void BaseColliderComponent::SetActive(bool active) {
    if (collider) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(collider);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(collider);
        }
    }
}

void BaseColliderComponent::SnapToPosition() {
    if (!owner || !collider) return;
    RigidbodyComponent* rigidbody = owner->GetComponent<RigidbodyComponent>();
    Transform_Component* goTransform = owner->GetTransform();
    if (!goTransform) return;

    //For release
    if (physics->IsForRelease()) {
        if (hasSnappedToInitialPosition) return;

        glm::vec3 worldPosition = goTransform->GetPosition();

        glm::vec3 colliderOffset = GetOffset();
        glm::vec3 finalColliderPosition = worldPosition + colliderOffset;

        if (rigidbody) {
            btRigidBody* rb = rigidbody->GetRigidBody();
            if (rb) {
                btTransform newTransform;
                newTransform.setIdentity();
                newTransform.setOrigin(btVector3(finalColliderPosition.x, finalColliderPosition.y, finalColliderPosition.z));

                rb->getMotionState()->setWorldTransform(newTransform);
                rb->setWorldTransform(newTransform);
                rb->activate();

                hasSnappedToInitialPosition = true;
            }
        }
    }
    else
    {
        if (physics->linkPhysicsToScene && rigidbody && !rigidbody->IsKinematic()) {
            return;
        }
    }

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
    btQuaternion currentRotation = currentTransform.getRotation();
    btVector3 currentScale = collider->getCollisionShape()->getLocalScaling();

    glm::vec3 currentPosition(currentOrigin.x(), currentOrigin.y(), currentOrigin.z());
    glm::quat currentQuat(currentRotation.w(), currentRotation.x(), currentRotation.y(), currentRotation.z());
    glm::vec3 currentColliderScale(currentScale.x(), currentScale.y(), currentScale.z());

    bool positionChanged = glm::distance(currentPosition, targetPosition) >= 0.001f;
    bool rotationChanged = glm::angle(glm::normalize(glm::normalize(worldRotation) * glm::inverse(glm::normalize(currentQuat)))) >= 0.001f;
    bool scaleChanged = glm::distance(currentColliderScale, finalScale) >= 0.001f;

    if (!positionChanged && !rotationChanged && !scaleChanged) {
        return;
    }

    btTransform transform;
    transform.setIdentity();

    //SetSize
    if (collider->getCollisionShape()) {
        btCollisionShape* shape = collider->getCollisionShape();

        btVector3 scaledSize(size.x * finalScale.x, size.y * finalScale.y, size.z * finalScale.z);

        shape->setLocalScaling(scaledSize);
    }

    //Set Transform
    if (collider->getMotionState()) {
        collider->getMotionState()->getWorldTransform(currentTransform);
    }
    else {
        currentTransform = collider->getWorldTransform();
    }

    glm::vec3 adjustedOffset = offset;
    glm::quat combinedRotation = worldRotation;
    if (owner->GetParent()) {
        Transform_Component* parentTransform = owner->GetParent()->GetTransform();
        if (parentTransform) {
            glm::quat parentRotation = parentTransform->GetRotation();
            adjustedOffset = parentRotation * offset;
            combinedRotation = parentRotation * worldRotation;
        }
    }
    currentTransform.setOrigin(btVector3(worldPosition.x + adjustedOffset.x,
        worldPosition.y + adjustedOffset.y,
        worldPosition.z + adjustedOffset.z));

    btQuaternion btCombinedRotation(
        static_cast<btScalar>(combinedRotation.x),
        static_cast<btScalar>(combinedRotation.y),
        static_cast<btScalar>(combinedRotation.z),
        static_cast<btScalar>(combinedRotation.w)
    );
    currentTransform.setRotation(btCombinedRotation);

    if (collider->getMotionState()) {
        collider->getMotionState()->setWorldTransform(currentTransform);
    }
    else {
        collider->setWorldTransform(currentTransform);
    }

    collider->setCenterOfMassTransform(currentTransform);
}