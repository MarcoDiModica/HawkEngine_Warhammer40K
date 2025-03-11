#include "BoneComponent.h"

#include "../MyGameEngine/GameObject.h"

BoneComponent::BoneComponent(GameObject* owner)
	: Component(owner), bone(nullptr)
{
}

BoneComponent::BoneComponent(const BoneComponent& other)
	: Component(other), bone(other.bone)
{
}

BoneComponent& BoneComponent::operator=(const BoneComponent& other)
{
	if (this != &other)
	{
		Component::operator=(other);
		bone = other.bone;
	}
	return *this;
}

BoneComponent::BoneComponent(BoneComponent&& other) noexcept
	: Component(std::move(other)), bone(other.bone)
{
	other.bone = nullptr;
}

BoneComponent& BoneComponent::operator=(BoneComponent&& other) noexcept
{
	if (this != &other)
	{
		Component::operator=(std::move(other));
		bone = other.bone;
		other.bone = nullptr;
	}
	return *this;
}

void BoneComponent::Start()
{

}

void BoneComponent::Update(float deltaTime)
{
	owner->GetTransform()->SetLocalMatrix(bone->GetLocalTransform());
}

void BoneComponent::Destroy()
{

}

void BoneComponent::SetBone(Bone* bone)
{
	this->bone = bone;
}


