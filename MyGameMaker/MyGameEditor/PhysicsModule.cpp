#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include <iostream>

PhysicsModule::PhysicsModule(App* app)
    : Module(app), dynamicsWorld(nullptr), cubeShape(nullptr) {}

PhysicsModule::~PhysicsModule() {
    CleanUp();
}

bool PhysicsModule::Awake() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // Configurar gravedad estándar
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Forma del cubo: un cubo de 2x2x2
    cubeShape = new btBoxShape(btVector3(1, 1, 1));
    return true;
}

void PhysicsModule::CreatePhysicsForGameObject(GameObject& go, float mass) {
    // Obtiene la posición inicial del GameObject
    auto transform = go.GetTransform();
    glm::vec3 position = transform->GetPosition();
    glm::quat rotation = transform->GetRotation();

    // Configura el transform inicial de Bullet
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(position.x, position.y, position.z));
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    // Configura el estado de movimiento
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    // Calcula la inercia si el objeto tiene masa
    btVector3 inertia(0, 0, 0);
    if (mass > 0.0f) {
        cubeShape->calculateLocalInertia(mass, inertia);
    }

    // Configura el cuerpo rígido
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, cubeShape, inertia);
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    // Añade el cuerpo rígido al mundo físico
    dynamicsWorld->addRigidBody(rigidBody);

    // Almacena la relación entre el GameObject y el cuerpo rígido
    gameObjectRigidBodyMap[&go] = rigidBody;

    // Depuración: Imprime la posición inicial
    std::cout << "Physics created for GameObject at position: ("
        << position.x << ", " << position.y << ", " << position.z << ")\n";
}

void PhysicsModule::SyncTransforms() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        btTransform transform;
        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->getWorldTransform(transform);
        }

        btVector3 pos = transform.getOrigin();
        btQuaternion rot = transform.getRotation();

        // Actualiza la posición y rotación del GameObject
        auto goTransform = gameObject->GetTransform();
        goTransform->SetPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
        goTransform->SetRotationQuat(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));

        // Depuración: Imprime la posición actual
        std::cout << "GameObject position updated to: ("
            << pos.x() << ", " << pos.y() << ", " << pos.z() << ")\n";
    }
}

bool PhysicsModule::Update(double dt) {
    dynamicsWorld->stepSimulation(static_cast<btScalar>(dt), 10);
    SyncTransforms();
    return true;
}

bool PhysicsModule::CleanUp() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
    }
    gameObjectRigidBodyMap.clear();

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
    delete cubeShape;

    return true;
}

bool PhysicsModule::Start() {
    return true;
}

bool PhysicsModule::PreUpdate() {
    return true;
}

bool PhysicsModule::PostUpdate() {
    return true;
}
