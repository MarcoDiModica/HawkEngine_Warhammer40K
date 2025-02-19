#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include <iostream>
#include <glm/glm.hpp>


PhysicsModule::PhysicsModule(): dynamicsWorld(nullptr), cubeShape(nullptr) {}

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

//PREVIOUS SYNC TRANSFORMS FUNCTION FOR DEBUG 

//void PhysicsModule::SyncTransforms() {
//    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
//        btTransform transform;
//        if (rigidBody->getMotionState()) {
//            rigidBody->getMotionState()->getWorldTransform(transform);
//        }
//
//        btVector3 pos = transform.getOrigin();
//        btQuaternion rot = transform.getRotation();
//
//        // Actualiza la posición y rotación del GameObject
//        auto goTransform = gameObject->GetTransform();
//        glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
//        glm::dvec3 deltaPos = newPosition - goTransform->GetPosition();
//        goTransform->Translate(deltaPos);
//        auto x = pos.x();
//        auto y = pos.y();
//        auto z = pos.z();
//        auto v = glm::vec3(x, y, z);
//
//        // Convertir btQuaternion a glm::quat y aplicar rotación
//        glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
//        glm::dvec3 deltaRot = newRotation - glm::dvec3(0.0, 0.0, 0.0);
//
//        goTransform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
//        goTransform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
//        goTransform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
//
//        std::cout << "GameObject position updated to: ("
//            << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << ")\n";
//    }
//}



void PhysicsModule::SyncTransforms() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        btTransform transform;
        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->getWorldTransform(transform);
        }

        btVector3 pos = transform.getOrigin();
        btQuaternion rot = transform.getRotation();

        auto goTransform = gameObject->GetTransform();

        //TESTING
        //glm::quat newRotation = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
        //goTransform->SetRotationQuat(newRotation);

        //// Calcular delta de posición y aplicar traslación
        //glm::dvec3 newPosition = { pos.getX(), pos.getY(), pos.getZ() };
        //glm::dvec3 currentPosition = goTransform->GetPosition();
        //glm::dvec3 deltaPos = newPosition - currentPosition;

        //// Aplica la traslación
        //goTransform->Translate(deltaPos);


        glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
        glm::dvec3 deltaPos = newPosition - goTransform->GetPosition();
        goTransform->Translate(deltaPos);
        auto x = pos.x();
        auto y = pos.y();
        auto z = pos.z();
        auto v = glm::vec3(x, y, z);

        glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
        glm::dvec3 currentRotation = glm::radians(goTransform->GetEulerAngles());
        glm::dvec3 deltaRot = newRotation - currentRotation;

        goTransform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
        goTransform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
        goTransform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));

        std::cout << "GameObject position updated to: ("
            << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << ")\n";
    }
}

void PhysicsModule::SyncCollidersToGameObjects() {   
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        auto goTransform = gameObject->GetTransform();
        glm::vec3 position = goTransform->GetPosition();
        glm::quat rotation = goTransform->GetRotation();

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(position.x, position.y, position.z));
        transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

        if (rigidBody->getMotionState()) {
            rigidBody->getMotionState()->setWorldTransform(transform);
        } else {
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
    DrawDebugDrawer();
    
   

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

void PhysicsModule::AddConstraintP2P(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB) {
    // Obtén los cuerpos rígidos de los GameObjects
    auto itA = gameObjectRigidBodyMap.find(&goA);
    auto itB = gameObjectRigidBodyMap.find(&goB);

    if (itA == gameObjectRigidBodyMap.end() || itB == gameObjectRigidBodyMap.end()) {
        std::cerr << "Error: Uno o ambos GameObjects no tienen cuerpos rígidos asociados.\n";
        return;
    }

    btRigidBody* bodyA = itA->second;
    btRigidBody* bodyB = itB->second;

    btTypedConstraint* p2p = new btPoint2PointConstraint(
        *bodyA,
        *bodyB,
        btVector3(anchorA.x, anchorA.y, anchorA.z),
        btVector3(anchorB.x, anchorB.y, anchorB.z));

    dynamicsWorld->addConstraint(p2p);
    constraints.add(p2p); // Agregar a la lista de constraints para limpieza
    p2p->setDbgDrawSize(2.0f);

    std::cout << "P2P Constraint added between GameObjects.\n";
}

void PhysicsModule::AddConstraintHinge(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB, const glm::vec3& axisA, const glm::vec3& axisB, bool disable_collision) {
    auto itA = gameObjectRigidBodyMap.find(&goA);
    auto itB = gameObjectRigidBodyMap.find(&goB);

    if (itA == gameObjectRigidBodyMap.end() || itB == gameObjectRigidBodyMap.end()) {
        std::cerr << "Error: One or both GameObjects do not have associated rigid bodies.\n";
        return;
    }

    btRigidBody* bodyA = itA->second;
    btRigidBody* bodyB = itB->second;

    // Validate bodies
    if (bodyA == nullptr || bodyB == nullptr) {
        std::cerr << "Error: One or both rigid bodies are null.\n";
        return;
    }

    if (bodyA == bodyB) {
        std::cerr << "Error: Attempting to add a hinge constraint to the same rigid body.\n";
        return;
    }

    btHingeConstraint* hinge = new btHingeConstraint(
        *bodyA, *bodyB,
        btVector3(anchorA.x, anchorA.y, anchorA.z),
        btVector3(anchorB.x, anchorB.y, anchorB.z),
        btVector3(axisA.x, axisA.y, axisA.z),
        btVector3(axisB.x, axisB.y, axisB.z)
    );

    // Ensure dynamicsWorld is ready
    if (dynamicsWorld == nullptr) {
        std::cerr << "Error: dynamicsWorld is not initialized.\n";
        delete hinge;
        return;
    }

    try {
        dynamicsWorld->addConstraint(hinge, disable_collision);
        constraints.add(hinge);
        hinge->setDbgDrawSize(2.0f);
        std::cout << "Hinge Constraint added between GameObjects.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while adding constraint: " << e.what() << "\n";
        delete hinge;
    }
}


void PhysicsModule::SpawnPhysSphereWithForce(GameObject& go, float radius, float mass, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, float forceMagnitude){
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

bool PhysicsModule::Start() {
    return true;
}

bool PhysicsModule::PreUpdate() {
    return true;
}

bool PhysicsModule::PostUpdate() {
    return true;
}
