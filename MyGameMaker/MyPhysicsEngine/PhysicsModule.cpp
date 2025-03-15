
#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "RigidBodyComponent.h"
#include <iostream>
#include <glm/glm.hpp>


constexpr float fixedDeltaTime = 0.002; // 60 updates per second
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
    // Map to store the previous physics transform for interpolation.
    static std::unordered_map<GameObject*, btTransform> previousTransformMap;
    // Map to store initial offsets for root objects only.
    static std::unordered_map<GameObject*, glm::dvec3> offsetMap;

    // Compute interpolation factor (between 0 and 1)
    float interpolationFactor = accumulatedTime / fixedDeltaTime;

    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        if (!gameObject->HasComponent<RigidbodyComponent>())
            continue;

        btTransform currentBtTrans;
        if (rigidBody->getMotionState())
            rigidBody->getMotionState()->getWorldTransform(currentBtTrans);
        else
            currentBtTrans = rigidBody->getWorldTransform();

        // Get previous transform (initialize if not present)
        if (previousTransformMap.find(gameObject) == previousTransformMap.end())
            previousTransformMap[gameObject] = currentBtTrans;
        btTransform previousBtTrans = previousTransformMap[gameObject];

        // --- Interpolate Position ---
        btVector3 prevOrigin = previousBtTrans.getOrigin();
        btVector3 currOrigin = currentBtTrans.getOrigin();
        btVector3 interpOrigin = (1.0f - interpolationFactor) * prevOrigin + interpolationFactor * currOrigin;

        // --- Interpolate Rotation (slerp) ---
        btQuaternion prevRot = previousBtTrans.getRotation();
        btQuaternion currRot = currentBtTrans.getRotation();
        btQuaternion interpRot = prevRot.slerp(currRot, interpolationFactor);

        // Build the interpolated transform
        btTransform interpBtTrans;
        interpBtTrans.setOrigin(interpOrigin);
        interpBtTrans.setRotation(interpRot);

        // Update previous transform for next frame
        previousTransformMap[gameObject] = currentBtTrans;

        // Convert to glm types
        glm::dvec3 worldPosition(interpOrigin.x(), interpOrigin.y(), interpOrigin.z());
        glm::dquat worldRotation(interpRot.w(), interpRot.x(), interpRot.y(), interpRot.z());

        auto goTransform = gameObject->GetTransform();
        glm::dvec3 adjustedPosition;

        // Only for root objects use offset mapping.
        if (gameObject->GetParent()) {
            adjustedPosition = worldPosition;
        }
        else {
            if (offsetMap.find(gameObject) == offsetMap.end()) {
                glm::dvec3 initialPos = goTransform->GetPosition();
                offsetMap[gameObject] = initialPos - worldPosition;
            }
            adjustedPosition = worldPosition + offsetMap[gameObject];
        }

        // If the GameObject has a parent, convert world transform to local space.
        if (gameObject->GetParent()) {
            glm::dmat4 parentWorld = gameObject->GetParent()->GetTransform()->GetMatrix();
            glm::dmat4 invParent = glm::inverse(parentWorld);
            glm::dvec3 localPosition = glm::dvec3(invParent * glm::dvec4(adjustedPosition, 1.0));
            goTransform->SetLocalPosition(localPosition);

            glm::dquat parentWorldRot = glm::quat_cast(gameObject->GetParent()->GetTransform()->GetMatrix());
            glm::dquat localRotation = glm::inverse(parentWorldRot) * worldRotation;
            goTransform->SetRotationQuat(localRotation);
        }
        else {
            goTransform->SetLocalPosition(adjustedPosition);
            goTransform->SetRotationQuat(worldRotation);
        }
    }
}


void PhysicsModule::SyncCollidersToGameObjects() {
    for (auto& [gameObject, rigidBody] : gameObjectRigidBodyMap) {
        auto goTransform = gameObject->GetTransform();
        glm::vec3 position = goTransform->GetLocalPosition();
        glm::quat rotation = goTransform->GetRotation();

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
            else if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {
                btBvhTriangleMeshShape* meshShape = static_cast<btBvhTriangleMeshShape*>(shape);
                const btStridingMeshInterface* meshInterface = meshShape->getMeshInterface();

                const unsigned char* vertexBase;
                const unsigned char* indexBase;
                int numVerts, numFaces;
                PHY_ScalarType vertexType, indexType;
                int vertexStride, indexStride;

                meshInterface->getLockedReadOnlyVertexIndexBase(&vertexBase, numVerts, vertexType, vertexStride,
                    &indexBase, indexStride, numFaces, indexType);

                for (int i = 0; i < numFaces; i++) {
                    const unsigned int* indices = reinterpret_cast<const unsigned int*>(indexBase + i * indexStride);

                    const btScalar* v0 = reinterpret_cast<const btScalar*>(vertexBase + indices[0] * vertexStride);
                    const btScalar* v1 = reinterpret_cast<const btScalar*>(vertexBase + indices[1] * vertexStride);
                    const btScalar* v2 = reinterpret_cast<const btScalar*>(vertexBase + indices[2] * vertexStride);

                    btVector3 p0(v0[0], v0[1], v0[2]);
                    btVector3 p1(v1[0], v1[1], v1[2]);
                    btVector3 p2(v2[0], v2[1], v2[2]);

                    p0 = transform * p0;
                    p1 = transform * p1;
                    p2 = transform * p2;

                    debugDrawer->drawTriangle(glm::vec3(p0.x(), p0.y(), p0.z()),
                        glm::vec3(p1.x(), p1.y(), p1.z()),
                        glm::vec3(p2.x(), p2.y(), p2.z()),
                        glm::vec3(0.0f, 1.0f, 0.0f));
                }

                meshInterface->unLockReadOnlyVertexBase(0);
            }
        }
    }
}


void PhysicsModule::CallMonoCollision(GameObject* obj, const std::string& methodName, GameObject* other) {
    if (!obj) return;

    for (auto& script : obj->scriptComponents) {
        if (script) {
            script->InvokeMonoMethod(methodName, other);
        }
    }
}


void PhysicsModule::CheckCollisions() {
    static std::set<std::pair<GameObject*, GameObject*>> previousCollisions;
    std::set<std::pair<GameObject*, GameObject*>> currentCollisions;

    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* btObjA = contactManifold->getBody0();
        const btCollisionObject* btObjB = contactManifold->getBody1();

        GameObject* objA = nullptr;
        GameObject* objB = nullptr;

        for (const auto& pair : gameObjectRigidBodyMap) {
            if (pair.second == btObjA) objA = pair.first;
            if (pair.second == btObjB) objB = pair.first;
        }

        if (objA && objB) {
            ColliderComponent* colliderA = objA->GetComponent<ColliderComponent>();
            ColliderComponent* colliderB = objB->GetComponent<ColliderComponent>();
            std::pair<GameObject*, GameObject*> collisionPair = std::minmax(objA, objB);

            if (colliderA && colliderB) {
                bool isTriggerA = colliderA->IsTrigger();
                bool isTriggerB = colliderB->IsTrigger();
                bool isTriggerCollision = isTriggerA || isTriggerB;

                if (previousCollisions.find(collisionPair) == previousCollisions.end()) {
					// --------------- ON ENTER ---------------
                    if (isTriggerCollision) {
                        colliderA->OnTriggerEnter(colliderB);
                        colliderB->OnTriggerEnter(colliderA);

                        CallMonoCollision(objA, "OnTriggerEnter", objB);
                        CallMonoCollision(objB, "OnTriggerEnter", objA);
                    }
                    else {
                        colliderA->OnCollisionEnter(colliderB);
                        colliderB->OnCollisionEnter(colliderA);

                        CallMonoCollision(objA, "OnCollisionEnter", objB);
                        CallMonoCollision(objB, "OnCollisionEnter", objA);
                    }
                }
                else {
                    // --------------- ON STAY ---------------
                    if (isTriggerCollision) {
                        colliderA->OnTriggerStay(colliderB);
                        colliderB->OnTriggerStay(colliderA);

                        CallMonoCollision(objA, "OnTriggerStay", objB);
                        CallMonoCollision(objB, "OnTriggerStay", objA);
                    }
                    else {
                        colliderA->OnCollisionStay(colliderB);
                        colliderB->OnCollisionStay(colliderA);

                        CallMonoCollision(objA, "OnCollisionStay", objB);
                        CallMonoCollision(objB, "OnCollisionStay", objA);
                    }
                }
                currentCollisions.insert(collisionPair);
            }
        }
    }

	// --------------- ON EXIT ---------------
    for (const auto& collisionPair : previousCollisions) {
        if (currentCollisions.find(collisionPair) == currentCollisions.end()) {
            GameObject* objA = collisionPair.first;
            GameObject* objB = collisionPair.second;
            ColliderComponent* colliderA = objA->GetComponent<ColliderComponent>();
            ColliderComponent* colliderB = objB->GetComponent<ColliderComponent>();
            if (colliderA && colliderB) {
                bool isTriggerA = colliderA->IsTrigger();
                bool isTriggerB = colliderB->IsTrigger();
                if (isTriggerA || isTriggerB) {
                    colliderA->OnTriggerExit(colliderB);
                    colliderB->OnTriggerExit(colliderA);

                    CallMonoCollision(objA, "OnTriggerExit", objB);
                    CallMonoCollision(objB, "OnTriggerExit", objA);
                }
                else {
                    colliderA->OnCollisionExit(colliderB);
                    colliderB->OnCollisionExit(colliderA);

                    CallMonoCollision(objA, "OnCollisionExit", objB);
                    CallMonoCollision(objB, "OnCollisionExit", objA);
                }
            }
        }
    }

    previousCollisions = currentCollisions;
}



bool PhysicsModule::Update(double dt) {
    //if (linkPhysicsToScene) {
    //    accumulatedTime += static_cast<float>(dt);
    //    // Perform fixed steps
    //    while (accumulatedTime >= fixedDeltaTime) {
    //        dynamicsWorld->stepSimulation(fixedDeltaTime, 10);
    //        accumulatedTime -= fixedDeltaTime;
    //    }
    //    // After stepping, interpolate and sync transforms
    //    SyncTransforms();
    //    CheckCollisions();
    //}
    //DrawDebugDrawer();
    if (linkPhysicsToScene) {
		dynamicsWorld->stepSimulation(dt, 16, fixedDeltaTime);
		SyncTransforms();
        CheckCollisions();
	}
    
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

void PhysicsModule::SpawnPhysSphereWithForce(GameObject& launcher, GameObject& sphere, float radius, float mass, float forceMagnitude) {
    Transform_Component* transform = launcher.GetTransform();
    glm::vec3 spawnPosition = transform->GetPosition();

    glm::vec3 spawnDirection = glm::vec3(transform->GetForward().x, 0.0f, transform->GetForward().z);
    float offsetDistance = 1.0f;
    spawnPosition += spawnDirection * offsetDistance;

    Transform_Component* sphereTransform = sphere.GetTransform();
    sphereTransform->SetPosition(spawnPosition);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(spawnPosition.x, spawnPosition.y, spawnPosition.z));
    startTransform.setRotation(btQuaternion(0, 0, 0, 1)); 

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
    gameObjectRigidBodyMap[&sphere] = rigidBody;

    btVector3 btForce = btVector3(spawnDirection.x * forceMagnitude, 0.0f, spawnDirection.z * forceMagnitude);
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


