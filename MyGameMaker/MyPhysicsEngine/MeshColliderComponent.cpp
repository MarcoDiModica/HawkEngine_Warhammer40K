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
}

std::unique_ptr<Component> MeshColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<MeshColliderComponent>(new_owner, physics);
}void MeshColliderComponent::CreateMeshCollider() {
    if (!owner) return;

    auto meshRenderer = owner->GetComponent<MeshRenderer>();
    if (!meshRenderer) return;

    auto model = meshRenderer->GetMesh()->getModel();
    if (!model) return;

    auto vertices = model->GetModelData().vertexData;
    auto indices = model->GetModelData().indexData;

    btTriangleMesh* triangleMesh = new btTriangleMesh();

    Transform_Component* transform = owner->GetTransform();
    glm::vec3 scale = transform->GetScale();
    glm::vec3 parentScale(1.0f);
    if (owner->GetParent()) {
        parentScale = owner->GetParent()->GetTransform()->GetScale();
    }
    glm::vec3 finalScale = scale * parentScale;

    for (size_t i = 0; i < indices.size(); i += 3) {
        btVector3 v0(vertices[indices[i]].position.x * finalScale.x,
            vertices[indices[i]].position.y * finalScale.y,
            vertices[indices[i]].position.z * finalScale.z);
        btVector3 v1(vertices[indices[i + 1]].position.x * finalScale.x,
            vertices[indices[i + 1]].position.y * finalScale.y,
            vertices[indices[i + 1]].position.z * finalScale.z);
        btVector3 v2(vertices[indices[i + 2]].position.x * finalScale.x,
            vertices[indices[i + 2]].position.y * finalScale.y,
            vertices[indices[i + 2]].position.z * finalScale.z);

        triangleMesh->addTriangle(v0, v1, v2);
    }

    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);
    shape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f)); // No aplicar la escala aquí, ya se aplicó en los vértices

    glm::dquat rotation = transform->GetRotation();
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(owner->boundingBox().center().x, owner->boundingBox().center().y, owner->boundingBox().center().z));
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape);
    rigidBody = new btRigidBody(rbInfo);

    physics->dynamicsWorld->addRigidBody(rigidBody);
    physics->gameObjectRigidBodyMap[owner] = rigidBody;
}