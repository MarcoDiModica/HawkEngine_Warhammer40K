#pragma once

#include "ColliderComponent.h"
#include "../MyGameEngine/GameObject.h"

ColliderComponent::ColliderComponent(GameObject* owner/*, PhysicsModule* physicsModule, bool isForStreet*/) : Component(owner) { name = "ColliderComponent"; physicsModule = new PhysicsModule(); /*physics = physicsModule; isForStreetLocal = isForStreet; */ }

ColliderComponent::~ColliderComponent() {
    Destroy();
}

void ColliderComponent::Start() {
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

    btCollisionShape* shape = new btBoxShape(btVector3(size.x * 0.5f, size.z, size.y * 0.5f));

    btTransform startTransform;
    startTransform.setIdentity();

    if (isForStreet) {
        glm::vec3 localPosition = glm::vec3(transform->GetPosition());
        startTransform.setOrigin(btVector3(localPosition.x, localPosition.z, localPosition.y));
        glm::quat rotation = transform->GetRotation();
        startTransform.setRotation(btQuaternion(btVector3(1, 0, 0), glm::radians(90.0f)));

        //auto x = transform->GetRotation().y;
        //auto z = transform->GetRotation().z;
        glm::vec3 scale = transform->GetScale();
        shape->setLocalScaling(btVector3(scale.x, scale.z, scale.y));
    }
    else
    {
        glm::vec3 localPosition = glm::vec3(transform->GetLocalMatrix()[3]);
        startTransform.setOrigin(btVector3(localPosition.x, localPosition.y, localPosition.z));
        //startTransform.setRotation(btQuaternion(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z, transform->GetRotation().w));
    }


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
    gameObjectRigidBodyMapForhouse[owner] = rigidBody;





    //void ColliderComponent::CreateCollider(bool isForStreet) {
    //    if (!owner) return;

    //    // Obtener el Transform del GameObject
    //    Transform_Component* transform = owner->GetTransform();
    //    if (!transform) return;

    //    // Obtener el BoundingBox del GameObject
    //    BoundingBox bbox = owner->boundingBox(); // Asume que el GameObject tiene un método GetBoundingBox()
    //    size = bbox.size(); // Tamaño del Bounding Box

    //    // Crear la forma de colisión usando el tamaño del Bounding Box
    //    btCollisionShape* shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));

    //    // Configurar la transformación inicial
    //    btTransform startTransform;
    //    startTransform.setIdentity();

    //    // Obtener la transformación global
    //    glm::mat4 globalMatrix = transform->GetMatrix(); // Matriz de transformación global
    //    glm::vec3 globalPosition(globalMatrix[3]);       // Posición global
    //    glm::quat globalRotation = transform->GetRotation(); // Rotación global

    //    // Ajustar las transformaciones si es para la calle
    //    if (isForStreet) {
    //        // Aplicar las transformaciones específicas de la calle
    //        glm::vec3 adjustedPosition = globalPosition * glm::vec3(0.5f, 0.5f, 0.5f) + glm::vec3(0, 0.1f, 0);
    //        globalPosition = adjustedPosition;

    //        // Rotar -90 grados en el eje X
    //        glm::quat rotationAdjustment = glm::rotate(glm::quat(1, 0, 0, 0), -1.5708f, glm::vec3(1, 0, 0));
    //        globalRotation = globalRotation * rotationAdjustment;
    //    }

    //    // Configurar la posición y rotación en Bullet
    //    startTransform.setOrigin(btVector3(globalPosition.x, globalPosition.y, globalPosition.z));
    //    startTransform.setRotation(btQuaternion(globalRotation.x, globalRotation.y, globalRotation.z, globalRotation.w));

    //    // Configurar la masa e inercia
    //    btVector3 localInertia(0, 0, 0);
    //    if (mass > 0.0f) {
    //        shape->calculateLocalInertia(mass, localInertia);
    //    }

    //    // Crear el cuerpo rígido
    //    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    //    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    //    rigidBody = new btRigidBody(rbInfo);

    //    // Añadir el colisionador al mundo de físicas
    //    physics->dynamicsWorld->addRigidBody(rigidBody);
    //}






}