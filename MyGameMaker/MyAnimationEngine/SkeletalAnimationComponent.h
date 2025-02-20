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
        return ComponentType::ANIMATION; // Cambia a un tipo específico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    MonoObject* GetSharp() override {
        return CsharpReference;
    }

    void SetAnimation(Animation* animation) {
        testAnimation = std::make_unique<Animation>(*animation);
    }

    Animation* GetAnimation() const {
        return testAnimation.get();
    }
    Animation animationTest;
    float patatudo;
private:
    std::unique_ptr<Animator> animator;
    std::unique_ptr<Animation> testAnimation;
    
 

protected:
    YAML::Node encode() override {
        YAML::Node node = Component::encode();
        // Añadir atributos específicos de SkeletalAnimationComponent
        return node;
    }

    bool decode(const YAML::Node& node) override {
        if (!Component::decode(node))
            return false;
        // Extraer atributos específicos de SkeletalAnimationComponent
        return true;
    }
};

