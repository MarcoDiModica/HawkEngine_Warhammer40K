#pragma once

#include "ColliderComponent.h"
#include "GameObject.h"

ColliderComponent::ColliderComponent(GameObject* owner, PhysicsModule* physicsModule) : Component(owner) { name = "ColliderComponent"; physics = physicsModule; }

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Start() {
    CreateCollider();
}

void ColliderComponent::Update(float deltaTime) {
    // Sincroniza la posición y rotación entre el GameObject y el colisionador
    //if (rigidBody && rigidBody->getMotionState()) {
    //    Transform_Component* transform = owner->GetTransform();
    //    btTransform btTransform;
    //    rigidBody->getMotionState()->getWorldTransform(btTransform);

    //    btVector3 pos = btTransform.getOrigin();
    //    btQuaternion rot = btTransform.getRotation();

    //    // Actualiza las transformaciones del GameObject
    //    transform->SetPosition(glm::vec3(pos.getX(), pos.getY(), pos.getZ()));
    //    transform->SetRotationQuat(glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));
    //}
}

void ColliderComponent::Destroy() {
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
}

std::unique_ptr<Component> ColliderComponent::Clone(GameObject* new_owner) {
    return std::make_unique<ColliderComponent>(new_owner, physics);
}
void ColliderComponent::CreateCollider() {
    if (!owner) return;

    // Obtener el Transform del GameObject
    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    // Obtener el BoundingBox del GameObject
    BoundingBox bbox = owner->boundingBox(); // Asume que el GameObject tiene un método GetBoundingBox()
    size = bbox.size(); // Tamaño del Bounding Box

    // Crear la forma de colisión usando el tamaño del Bounding Box
    btCollisionShape* shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));

    // Configurar la transformación inicial
    btTransform startTransform;
    startTransform.setIdentity();


    startTransform.setOrigin(btVector3(transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z));
    startTransform.setRotation(btQuaternion(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z, transform->GetRotation().w));

    // Configurar la masa e inercia
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    // Crear el cuerpo rígido
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    rigidBody = new btRigidBody(rbInfo);
    // Añadir el colisionador al mundo de físicas
    physics->dynamicsWorld->addRigidBody(rigidBody);
}