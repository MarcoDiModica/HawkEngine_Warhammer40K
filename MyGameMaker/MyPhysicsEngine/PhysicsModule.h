#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_map>
#include <memory>

class GameObject;


    class PhysicsModule {
    public:
        PhysicsModule();
        virtual ~PhysicsModule();

        bool Init();

        bool Start();
        bool PreUpdate();
        bool Update(double dt);
        bool PostUpdate();
        bool CleanUp();

        void CreatePhysicsForGameObject(GameObject& go, float mass);
        //void CreatePhysicsPlane();
        void SyncTransforms();

        bool hasStartedBullet = false;

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


