#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/PhysBody3D.h"
#include "../MyGameEngine/PhysVehicle3D.h"
#include <iostream>
#include <glm/glm.hpp>
#include "App.h"


PhysicsModule::PhysicsModule(App* app)
    : Module(app), dynamicsWorld(nullptr), cubeShape(nullptr) {}

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
                continue; // Salta si no hay forma de colisión
            }

            btCollisionShape* shape = rigidBody->getCollisionShape();

            if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
                // Dibuja un cubo si la forma es un cubo
                btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
                btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();

                btTransform transform;
                rigidBody->getMotionState()->getWorldTransform(transform);

                BoundingBox bbox(
                    glm::vec3(transform.getOrigin().getX() - halfExtents.getX(),
                        transform.getOrigin().getY() - halfExtents.getY(),
                        transform.getOrigin().getZ() - halfExtents.getZ()),
                    glm::vec3(transform.getOrigin().getX() + halfExtents.getX(),
                        transform.getOrigin().getY() + halfExtents.getY(),
                        transform.getOrigin().getZ() + halfExtents.getZ())
                );

                debugDrawer->drawBoundingBox(bbox, glm::vec3(0.0f, 1.0f, 0.0f)); // Dibuja el cubo en verde
            }
            else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
                // Dibuja una esfera si la forma es una esfera
                btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
                float radius = sphereShape->getRadius();

                btTransform transform;
                rigidBody->getMotionState()->getWorldTransform(transform);

                glm::vec3 center(transform.getOrigin().getX(),
                    transform.getOrigin().getY(),
                    transform.getOrigin().getZ());

                debugDrawer->drawSphere(center, radius, glm::vec3(1.0f, 0.0f, 0.0f), 16); // Dibuja la esfera en rojo
            }
            else {
                // Otros tipos de colisión (por ahora no renderizamos)
                continue;
            }
        }
    }
}


bool PhysicsModule::Update(double dt) {
    dynamicsWorld->stepSimulation(static_cast<btScalar>(dt), 10);
    SyncTransforms();

    for (p2List_item<FinalVehicleInfo*>* item = vehicles.getFirst(); item != nullptr; item = item->next) {
        FinalVehicleInfo* vehicle = item->data;
        SyncVehicleComponents(vehicle->bulletVehicle, vehicle->chassis, vehicle->wheels);
    }
   

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

//Crea el coche en bullet
PhysVehicle3D* PhysicsModule::AddVehicle(const VehicleInfo& info)
{
    btCompoundShape* comShape = new btCompoundShape();
    shapes.add(comShape);

    btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
    shapes.add(colShape);

    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

    comShape->addChildShape(trans, colShape);

    btTransform startTransform;
    startTransform.setIdentity();

    btVector3 localInertia(0, 0, 0);
    comShape->calculateLocalInertia(info.mass, localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

    btRigidBody* body = new btRigidBody(rbInfo);
    body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    body->setActivationState(DISABLE_DEACTIVATION);

    dynamicsWorld->addRigidBody(body);

    btRaycastVehicle::btVehicleTuning tuning;
    tuning.m_frictionSlip = info.frictionSlip;
    tuning.m_maxSuspensionForce = info.maxSuspensionForce;
    tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
    tuning.m_suspensionCompression = info.suspensionCompression;
    tuning.m_suspensionDamping = info.suspensionDamping;
    tuning.m_suspensionStiffness = info.suspensionStiffness;

    btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

    vehicle->setCoordinateSystem(0, 1, 2);

    for (int i = 0; i < info.num_wheels; ++i) {
        btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
        btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z); // Dirección de suspensión
        btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);             // Eje de rotación

        // Asegurarte de que 'axis' sea perpendicular a 'dir'
        // Por ejemplo: dir es (0, -1, 0), entonces axis debe ser (-1, 0, 0) o (1, 0, 0)
        vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
    }
    // ---------------------
    PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
    body->setUserPointer(pvehicle);
    dynamicsWorld->addVehicle(vehicle);

    // Crear componentes asociados al vehículo (ruedas y chasis)
    //CreateVehicleComponents(pvehicle, info);

    return pvehicle;
}

void PhysicsModule::SyncVehicleComponents(PhysVehicle3D* vehicle, GameObject* chassis, std::vector<GameObject*> wheels) {
    if (!vehicle || !chassis || wheels.size() != vehicle->info.num_wheels) {
        std::cerr << "Error: Mismatch between vehicle and game objects.\n";
        return;
    }

    const VehicleInfo& info = vehicle->info;

    // Sincronizar el chasis
    {
        const btTransform& chassisTransform = vehicle->vehicle->getChassisWorldTransform();
        btVector3 chassisPos = chassisTransform.getOrigin();
        btQuaternion chassisRot = chassisTransform.getRotation();

        glm::vec3 newChassisPos(chassisPos.getX(), chassisPos.getY(), chassisPos.getZ());
        glm::quat newChassisRot(chassisRot.getW(), chassisRot.getX(), chassisRot.getY(), chassisRot.getZ());

        glm::dvec3 newPosition = { chassisPos[0], chassisPos[1], chassisPos[2] };
        glm::dvec3 deltaPos = newPosition - chassis->GetTransform()->GetPosition();
        chassis->GetTransform()->Translate(deltaPos);


        glm::dvec3 newRotation = glm::radians(glm::dvec3(chassisRot[0], chassisRot[1], chassisRot[2]));
        glm::dvec3 currentRotation = glm::radians(chassis->GetTransform()->GetEulerAngles());
        glm::dvec3 deltaRot = newRotation - currentRotation;

        chassis->GetTransform()->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
        chassis->GetTransform()->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
        chassis->GetTransform()->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
    }

    // Sincronizar las ruedas
    for (size_t i = 0; i < wheels.size(); ++i) {
        btTransform wheelTransform;
        vehicle->vehicle->updateWheelTransform(static_cast<int>(i), true); // Obtener transformación actualizada
        wheelTransform = vehicle->vehicle->getWheelTransformWS(static_cast<int>(i));

        btVector3 wheelPos = wheelTransform.getOrigin();
        btQuaternion wheelRot = wheelTransform.getRotation();

        glm::vec3 newWheelPos(wheelPos.getX(), wheelPos.getY(), wheelPos.getZ());
        glm::quat newWheelRot(wheelRot.getW(), wheelRot.getX(), wheelRot.getY(), wheelRot.getZ());

        glm::dvec3 newPosition = { wheelPos[0], wheelPos[1], wheelPos[2] };
        glm::dvec3 deltaPos = newPosition - wheels[i]->GetTransform()->GetPosition();
        wheels[i]->GetTransform()->Translate(deltaPos);


        glm::dvec3 newRotation = glm::radians(glm::dvec3(wheelRot[0], wheelRot[1], wheelRot[2]));
        glm::dvec3 currentRotation = glm::radians(wheels[i]->GetTransform()->GetEulerAngles());
        glm::dvec3 deltaRot = newRotation - currentRotation;

        wheels[i]->GetTransform()->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
        wheels[i]->GetTransform()->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
        wheels[i]->GetTransform()->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
    }
}
void PhysicsModule::SpawnPhysSphereWithForce(GameObject& go, float radius, float mass, const glm::vec3& cameraPosition, float forceMagnitude) {
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
    glm::vec3 cameraDirection = Application->camera->GetTransform().GetForward(); // Dirección de la cámara
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
