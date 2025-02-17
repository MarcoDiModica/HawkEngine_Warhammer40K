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
        // Implementación específica para SkeletalAnimationComponent
    }

    void Update(float deltaTime) override {
        // Implementación específica para SkeletalAnimationComponent
    }

    void Destroy() override {
        // Implementación específica para SkeletalAnimationComponent
    }

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
		testAnimation = animation;
	}
    
private:
	Animator* animator;
	Animation* testAnimation;

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

