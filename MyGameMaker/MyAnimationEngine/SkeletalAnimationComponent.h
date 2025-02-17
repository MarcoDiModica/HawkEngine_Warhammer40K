#pragma once
#include "MyGameEngine/Component.h"
#include "Animator.h"
#include "Animation.h"

class SkeletalAnimationComponent : public Component
{
public:
    explicit SkeletalAnimationComponent(GameObject* owner, Animation* animation) : Component(owner) {}
    virtual ~SkeletalAnimationComponent() = default;

    void Start() override {
        // Implementaci�n espec�fica para SkeletalAnimationComponent
    }

    void Update(float deltaTime) override {
        // Implementaci�n espec�fica para SkeletalAnimationComponent
    }

    void Destroy() override {
        // Implementaci�n espec�fica para SkeletalAnimationComponent
    }

    ComponentType GetType() const override {
        return ComponentType::ANIMATION; // Cambia a un tipo espec�fico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    MonoObject* GetSharp() override {
        return CsharpReference;
    }
    
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

