#include "SkeletalAnimationComponent.h"
#include <iostream>
#include <MyScriptingEngine/MonoManager.h>
#include <mono/metadata/debug-helpers.h>

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner)
{
    name = "SkeletalAnimation_Component";
}

SkeletalAnimationComponent::SkeletalAnimationComponent(const SkeletalAnimationComponent& other) : Component(other) {
    if (other.animator) {
        animator = std::make_unique<Animator>(*other.animator);
    }
    if (other.animation1) {
        animation1 = std::make_unique<Animation>(*other.animation1);
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
        if (other.animation1) {
            animation1 = std::make_unique<Animation>(*other.animation1);
        } else {
            animation1.reset();
        }
    }
    return *this;
}

SkeletalAnimationComponent::SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept : Component(std::move(other)) {
    animator = std::move(other.animator);
    animation1 = std::move(other.animation1);
}


SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(SkeletalAnimationComponent&& other) noexcept {
    if (this != &other) {
        Component::operator=(std::move(other));
        animator = std::move(other.animator);
        animation1 = std::move(other.animation1);
    }
    return *this;
}

void SkeletalAnimationComponent::Start()
{
	if (animator == nullptr) 
    {
		animator = std::make_unique<Animator>(animation1.get());
	}
    //animator = std::make_unique<Animator>(testAnimation.get());
    animator->PlayAnimation(animation1.get());

}

void SkeletalAnimationComponent::Update(float deltaTime)
{
    if (isPlaying) 
    {
        if (isBlending) 
        {
            animator->TransitionToAnimation(animation1.get(), newAnimation.get(), timeToTransition, deltaTime);
			//animator->BlendTwoAnimations(animations[0].get(), animations[2].get(), blendFactor, deltaTime);
        }
        else 
        {
            animator->UpdateAnimation(deltaTime);
        }
        
    }
}

void SkeletalAnimationComponent::TransitionAnimations(int oldAnim, int newAnim, float timeToTransitionAnim) 
{
    animation1 = std::make_unique<Animation>(*animations[oldAnim].get());
    newAnimation = std::make_unique<Animation>(*animations[newAnim].get());
	timeToTransition = timeToTransitionAnim;
    animator->SetTransitionTime(0);
	isBlending = true;
}

void SkeletalAnimationComponent::Destroy()
{
}

int SkeletalAnimationComponent::GetAnimationIndex() {
    return animationIndex;
}

MonoObject* SkeletalAnimationComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "SkeletalAnimation");
    if (!klass) {
        return nullptr;
    }
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        return nullptr;
    }
    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.SkeletalAnimation:.ctor(uintptr,HawkEngine.GameObject)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    if (!method)
    {
        return nullptr;
    }
    uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
    MonoObject* ownerGo = owner ? owner->GetSharp() : nullptr;
    if (!ownerGo)
    {
        return nullptr;
    }
    void* args[2];
    args[0] = &componentPtr;
    args[1] = ownerGo;
    mono_runtime_invoke(method, monoObject, args, NULL);
    CsharpReference = monoObject;
    return CsharpReference;
}
