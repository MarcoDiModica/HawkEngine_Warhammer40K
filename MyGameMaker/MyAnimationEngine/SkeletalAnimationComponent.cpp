#include "SkeletalAnimationComponent.h"
#include <iostream>

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner)
{
    name = "SkeletalAnimation_Component";
}

SkeletalAnimationComponent::SkeletalAnimationComponent(const SkeletalAnimationComponent& other) : Component(other)
{
    animator = other.animator;
    testAnimation = other.testAnimation;
}

SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(const SkeletalAnimationComponent& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        animator = other.animator;
        testAnimation = other.testAnimation;
    }
    return *this;
}

SkeletalAnimationComponent::SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept : Component(std::move(other))
{
    animator = other.animator;
    testAnimation = other.testAnimation;
}

SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(SkeletalAnimationComponent&& other) noexcept
{
    if (this != &other)
    {
        Component::operator=(std::move(other));
        animator = other.animator;
        testAnimation = other.testAnimation;
        other.animator = nullptr;
        other.testAnimation = nullptr;
    }
    return *this;
}


void SkeletalAnimationComponent::Start()
{
	animator = new Animator(testAnimation);
	animator->PlayAnimation(testAnimation);
}

void SkeletalAnimationComponent::Update(float deltaTime)
{
	animator->UpdateAnimation(deltaTime);
}

void SkeletalAnimationComponent::Destroy()
{
}