#include "CapsuleColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "RigidBodyComponent.h"

CapsuleColliderComponent::CapsuleColliderComponent(GameObject* owner, PhysicsModule* physicsModule) : BaseColliderComponent(owner, physicsModule)
{
    name = "CapsuleColliderComponent";
    physics = physicsModule;
    updateInStop = true;
    Start();
}

CapsuleColliderComponent::~CapsuleColliderComponent() {
    Destroy();
}

void CapsuleColliderComponent::Start() {
    if (!collider) {
        CreateCollider();
    }
}

void CapsuleColliderComponent::Update(float deltaTime) {
    if (owner) {
        SnapToPosition();
    }
}

std::unique_ptr<Component> CapsuleColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<CapsuleColliderComponent>(new_owner, physics);
}

//Local BBox Adjusted (doesnt works with the blocking)


MonoObject* CapsuleColliderComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Collider");
    if (!klass) {
        MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "CapsuleCollider");
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

void CapsuleColliderComponent::CreateCollider() {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    BoundingBox bbox = owner->localBoundingBox();
    auto localSize = bbox.size();

    btCollisionShape* shape;
    btTransform startTransform;
    startTransform.setIdentity();

    if (localSize.x == 0.0f && localSize.y == 0.0f && localSize.z == 0.0f) {
        localSize = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    glm::vec3 bboxCenter = transform->GetLocalPosition();

	shape = new btCapsuleShape(1.0f, 1.0f);
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

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, localInertia);
    collider = new btRigidBody(rbInfo);
    btVector3 btSize = shape->getLocalScaling();

    // Add the collider to the physics world
    physics->dynamicsWorld->addRigidBody(collider);
    physics->gameObjectRigidBodyMap[owner] = collider;
}

