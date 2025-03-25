#include "BoxColliderComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "RigidBodyComponent.h"

BoxColliderComponent::BoxColliderComponent(GameObject* owner, PhysicsModule* physicsModule) : BaseColliderComponent(owner, physicsModule)
{
    name = "BoxColliderComponent";
    physics = physicsModule;
    updateInStop = true;
    Start();
}

BoxColliderComponent::~BoxColliderComponent() {
    Destroy();
}

std::unique_ptr<Component> BoxColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<BoxColliderComponent>(new_owner, physics);
}


MonoObject* BoxColliderComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Collider");
    if (!klass) {
        MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "BoxCollider");
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

void BoxColliderComponent::CreateCollider() {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    BoundingBox bbox = owner->localBoundingBox();
    auto localSize = bbox.size();

    glm::vec3 bboxCenter = owner->boundingBox().center();

    btCollisionShape* shape;
    btTransform startTransform;
    startTransform.setIdentity();

    localSize = glm::vec3(1.0f, 1.0f, 1.0f);
    bboxCenter = transform->GetLocalPosition();

    shape = new btBoxShape(btVector3(localSize.x * 0.5, localSize.y * 0.5, localSize.z * 0.5));
    glm::vec3 localPosition = transform->GetLocalPosition();
    startTransform.setOrigin(btVector3(bboxCenter.x + offset.x, bboxCenter.y + offset.y, bboxCenter.z + offset.z));
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
	if (size != glm::vec3(1.0f, 1.0f, 1.0f)) {
	/*	btSize = btVector3(size.x, size.y, size.z);
		shape->setLocalScaling(btSize);*/
	}

    // Add the collider to the physics world
    physics->dynamicsWorld->addRigidBody(collider);
    physics->gameObjectRigidBodyMap[owner] = collider;
}
