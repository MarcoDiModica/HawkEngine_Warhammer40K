#include "SkeletalAnimationComponent.h"
#include <iostream>

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner)
{
    name = "SkeletalAnimation_Component";
}

SkeletalAnimationComponent::SkeletalAnimationComponent(const SkeletalAnimationComponent& other) : Component(other) {
    if (other.animator) {
        animator = std::make_unique<Animator>(*other.animator);
    }
    if (other.testAnimation) {
        testAnimation = std::make_unique<Animation>(*other.testAnimation);
    }
}

SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(const SkeletalAnimationComponent& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        if (other.animator) {
            animator = std::make_unique<Animator>(*other.animator);
        } else {
            animator.reset();
        }
        if (other.testAnimation) {
            testAnimation = std::make_unique<Animation>(*other.testAnimation);
        } else {
            testAnimation.reset();
        }
    }
    return *this;
}

SkeletalAnimationComponent::SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept : Component(std::move(other)) {
    animator = std::move(other.animator);
    testAnimation = std::move(other.testAnimation);
}


SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(SkeletalAnimationComponent&& other) noexcept {
    if (this != &other) {
        Component::operator=(std::move(other));
        animator = std::move(other.animator);
        testAnimation = std::move(other.testAnimation);
    }
    return *this;
}

void SkeletalAnimationComponent::Start()
{
    animator = std::make_unique<Animator>(testAnimation.get());
    animator->PlayAnimation(testAnimation.get());
}

void SkeletalAnimationComponent::Update(float deltaTime)
{
    if (isPlaying) 
    {
        animator->UpdateAnimation(deltaTime);
    }
}

void SkeletalAnimationComponent::Destroy()
{
}