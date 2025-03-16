#include "MeshColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"

MeshColliderComponent::MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner), physics(physicsModule), rigidBody(nullptr) 
{
	name = "MeshColliderComponent";
}

MeshColliderComponent::~MeshColliderComponent() {
    Destroy();
}

void MeshColliderComponent::Start() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
    CreateMeshCollider();
}

void MeshColliderComponent::Update(float deltaTime) {
    // Update logic if needed
}

void MeshColliderComponent::Destroy() {
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

std::unique_ptr<Component> MeshColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<MeshColliderComponent>(new_owner, physics);
}


void MeshColliderComponent::SetTrigger(bool trigger) {
    if (rigidBody) {
        if (trigger) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }
}

bool MeshColliderComponent::IsTrigger() const {
    return (rigidBody && (rigidBody->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
}

glm::vec3 MeshColliderComponent::GetColliderPos() {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat MeshColliderComponent::GetColliderRotation() {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void MeshColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    if (!rigidBody || !rigidBody->getMotionState()) {
        return;
    }

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setWorldTransform(trans);
    rigidBody->activate();
}

void MeshColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setCenterOfMassTransform(trans);
}

glm::vec3 MeshColliderComponent::GetSize() {
    if (rigidBody) {
        btCollisionShape* shape = rigidBody->getCollisionShape();
        if (shape) {
            btVector3 scale = shape->getLocalScaling();
            return glm::vec3(scale.getX() * 2.0f, scale.getY() * 2.0f, scale.getZ() * 2.0f);
        }
    }
    return size;
}

void MeshColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (rigidBody) {
        btCollisionShape* shape = rigidBody->getCollisionShape();
        if (shape) {
            shape->setLocalScaling(btVector3(newSize.x * 0.5f, newSize.y * 0.5f, newSize.z * 0.5f));
        }
    }
}

void MeshColliderComponent::SetMass(float newMass) {
    mass = newMass;
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
    CreateMeshCollider();
}

void MeshColliderComponent::SetActive(bool active) {
    if (rigidBody) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(rigidBody);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(rigidBody);
        }
    }
}

void MeshColliderComponent::SnapToPosition() {
    if (!owner) return;

    Transform_Component* goTransform = owner->GetTransform();
    if (!goTransform) return;

    BoundingBox localBBox = owner->localBoundingBox();
    glm::vec3 localCenter = localBBox.center();
    glm::vec3 worldScale = goTransform->GetScale();
    glm::vec3 worldPosition = goTransform->GetPosition();
    glm::quat worldRotation = goTransform->GetRotation();
    glm::vec3 adjustedPosition = worldPosition + worldRotation * (localCenter * worldScale);

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

    if (rigidBody->getCollisionShape()) {
        rigidBody->getCollisionShape()->setLocalScaling(btVector3(worldScale.x, worldScale.y, worldScale.z));
    }
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
    glm::vec3 scale = transform->GetScale();
    glm::vec3 parentScale(1.0f);
    if (owner->GetParent()) {
        parentScale = owner->GetParent()->GetTransform()->GetScale();
    }
    glm::vec3 finalScale = scale * parentScale;

    glm::vec3 minVertex(FLT_MAX), maxVertex(-FLT_MAX);
    for (const auto& vertex : vertices) {
        glm::vec3 scaledPos = glm::vec3(vertex.position.x * finalScale.x, vertex.position.y * finalScale.y, vertex.position.z * finalScale.z);
        minVertex = glm::min(minVertex, scaledPos);
        maxVertex = glm::max(maxVertex, scaledPos);
    }
    glm::vec3 meshOffset = (minVertex + maxVertex) * 0.5f;

    for (size_t i = 0; i < indices.size(); i += 3) {
        btVector3 v0((vertices[indices[i]].position.x * finalScale.x) - meshOffset.x,
            (vertices[indices[i]].position.y * finalScale.y) - meshOffset.y,
            (vertices[indices[i]].position.z * finalScale.z) - meshOffset.z);
        btVector3 v1((vertices[indices[i + 1]].position.x * finalScale.x) - meshOffset.x,
            (vertices[indices[i + 1]].position.y * finalScale.y) - meshOffset.y,
            (vertices[indices[i + 1]].position.z * finalScale.z) - meshOffset.z);
        btVector3 v2((vertices[indices[i + 2]].position.x * finalScale.x) - meshOffset.x,
            (vertices[indices[i + 2]].position.y * finalScale.y) - meshOffset.y,
            (vertices[indices[i + 2]].position.z * finalScale.z) - meshOffset.z);

        triangleMesh->addTriangle(v0, v1, v2);
    }

    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);
    shape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));

    glm::dquat rotation = transform->GetRotation();
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(position.x + meshOffset.x, position.y + meshOffset.y, position.z + meshOffset.z));
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape);
    rigidBody = new btRigidBody(rbInfo);

    physics->dynamicsWorld->addRigidBody(rigidBody);
    physics->gameObjectRigidBodyMap[owner] = rigidBody;
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
//     Reducir la cantidad de triángulos para optimizar el rendimiento
//    size_t step = std::max<size_t>(1, indices.size() / 100000); // Reduce la cantidad de triángulos dinámicamente
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
