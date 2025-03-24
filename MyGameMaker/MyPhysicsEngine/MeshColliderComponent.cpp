#include "MeshColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "RigidBodyComponent.h"

MeshColliderComponent::MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner), physics(physicsModule), meshCollider(nullptr) 
{
	name = "MeshColliderComponent";
	Start();
	updateInStop = false;
}

MeshColliderComponent::~MeshColliderComponent() {
    Destroy();
}

void MeshColliderComponent::Start() {
    if (!meshCollider) {
        CreateMeshCollider();
    }
}

void MeshColliderComponent::Update(float deltaTime) {
    if (owner) {
        SnapToPosition();
    }
}

void MeshColliderComponent::Destroy() {
    if (meshCollider) {
        physics->dynamicsWorld->removeRigidBody(meshCollider);
        delete meshCollider->getMotionState();
        delete meshCollider;
        meshCollider = nullptr;
    }

    if (physics->gameObjectRigidBodyMap.find(owner) != physics->gameObjectRigidBodyMap.end()) {
        physics->gameObjectRigidBodyMap.erase(owner);
    }
}

std::unique_ptr<Component> MeshColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<MeshColliderComponent>(new_owner, physics);
}


void MeshColliderComponent::SetTrigger(bool trigger) {
    if (meshCollider) {
        if (trigger) {
            meshCollider->setCollisionFlags(meshCollider->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            meshCollider->setCollisionFlags(meshCollider->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

bool MeshColliderComponent::IsTrigger() const {
    return (meshCollider && (meshCollider->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
}

glm::vec3 MeshColliderComponent::GetColliderPos() {
    btTransform trans;
    meshCollider->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat MeshColliderComponent::GetColliderRotation() {
    btTransform trans;
    meshCollider->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void MeshColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    if (!meshCollider || !meshCollider->getMotionState()) {
        return;
    }

    btTransform trans;
    meshCollider->getMotionState()->getWorldTransform(trans);
    trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    meshCollider->getMotionState()->setWorldTransform(trans);
    meshCollider->setWorldTransform(trans);
    meshCollider->activate();
}

void MeshColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    meshCollider->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    meshCollider->getMotionState()->setWorldTransform(trans);
    meshCollider->setCenterOfMassTransform(trans);
}

glm::vec3 MeshColliderComponent::GetSize() {
    return size;
}

void MeshColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (meshCollider) {
        btCollisionShape* shape = meshCollider->getCollisionShape();
        if (shape) {
            btVector3 newBtSize(size.x * owner->GetTransform()->GetScale().x, size.y * owner->GetTransform()->GetScale().y, size.z * owner->GetTransform()->GetScale().z);
            shape->setLocalScaling(newBtSize);
        }
    }
}

void MeshColliderComponent::SetActive(bool active) {
    if (meshCollider) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(meshCollider);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(meshCollider);
        }
    }
}


glm::vec3 MeshColliderComponent::GetOffset() {
    return offset;
}

void MeshColliderComponent::SetOffset(const glm::vec3& newoffset) {
    if (offset != newoffset) {
        offset = newoffset;
    }
}


void MeshColliderComponent::SnapToPosition() {
    if (!owner || !meshCollider) return;
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
        if (physics->linkPhysicsToScene && rigidbody) {
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
    //glm::vec3 finalScale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 worldPosition = goTransform->GetPosition();
    glm::quat worldRotation = goTransform->GetRotation();
    glm::vec3 adjustedPosition = worldPosition + worldRotation * (localCenter * finalScale);

    glm::vec3 targetPosition = adjustedPosition + offset;

    btTransform currentTransform;
    meshCollider->getMotionState()->getWorldTransform(currentTransform);
    btVector3 currentOrigin = currentTransform.getOrigin();
    btQuaternion currentRotation = currentTransform.getRotation();
    btVector3 currentScale = meshCollider->getCollisionShape()->getLocalScaling();

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
    if (meshCollider->getCollisionShape()) {
        btCollisionShape* shape = meshCollider->getCollisionShape();

        btVector3 scaledSize(size.x * finalScale.x, size.y * finalScale.y, size.z * finalScale.z);

        shape->setLocalScaling(scaledSize);
    }

    //Set Transform
    if (meshCollider->getMotionState()) {
        meshCollider->getMotionState()->getWorldTransform(currentTransform);
    }
    else {
        currentTransform = meshCollider->getWorldTransform();
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

    if (meshCollider->getMotionState()) {
        meshCollider->getMotionState()->setWorldTransform(currentTransform);
    }
    else {
        meshCollider->setWorldTransform(currentTransform);
    }

    meshCollider->setCenterOfMassTransform(currentTransform);
}



//Works for environment 
void MeshColliderComponent::CreateMeshCollider() {
    if (!owner) return;

    auto meshRenderer = owner->GetComponent<MeshRenderer>();
    if (!meshRenderer) return;

    auto model = meshRenderer->GetMesh()->getModel();
    if (!model) return;

    auto vertices = model->GetModelData().vertexData;
    auto indices = model->GetModelData().indexData;

    if (vertices.empty() || indices.empty()) return;

    btTriangleMesh* triangleMesh = new btTriangleMesh();

    Transform_Component* transform = owner->GetTransform();
    glm::vec3 position = transform->GetPosition();
    glm::vec3 minVertex(FLT_MAX), maxVertex(-FLT_MAX);
    for (const auto& vertex : vertices) {
        glm::vec3 scaledPos = glm::vec3(vertex.position.x , vertex.position.y , vertex.position.z );
        minVertex = glm::min(minVertex, scaledPos);
        maxVertex = glm::max(maxVertex, scaledPos);
    }
    glm::vec3 meshOffset = (minVertex + maxVertex) * 0.5f;

    for (size_t i = 0; i < indices.size(); i += 3) {
        btVector3 v0((vertices[indices[i]].position.x ) - meshOffset.x,
            (vertices[indices[i]].position.y ) - meshOffset.y,
            (vertices[indices[i]].position.z ) - meshOffset.z);
        btVector3 v1((vertices[indices[i + 1]].position.x ) - meshOffset.x,
            (vertices[indices[i + 1]].position.y ) - meshOffset.y,
            (vertices[indices[i + 1]].position.z ) - meshOffset.z);
        btVector3 v2((vertices[indices[i + 2]].position.x ) - meshOffset.x,
            (vertices[indices[i + 2]].position.y ) - meshOffset.y,
            (vertices[indices[i + 2]].position.z ) - meshOffset.z);

        triangleMesh->addTriangle(v0, v1, v2);
    }

    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);

    glm::dquat rotation = transform->GetRotation();
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(position.x + meshOffset.x, position.y + meshOffset.y, position.z + meshOffset.z));
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    glm::vec3 scale = transform->GetScale();
    glm::vec3 parentScale(1.0f);
    if (owner->GetParent()) {
        parentScale = owner->GetParent()->GetTransform()->GetScale();
    }
    glm::vec3 finalScale = scale * parentScale;
    shape->setLocalScaling(btVector3(finalScale.x, finalScale.y, finalScale.z));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape);
    meshCollider = new btRigidBody(rbInfo);

    btVector3 btSize = shape->getLocalScaling();
    if (size != glm::vec3(1.0f, 1.0f, 1.0f)) {
        btSize = btVector3(size.x, size.y, size.z);
        shape->setLocalScaling(btSize);
    }

    physics->dynamicsWorld->addRigidBody(meshCollider);
    physics->gameObjectRigidBodyMap[owner] = meshCollider;
}



//Testing how to decrease the triangle count to optimize performance
//void MeshColliderComponent::CreateMeshCollider() {
//    if (!owner) return;
//
//    auto meshRenderer = owner->GetComponent<MeshRenderer>();
//    if (!meshRenderer) return;
//
//    auto model = meshRenderer->GetMesh()->getModel();
//    if (!model) return;
//
//    auto vertices = model->GetModelData().vertexData;
//    auto indices = model->GetModelData().indexData;
//
//    if (vertices.empty() || indices.empty()) return;
//
//    btTriangleMesh* triangleMesh = new btTriangleMesh();
//
//    Transform_Component* transform = owner->GetTransform();
//    glm::vec3 position = transform->GetPosition();
//    glm::vec3 scale = transform->GetScale();
//    glm::vec3 parentScale = owner->GetParent() ? glm::vec3(owner->GetParent()->GetTransform()->GetScale()) : glm::vec3(1.0f);
//    glm::vec3 finalScale = scale * parentScale;
//
//     Reducir la cantidad de tri�ngulos para optimizar el rendimiento
//    size_t step = std::max<size_t>(1, indices.size() / 100000); // Reduce la cantidad de tri�ngulos din�micamente
//    for (size_t i = 0; i < indices.size(); i += 3 * step) {
//        btVector3 v0(vertices[indices[i]].position.x * finalScale.x - position.x,
//                     vertices[indices[i]].position.y * finalScale.y - position.y,
//                     vertices[indices[i]].position.z * finalScale.z - position.z);
//        btVector3 v1(vertices[indices[i + 1]].position.x * finalScale.x - position.x,
//                     vertices[indices[i + 1]].position.y * finalScale.y - position.y,
//                     vertices[indices[i + 1]].position.z * finalScale.z - position.z);
//        btVector3 v2(vertices[indices[i + 2]].position.x * finalScale.x - position.x,
//                     vertices[indices[i + 2]].position.y * finalScale.y - position.y,
//                     vertices[indices[i + 2]].position.z * finalScale.z - position.z);
//
//        triangleMesh->addTriangle(v0, v1, v2);
//    }
//
//    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);
//    shape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));
//
//    btTransform startTransform;
//    startTransform.setIdentity();
//    startTransform.setOrigin(btVector3(position.x, position.y, position.z));
//    startTransform.setRotation(btQuaternion(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z, transform->GetRotation().w));
//
//    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
//    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape);
//    rigidBody = new btRigidBody(rbInfo);
//
//    physics->dynamicsWorld->addRigidBody(rigidBody);
//    physics->gameObjectRigidBodyMap[owner] = rigidBody;
//}
