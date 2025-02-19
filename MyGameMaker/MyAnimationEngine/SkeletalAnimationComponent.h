#pragma once
#include "../MyGameEngine/Component.h"
#include "Animator.h"
#include "Animation.h"

class SkeletalAnimationComponent : public Component
{
public:
    SkeletalAnimationComponent(GameObject* owner);
    virtual ~SkeletalAnimationComponent() = default;

    SkeletalAnimationComponent(const SkeletalAnimationComponent& other);
    SkeletalAnimationComponent& operator=(const SkeletalAnimationComponent& other);


    SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept;
    SkeletalAnimationComponent& operator=(SkeletalAnimationComponent&& other) noexcept;


    void Start() override;

    void Update(float deltaTime) override;

    void Destroy() override;

    ComponentType GetType() const override {
        return ComponentType::ANIMATION; // Cambia a un tipo espec�fico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    MonoObject* GetSharp() override {
        return CsharpReference;
    }

	void SetAnimation(Animation* animation) {
		testAnimation = animation;
	}

	Animation* GetAnimation() const{
		return testAnimation;
	}
    Animation animationTest;
    float patatudo;
private:
	Animator* animator;
	Animation* testAnimation;
    
 

protected:
    YAML::Node encode() override {
        YAML::Node node = Component::encode();
        // A�adir atributos espec�ficos de SkeletalAnimationComponent
        return node;
    }

    bool decode(const YAML::Node& node) override {
        if (!Component::decode(node))
            return false;
        // Extraer atributos espec�ficos de SkeletalAnimationComponent
        return true;
    }
};

