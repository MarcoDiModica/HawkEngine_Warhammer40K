#include "MeshColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"

MeshColliderComponent::MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule)
    : Component(owner), physics(physicsModule), rigidBody(nullptr) {
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
