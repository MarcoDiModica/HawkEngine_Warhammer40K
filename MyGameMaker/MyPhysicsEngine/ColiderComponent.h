#pragma once
#include "../MyGameEngine/Component.h"


    //class PhysicsModule;

    class ColiderComponent : public Component {
    public:
         ColiderComponent(GameObject* owner);
        ~ColiderComponent() override = default;
        
        void Start() override {};
        void Update(float deltaTime) override {};
        void Destroy() override {};
        
        ComponentType GetType() const override { return ComponentType::COLLIDER; }
        
		std::unique_ptr<Component> Clone(GameObject* owner) override { return std::make_unique<ColiderComponent>(owner); }

    private:
        //PhysicsModule* physicsModule;

    };


