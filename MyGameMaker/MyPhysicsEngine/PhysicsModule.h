
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

	//Helper Functions
    void FreezeRotations(GameObject& go);

    void UnlockRotations(GameObject& go);

    void SetGravity(GameObject& go, const glm::vec3& gravity);

    void SetBounciness(GameObject& go, float restitution);

    void SetKinematic(GameObject& go, bool isKinematic);

    void EnableContinuousCollision(GameObject& go);

    bool Start();
    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();
    void AddForceToCollider(GameObject& go, const glm::vec3& force);
    bool CleanUp();

    void DrawDebugDrawer();

    void CreatePhysicsForCube(GameObject& go, float mass);
    //void CreatePhysicsPlane();
    void SyncTransforms();
    void SyncCollidersToGameObjects();

    void AddConstraintP2P(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB);
    // Añade una restricción de bisagra (Hinge)
    void AddConstraintHinge(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB,
        const glm::vec3& axisA, const glm::vec3& axisB, bool disable_collision = false);
    void SpawnPhysSphereWithForce(GameObject& go, float radius, float mass, const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, float forceMagnitude);
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

    // Relación entre GameObject y su cuerpo rígido
    DebugDrawerPhysics* debugDrawer;
};

