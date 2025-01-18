#include "PhysicsModule.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyPhysicsEngine/PhysBody3D.h"
#include <iostream>


    PhysicsModule::PhysicsModule() 
    {
		Init();
    }

    PhysicsModule::~PhysicsModule() {
        CleanUp();
    }

    bool PhysicsModule::Init() {
        // Inicialización del sistema de físicas de Bullet
        if (!hasStartedBullet) {
            broadphase = new btDbvtBroadphase();
            collisionConfiguration = new btDefaultCollisionConfiguration();
            dispatcher = new btCollisionDispatcher(collisionConfiguration);
            solver = new btSequentialImpulseConstraintSolver();
            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
            dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
            hasStartedBullet = true;
        }

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

            glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
            glm::dvec3 deltaRot = newRotation - glm::dvec3(0.0, 0.0, 0.0);

            goTransform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
            goTransform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
            goTransform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));


            std::cout << "GameObject position updated to: ("
                << pos.x() << ", " << pos.y() << ", " << pos.z() << ")\n";
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

    bool PhysicsModule::Start() {
        return true;
    }

    bool PhysicsModule::PreUpdate() {
        return true;
    }

    bool PhysicsModule::PostUpdate() {
        return true;
    }


