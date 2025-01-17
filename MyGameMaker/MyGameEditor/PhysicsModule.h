#pragma once
#include "Module.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>

class GameObject;

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

    void CreatePhysicsForGameObject(GameObject& go, float mass); // Vincula físicas a un GameObject
    void SyncTransforms(); // Sincroniza transformaciones físicas con el motor

private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    btCollisionShape* cubeShape;

    // Relación entre GameObject y su cuerpo rígido
    std::unordered_map<GameObject*, btRigidBody*> gameObjectRigidBodyMap;
};
