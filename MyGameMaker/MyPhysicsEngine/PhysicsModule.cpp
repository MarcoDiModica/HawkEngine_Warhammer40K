
#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include <iostream>
#include <glm/glm.hpp>


PhysicsModule::PhysicsModule() : dynamicsWorld(nullptr), cubeShape(nullptr) {}

PhysicsModule::~PhysicsModule() {
    CleanUp();
}

bool PhysicsModule::Awake() {
    // Inicialización del sistema de físicas de Bullet
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

void PhysicsModule::CreatePhysicsForCube(GameObject& go, float mass) {

    Transform_Component* transform = go.GetTransform();
    glm::vec3 position = transform->GetPosition();
    glm::quat rotation = transform->GetRotation();

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(position.x, position.y, position.z));
    startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    btVector3 inertia(0, 0, 0);
    if (mass > 0.0f) {
        cubeShape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, cubeShape, inertia);
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
    //PhysBody3D* body = new PhysBody3D(rigidBody);
    dynamicsWorld->addRigidBody(rigidBody);
    //body.Push(1, 1, 1);

    gameObjectRigidBodyMap[&go] = rigidBody;

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

        auto goTransform = gameObject->GetTransform();

        // Obtener la posición inicial de gameObject solo si no está almacenada
        static std::unordered_map<GameObject*, glm::dvec3> initialOffsets;
        if (initialOffsets.find(gameObject) == initialOffsets.end()) {
            glm::dvec3 initialPos = goTransform->GetPosition();
            glm::dvec3 rigidBodyInitialPos = { pos[0], pos[1], pos[2] };
            initialOffsets[gameObject] = initialPos - rigidBodyInitialPos;
        }

        glm::dvec3 adjustedPosition = glm::dvec3(pos[0], pos[1], pos[2]) + initialOffsets[gameObject];
        goTransform->SetPosition(adjustedPosition);

        // Aplicar la rotación solo si ha cambiado
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
    if (linkPhysicsToScene) {
        dynamicsWorld->stepSimulation(static_cast<btScalar>(dt), 10);
        //GameObjectsSync
        SyncTransforms();
    }
    else
    {
        //Line to sync the colliders to the gameobjects
        //SyncCollidersToGameObjects();
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
    // Obtener el componente Transform del GameObject
    Transform_Component* transform = go.GetTransform();

    // Establecer la posición inicial del GameObject a la posición de la cámara
    transform->SetPosition(cameraPosition);

    // Configurar la transformación inicial para el cuerpo rígido
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
    startTransform.setRotation(btQuaternion(0, 0, 0, 1)); // Sin rotación inicial

    // Crear la forma de colisión para la esfera
    btSphereShape* sphereShape = new btSphereShape(radius);

    // Crear el estado de movimiento
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

    // Calcular la inercia si el cuerpo tiene masa
    btVector3 inertia(0, 0, 0);
    if (mass > 0.0f) {
        sphereShape->calculateLocalInertia(mass, inertia);
    }

    // Configurar la información del cuerpo rígido
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphereShape, inertia);
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    rigidBody->setRestitution(0.7f);
    // Añadir el cuerpo rígido al mundo de Bullet
    dynamicsWorld->addRigidBody(rigidBody);

    // Asociar el cuerpo rígido con el GameObject en el mapa
    gameObjectRigidBodyMap[&go] = rigidBody;

    // Obtener la dirección de la cámara y aplicar una fuerza inicial
    //glm::vec3 cameraDirection = Application->camera->GetTransform().GetForward(); // Dirección de la cámara
    btVector3 btForce = btVector3(cameraDirection.x, cameraDirection.y, cameraDirection.z) * forceMagnitude;
    rigidBody->applyCentralImpulse(btForce);

    // Mensaje de confirmación
    std::cout << "Physics sphere spawned at: ("
        << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z
        << ") with force: (" << btForce.x() << ", " << btForce.y() << ", " << btForce.z() << ")\n";
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


void PhysicsModule::FreezeRotations(GameObject& go) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setAngularFactor(btVector3(0, 0, 0)); // Lock all rotations
        std::cout << "Rotations frozen for GameObject.\n";
    }
}


void PhysicsModule::UnlockRotations(GameObject& go) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setAngularFactor(btVector3(1, 1, 1)); // Unlock all rotations
        std::cout << "Rotations unlocked for GameObject.\n";
    }
}

void PhysicsModule::SetGravity(GameObject& go, const glm::vec3& gravity) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;
        rigidBody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
        std::cout << "Gravity set for GameObject: " << gravity.x << ", " << gravity.y << ", " << gravity.z << "\n";
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

void PhysicsModule::SetKinematic(GameObject& go, bool isKinematic) {
    auto it = gameObjectRigidBodyMap.find(&go);
    if (it != gameObjectRigidBodyMap.end()) {
        btRigidBody* rigidBody = it->second;

        if (isKinematic) {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else {
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        }

        std::cout << "GameObject set to " << (isKinematic ? "kinematic" : "dynamic") << ".\n";
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


