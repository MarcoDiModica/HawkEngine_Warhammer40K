#include "SkeletalAnimationComponent.h"

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner, Animation* animation) : Component(owner)
{
	animator = new Animator(animation);
}

void SkeletalAnimationComponent::Start()
{
	animator->PlayAnimation(testAnimation);
}

void SkeletalAnimationComponent::Update(float deltaTime)
{
	animator->UpdateAnimation(deltaTime);
}
