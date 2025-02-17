#include "SkeletalAnimationComponent.h"

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner)
{
	
}

void SkeletalAnimationComponent::Start()
{
	animator->PlayAnimation(testAnimation);
}

void SkeletalAnimationComponent::Update(float deltaTime)
{
	animator->UpdateAnimation(deltaTime);
}
