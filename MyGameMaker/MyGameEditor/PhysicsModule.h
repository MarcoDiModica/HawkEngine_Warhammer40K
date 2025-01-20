#pragma once
#include "Module.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "../MyGameEngine/p2List.h"
#include "../MyGameEngine/DebugDrawerPhysics.h"
//#include "../MyGameEditor/App.h"


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


class PhysicsModule : public Module {
public:
    PhysicsModule(App* app);
    ~PhysicsModule();

    bool Awake() override;
    
    bool Start() override;
    bool PreUpdate() override;
    bool Update(double dt) override;
    bool PostUpdate() override;
    bool CleanUp() override;

    void DrawDebugDrawer();

    void CreatePhysicsForGameObject(GameObject& go, float mass);
    //void CreatePhysicsPlane();
    void SyncTransforms();

    void AddConstraintP2P(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB);
    PhysVehicle3D* AddVehicle(const VehicleInfo& info);
    void SyncVehicleComponents(PhysVehicle3D* vehicle, GameObject* chassis, std::vector<GameObject*> wheels);
    // Añade una restricción de bisagra (Hinge)
    void AddConstraintHinge(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB,
        const glm::vec3& axisA, const glm::vec3& axisB, bool disable_collision = false);
    void SpawnPhysSphereWithForce(const glm::vec3& cameraPosition, float radius, float forceMagnitude);

    p2List<FinalVehicleInfo*> vehicles;
private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    p2List<btTypedConstraint*> constraints;
	p2List<btCollisionShape*> shapes;
    btCollisionShape* cubeShape;
    btDefaultVehicleRaycaster* vehicle_raycaster;

    // Relación entre GameObject y su cuerpo rígido
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMap;
	DebugDrawerPhysics* debugDrawer;
};
