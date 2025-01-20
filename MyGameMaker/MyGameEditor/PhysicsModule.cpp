#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/PhysBody3D.h"
#include "../MyGameEngine/PhysVehicle3D.h"
#include <iostream>
#include <glm/glm.hpp>


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

    return true;
}

void PhysicsModule::CreatePhysicsForGameObject(GameObject& go, float mass) {

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

        // Actualiza la posición y rotación del GameObject
        auto goTransform = gameObject->GetTransform();
        glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
        glm::dvec3 deltaPos = newPosition - goTransform->GetPosition();
        goTransform->Translate(deltaPos);
        auto x = pos.x();
        auto y = pos.y();
        auto z = pos.z();
        auto v = glm::vec3(x, y, z);

        // Convertir btQuaternion a glm::quat y aplicar rotación
        glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
        glm::dvec3 deltaRot = newRotation - glm::dvec3(0.0, 0.0, 0.0);

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

        
        //glDisable(GL_LIGHTING);  // Desactiva la iluminación temporalmente
        auto rigidBodies = GetAllRigidBodies(dynamicsWorld);
        for (const auto& rigidBody : rigidBodies) {
            btVector3 min, max;
            rigidBody->getAabb(min, max);
            BoundingBox bbox(glm::vec3(min.getX(), min.getY(), min.getZ()), glm::vec3(max.getX(), max.getY(), max.getZ()));
            debugDrawer->drawBoundingBox(bbox, glm::vec3(1.0f, 0.0f, 0.0f));  // Dibuja la BoundingBox en rojo
        }
        //glEnable(GL_LIGHTING);  // Reactiva la iluminación
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

    for (int i = 0; i < info.num_wheels; ++i)
    {
        btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
        btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
        btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

        vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
    }
    // ---------------------
    PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
    body->setUserPointer(pvehicle);
    dynamicsWorld->addVehicle(vehicle);
    vehicles.add(pvehicle);

    // Crear componentes asociados al vehículo (ruedas y chasis)
    CreateVehicleComponents(pvehicle, info);

    return pvehicle;
}

void PhysicsModule::CreateVehicleComponents(PhysVehicle3D* vehicle, const VehicleInfo& info)
{
    // Crear las ruedas como cilindros
    for (int i = 0; i < info.num_wheels; ++i)
    {
        // Crear un cilindro para cada rueda
        GameObject* wheel = new GameObject("Wheel_" + std::to_string(i));
        Transform_Component* transform = wheel->GetTransform();

        // Posición inicial de la rueda
        glm::vec3 position(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
        transform->SetPosition(position);

        // Configurar la forma física del cilindro
        btCollisionShape* wheelShape = new btCylinderShape(btVector3(info.wheels[i].radius, info.wheels[i].width * 0.5f, info.wheels[i].radius));
        shapes.add(wheelShape);

        // Crear el cuerpo rígido para la rueda
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(position.x, position.y, position.z));

        btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, wheelShape, btVector3(0, 0, 0));
        btRigidBody* rigidBody = new btRigidBody(rbInfo);

        dynamicsWorld->addRigidBody(rigidBody);
        gameObjectRigidBodyMap[wheel] = rigidBody;

        std::cout << "Wheel_" << i << " created at position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
    }

    // Crear el chasis como un cubo
    GameObject* chassis = new GameObject("Chassis");
    Transform_Component* chassisTransform = chassis->GetTransform();

    // Posición inicial del chasis
    glm::vec3 chassisPosition(0, 1, 0); // Cambia esto según sea necesario
    chassisTransform->SetPosition(chassisPosition);

    // Configurar la forma física del cubo
    btCollisionShape* chassisShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
    shapes.add(chassisShape);

    // Crear el cuerpo rígido para el chasis
    btTransform chassisTransformBt;
    chassisTransformBt.setIdentity();
    chassisTransformBt.setOrigin(btVector3(chassisPosition.x, chassisPosition.y, chassisPosition.z));

    btDefaultMotionState* chassisMotionState = new btDefaultMotionState(chassisTransformBt);
    btRigidBody::btRigidBodyConstructionInfo chassisRbInfo(info.mass, chassisMotionState, chassisShape, btVector3(0, 0, 0));
    btRigidBody* chassisRigidBody = new btRigidBody(chassisRbInfo);

    dynamicsWorld->addRigidBody(chassisRigidBody);
    gameObjectRigidBodyMap[chassis] = chassisRigidBody;

    std::cout << "Chassis created at position: (" << chassisPosition.x << ", " << chassisPosition.y << ", " << chassisPosition.z << ")\n";
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
