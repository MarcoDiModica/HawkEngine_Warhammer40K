
#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include <iostream>
#include <glm/glm.hpp>


constexpr float fixedDeltaTime = 1.0f / 60.0f; // 60 updates per second
float accumulatedTime = 0.0f;


PhysicsModule::PhysicsModule() : dynamicsWorld(nullptr), cubeShape(nullptr) {}

PhysicsModule::~PhysicsModule() {
    CleanUp();
}

bool PhysicsModule::Awake() {
    // Inicializaci�n del sistema de f�sicas de Bullet
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    //Debug drawer
    debugDrawer = new DebugDrawerPhysics();
    dynamicsWorld->setDebugDrawer(debugDrawer);

    // Initialize vehicle raycaster
    vehicle_raycaster = new btDefaultVehicleRaycaster(dynamicsWorld);

    // Initialize shared collision shapes (e.g., cubeShape)
    cubeShape = new btBoxShape(btVector3(1, 1, 1));


    //Plane functions
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
    //groundRigidBody->setRestitution(0.8f);
    SetGlobalRestitution(0.5f);
    return true;
}

void PhysicsModule::SyncTransforms() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        btTransform transform;
        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->getWorldTransform(transform);
        }

        btVector3 pos = transform.getOrigin();
        btQuaternion rot = transform.getRotation();

        auto goTransform = gameObject->GetTransform();

        // Obtener la posici�n inicial de gameObject solo si no est� almacenada
        static std::unordered_map<GameObject*, glm::dvec3> initialOffsets;
        if (initialOffsets.find(gameObject) == initialOffsets.end()) {
            glm::dvec3 initialPos = goTransform->GetPosition();
            glm::dvec3 rigidBodyInitialPos = { pos[0], pos[1], pos[2] };
            initialOffsets[gameObject] = initialPos - rigidBodyInitialPos;
        }

        glm::dvec3 adjustedPosition = glm::dvec3(pos[0], pos[1], pos[2]) + initialOffsets[gameObject];
        goTransform->SetPosition(adjustedPosition);

        // Aplicar la rotaci�n solo si ha cambiado
        static std::unordered_map<GameObject*, glm::dquat> previousRotations;
        glm::dquat newRotation = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
        
        if (previousRotations.find(gameObject) != previousRotations.end()) {
            if (previousRotations[gameObject] != newRotation) {
                glm::dquat deltaRotation = glm::inverse(previousRotations[gameObject]) * newRotation;
                goTransform->SetRotationQuat(goTransform->GetRotation() * deltaRotation);
            }
        }
        previousRotations[gameObject] = newRotation;
    }
}

void PhysicsModule::SyncCollidersToGameObjects() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        auto goTransform = gameObject->GetTransform();
        glm::vec3 position = goTransform->GetLocalPosition();
        glm::quat rotation = goTransform->GetLocalRotation();

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(position.x, position.y, position.z));
        transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->setWorldTransform(transform);
        }
        else {
            rigidBody->setWorldTransform(transform);
        }

        std::cout << "Collider position updated to: ("
            << position.x << ", " << position.y << ", " << position.z << ")\n";
    }
}

std::vector<btRigidBody*> GetAllRigidBodies(btDiscreteDynamicsWorld* dynamicsWorld) {
    std::vector<btRigidBody*> rigidBodies;
    int numCollisionObjects = dynamicsWorld->getNumCollisionObjects();

    for (int i = 0; i < numCollisionObjects; ++i) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body) {
            rigidBodies.push_back(body);
        }
    }

    return rigidBodies;
}

void PhysicsModule::DrawDebugDrawer() {
    if (debugDrawer) {
        auto rigidBodies = GetAllRigidBodies(dynamicsWorld);

        for (const auto& rigidBody : rigidBodies) {
            if (!rigidBody || !rigidBody->getCollisionShape()) {
                continue;
            }

            btCollisionShape* shape = rigidBody->getCollisionShape();

            btTransform transform;
            rigidBody->getMotionState()->getWorldTransform(transform);
            btQuaternion rotation = transform.getRotation();
            btMatrix3x3 rotationMatrix(rotation);
            btVector3 position = transform.getOrigin();

            if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
                btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
                btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();

                // Definir las 8 esquinas del BoundingBox en espacio local
                btVector3 localCorners[8] = {
                    btVector3(-halfExtents.x(), -halfExtents.y(), -halfExtents.z()),
                    btVector3(halfExtents.x(), -halfExtents.y(), -halfExtents.z()),
                    btVector3(halfExtents.x(),  halfExtents.y(), -halfExtents.z()),
                    btVector3(-halfExtents.x(),  halfExtents.y(), -halfExtents.z()),
                    btVector3(-halfExtents.x(), -halfExtents.y(),  halfExtents.z()),
                    btVector3(halfExtents.x(), -halfExtents.y(),  halfExtents.z()),
                    btVector3(halfExtents.x(),  halfExtents.y(),  halfExtents.z()),
                    btVector3(-halfExtents.x(),  halfExtents.y(),  halfExtents.z())
                };

                glm::vec3 worldCorners[8];
                for (int i = 0; i < 8; i++) {
                    btVector3 rotatedCorner = rotationMatrix * localCorners[i] + position;
                    worldCorners[i] = glm::vec3(rotatedCorner.x(), rotatedCorner.y(), rotatedCorner.z());
                }

                debugDrawer->drawRotatedBoundingBox(worldCorners, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
                btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
                float radius = sphereShape->getRadius();
                glm::vec3 center(position.x(), position.y(), position.z());
                debugDrawer->drawSphere(center, radius, glm::vec3(1.0f, 0.0f, 0.0f), 16);
            }
        }
    }
}

bool PhysicsModule::Update(double dt) {
	if (linkPhysicsToScene)
    {
        accumulatedTime += dt;
        while (accumulatedTime >= fixedDeltaTime) {
            dynamicsWorld->stepSimulation(static_cast<btScalar>(fixedDeltaTime), 10);
            SyncTransforms();
            accumulatedTime -= fixedDeltaTime;
        }
    }
    DrawDebugDrawer();
    return true;
}

void PhysicsModule::AddForceToCollider(GameObject& go, const glm::vec3& force) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        btVector3 btForce(force.x, force.y, force.z);
        rigidBody->applyCentralForce(btForce);
        std::cout << "Force applied to GameObject: ("
            << force.x << ", " << force.y << ", " << force.z << ")\n";
    } else {
        std::cerr << "GameObject not found in physics world.\n";
    }
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

void PhysicsModule::SpawnPhysSphereWithForce(GameObject& go, float radius, float mass, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, float forceMagnitude) {
    Transform_Component* transform = go.GetTransform();
    transform->SetPosition(cameraPosition);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
    startTransform.setRotation(btQuaternion(0, 0, 0, 1)); // Sin rotaci�n inicial

    btSphereShape* sphereShape = new btSphereShape(radius);

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    btVector3 inertia(0, 0, 0);
    if (mass > 0.0f) {
        sphereShape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphereShape, inertia);
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    rigidBody->setRestitution(0.7f);
    dynamicsWorld->addRigidBody(rigidBody);
    gameObjectRigidBodyMap[&go] = rigidBody;

    // Obtener la direcci�n de la c�mara y aplicar una fuerza inicial
    //glm::vec3 cameraDirection = Application->camera->GetTransform().GetForward(); // Direcci�n de la c�mara
    btVector3 btForce = btVector3(cameraDirection.x, cameraDirection.y, cameraDirection.z) * forceMagnitude;
    rigidBody->applyCentralImpulse(btForce);
}

void PhysicsModule::SetGlobalRestitution(float restitutionValue) {
    int numCollisionObjects = dynamicsWorld->getNumCollisionObjects();

    for (int i = 0; i < numCollisionObjects; ++i) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* rigidBody = btRigidBody::upcast(obj);
        if (rigidBody) {
            rigidBody->setRestitution(restitutionValue);
        }
    }

    std::cout << "Global restitution set to: " << restitutionValue << "\n";
}
void PhysicsModule::SetColliderFriction(GameObject& go, float friction) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setFriction(friction);
        rigidBody->setRollingFriction(friction);
        rigidBody->setSpinningFriction(friction);
        std::cout << "Friction set to " << friction << " for GameObject.\n";
    }
}

void PhysicsModule::SetBounciness(GameObject& go, float restitution) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setRestitution(restitution);
        std::cout << "Bounciness set to " << restitution << " for GameObject.\n";
    }
}

void PhysicsModule::EnableContinuousCollision(GameObject& go) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setCcdMotionThreshold(0.01);
        rigidBody->setCcdSweptSphereRadius(0.05);
        std::cout << "Continuous collision detection enabled for GameObject.\n";
    }
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


