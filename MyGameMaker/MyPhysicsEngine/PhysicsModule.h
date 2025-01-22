#pragma once
#include "../MyGameEditor/Module.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "p2List.h"
#include "DebugDrawerPhysics.h"
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

    void CreatePhysicsForCube(GameObject& go, float mass);
    //void CreatePhysicsPlane();
    void SyncTransforms();

    void AddConstraintP2P(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB);
    PhysVehicle3D* AddVehicle(const VehicleInfo& info);
    void SyncVehicleComponents(PhysVehicle3D* vehicle, GameObject* chassis, std::vector<GameObject*> wheels);
    // A�ade una restricci�n de bisagra (Hinge)
    void AddConstraintHinge(GameObject& goA, GameObject& goB, const glm::vec3& anchorA, const glm::vec3& anchorB,
        const glm::vec3& axisA, const glm::vec3& axisB, bool disable_collision = false);
    void SpawnPhysSphereWithForce(GameObject& go, float radius, float mass, const glm::vec3& forceDirection, float forceMagnitude);
    void SetGlobalRestitution(float restitutionValue);

    p2List<FinalVehicleInfo*> vehicles;
    btDiscreteDynamicsWorld* dynamicsWorld;
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
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMap;
	DebugDrawerPhysics* debugDrawer;
};
