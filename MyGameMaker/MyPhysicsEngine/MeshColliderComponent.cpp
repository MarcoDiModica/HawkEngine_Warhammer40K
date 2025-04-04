#include "MeshColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "RigidBodyComponent.h"

MeshColliderComponent::MeshColliderComponent(GameObject* owner, PhysicsModule* physicsModule)
	: BaseColliderComponent(owner, physicsModule)
{
	name = "MeshColliderComponent";
	physics = physicsModule;
    updateInStop = true;
	Start();
}

MeshColliderComponent::~MeshColliderComponent() {
    Destroy();
}

void MeshColliderComponent::Start() {
    if (!collider) {
        CreateCollider();
    }
}

void MeshColliderComponent::Update(float deltaTime) {
    if (owner) {
        SnapToPosition();
    }
}

std::unique_ptr<Component> MeshColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<MeshColliderComponent>(new_owner, physics);
}

MonoObject* MeshColliderComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Collider");
    if (!klass) {
        MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "MeshCollider");
        if (!klass) {
            return nullptr;
        }
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

void MeshColliderComponent::CreateCollider() {
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
    collider = new btRigidBody(rbInfo);

    btVector3 btSize = shape->getLocalScaling();
    if (size != glm::vec3(1.0f, 1.0f, 1.0f)) {
        btSize = btVector3(size.x, size.y, size.z);
        shape->setLocalScaling(btSize);
    }

    physics->dynamicsWorld->addRigidBody(collider);
    physics->gameObjectRigidBodyMap[owner] = collider;
}
