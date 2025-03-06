
#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "p2List.h"
#include "DebugDrawerPhysics.h"


class GameObject;
struct VehicleInfo;
struct PhysVehicle3D;

struct FinalVehicleInfo {

    FinalVehicleInfo(GameObject* chassis, std::vector<GameObject*> wheels, PhysVehicle3D* bulletVehicle) :
        chassis(chassis), wheels(wheels), bulletVehicle(bulletVehicle) {}
    GameObject* chassis;
    std::vector<GameObject*> wheels;
    PhysVehicle3D* bulletVehicle;
};


class PhysicsModule {
public:
    PhysicsModule();
    ~PhysicsModule();

    bool Awake();

    void SetBounciness(GameObject& go, float restitution);

    void EnableContinuousCollision(GameObject& go);

    bool Start();
    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();
    void AddForceToCollider(GameObject& go, const glm::vec3& force);
    bool CleanUp();

    void DrawDebugDrawer();

    void CheckCollisions() {
        int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* btObjA = contactManifold->getBody0();
            const btCollisionObject* btObjB = contactManifold->getBody1();

            // Buscar los GameObjects en el mapa gameObjectRigidBodyMap
            GameObject* objA = nullptr;
            GameObject* objB = nullptr;

            for (const auto& pair : gameObjectRigidBodyMap) {
                if (pair.second == btObjA) objA = pair.first;
                if (pair.second == btObjB) objB = pair.first;
            }

            // Si encontramos los GameObjects correspondientes
            if (objA && objB) {
                // Obtener ColliderComponent de cada objeto
                ColliderComponent* colliderA = objA->GetComponent<ColliderComponent>();
                ColliderComponent* colliderB = objB->GetComponent<ColliderComponent>();

                if (colliderA) {
                    colliderA->OnCollisionEnter(colliderB);
                }
                if (colliderB) {
                    colliderB->OnCollisionEnter(colliderA);
                }
            }
        }
    }


    //void CreatePhysicsPlane();
    void SyncTransforms();
    void SyncCollidersToGameObjects();

    void AddConstraintP2P(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB);
    // A�ade una restricci�n de bisagra (Hinge)
    void AddConstraintHinge(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB,
        const glm::vec3& axisA, const glm::vec3& axisB, bool disable_collision = false);
    void SpawnPhysSphereWithForce(GameObject& launcher, GameObject& sphere, float radius, float mass, float forceMagnitude);
    void SetGlobalRestitution(float restitutionValue);

    void SetColliderFriction(GameObject& go, float friction);

    p2List<FinalVehicleInfo*> vehicles;
    btDiscreteDynamicsWorld* dynamicsWorld;
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMap;
    bool linkPhysicsToScene = false;
private:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    p2List<btTypedConstraint*> constraints;
    p2List<btCollisionShape*> shapes;
    btCollisionShape* cubeShape;
    btDefaultVehicleRaycaster* vehicle_raycaster;

    // Relaci�n entre GameObject y su cuerpo r�gido
    DebugDrawerPhysics* debugDrawer;
};

