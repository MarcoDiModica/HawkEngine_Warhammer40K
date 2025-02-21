
#pragma once

#include "ColliderComponent.h"
#include "../MyGameEngine/GameObject.h"

ColliderComponent::ColliderComponent(GameObject* owner, PhysicsModule* physicsModule, bool isForStreet) : Component(owner) { name = "ColliderComponent"; physics = physicsModule; isForStreetLocal = isForStreet; }

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Start() {
    mass = 0.0f;
    CreateCollider(isForStreetLocal);
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMapForhouse) {

        btTransform transform;
        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->getWorldTransform(transform);
        }

        btVector3 pos = transform.getOrigin();
        btQuaternion rot = transform.getRotation();

        auto goTransform = gameObject->GetTransform();

        glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
        glm::dvec3 deltaPos = newPosition - goTransform->GetPosition();
        goTransform->Translate(deltaPos);

        glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
        glm::dvec3 currentRotation = glm::radians(goTransform->GetEulerAngles());
        glm::dvec3 deltaRot = newRotation - currentRotation;

        goTransform->Rotate(glm::radians(-90.0), glm::dvec3(1, 0, 0));
        goTransform->Rotate(glm::radians(0.0), glm::dvec3(0, 1, 0));
        goTransform->Rotate(glm::radians(0.0), glm::dvec3(0, 0, 1));

        std::cout << "GameObject position updated to: ("
            << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << ")\n";
    }
}

glm::vec3 ColliderComponent::GetColliderPos() {

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return glm::vec3(pos.getX(), pos.getY(), pos.getZ());
}

glm::quat ColliderComponent::GetColliderRotation() {

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();
    return glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
}

void ColliderComponent::SetColliderRotation(const glm::quat& rotation) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btVector3 currentPosition = trans.getOrigin();
    trans.setOrigin(currentPosition);

    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setWorldTransform(trans);;
}

void ColliderComponent::SetColliderPos(const glm::vec3& position) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(position.x, position.y, position.z));
    rigidBody->getMotionState()->setWorldTransform(trans);
    rigidBody->setCenterOfMassTransform(trans);
}

void ColliderComponent::SetSize(const glm::vec3& newSize) {
    size = newSize;
    if (rigidBody) {
        btCollisionShape* shape = rigidBody->getCollisionShape();
        if (shape) {
            btVector3 newBtSize(newSize.x * 0.5f, newSize.y * 0.5f, newSize.z * 0.5f);
            shape->setLocalScaling(newBtSize);
        }
    }
}
void ColliderComponent::SetMass(float newMass) {
    mass = newMass;
    if (rigidBody) {
        physics->dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
        rigidBody = nullptr;
    }
    CreateCollider(false);
}

void ColliderComponent::SetActive(bool active) {
    if (rigidBody) {
        if (active) {
            physics->dynamicsWorld->addRigidBody(rigidBody);
        }
        else {
            physics->dynamicsWorld->removeRigidBody(rigidBody);
        }
    }
}

void ColliderComponent::Update(float deltaTime) {

    // Sincroniza la posición y rotación entre el GameObject y el colisionador

    //for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMapForhouse) {

    //    btTransform transform;
    //    if (rigidBody->getMotionState()) {
    //        rigidBody->getMotionState()->getWorldTransform(transform);
    //    }

    //    btVector3 pos = transform.getOrigin();
    //    btQuaternion rot = transform.getRotation();

    //    auto goTransform = gameObject->GetTransform();

    //    //TESTING
    //    //glm::quat newRotation = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
    //    //goTransform->SetRotationQuat(newRotation);

    //    //// Calcular delta de posición y aplicar traslación
    //    //glm::dvec3 newPosition = { pos.getX(), pos.getY(), pos.getZ() };
    //    //glm::dvec3 currentPosition = goTransform->GetPosition();
    //    //glm::dvec3 deltaPos = newPosition - currentPosition;

    //    //// Aplica la traslación
    //    //goTransform->Translate(deltaPos);


    //    glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
    //    glm::dvec3 deltaPos = newPosition - goTransform->GetPosition();
    //    goTransform->Translate(deltaPos);
    //    auto x = pos.x();
    //    auto y = pos.y();
    //    auto z = pos.z();
    //    auto v = glm::vec3(x, y, z);

    //    glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
    //    glm::dvec3 currentRotation = glm::radians(goTransform->GetEulerAngles());
    //    glm::dvec3 deltaRot = newRotation - currentRotation;

    //    goTransform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
    //    goTransform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
    //    goTransform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));

    //    std::cout << "GameObject position updated to: ("
    //        << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << ")\n";
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
void ColliderComponent::CreateCollider(bool isForStreet) {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    BoundingBox bbox = owner->boundingBox();
    size = bbox.size();


    btCollisionShape* shape;

    btTransform startTransform;
    startTransform.setIdentity();


    shape = new btBoxShape(btVector3(size.x * 0.5, size.y * 0.5, size.z * 0.5));
    glm::vec3 localPosition = transform->GetLocalPosition();
    startTransform.setOrigin(btVector3(owner->boundingBox().center().x, owner->boundingBox().center().y, owner->boundingBox().center().z));
    startTransform.setRotation(btQuaternion(transform->GetLocalRotation().x, transform->GetLocalRotation().y, transform->GetLocalRotation().z, transform->GetLocalRotation().w));
    glm::vec3 scale = transform->GetScale();
    shape->setLocalScaling(btVector3(scale.x, scale.z, scale.y));



    // Configurar la masa e inercia
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    // Crear el cuerpo rígido
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    rigidBody = new btRigidBody(rbInfo);

    //rigidBody->setRestitution(0.5f);

    // Añadir el colisionador al mundo de físicas
    physics->dynamicsWorld->addRigidBody(rigidBody);
    //gameObjectRigidBodyMapForhouse[owner] = rigidBody;
    //doesnt works properly
    physics->gameObjectRigidBodyMap[owner] = rigidBody;

}
