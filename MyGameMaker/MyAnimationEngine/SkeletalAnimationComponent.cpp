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
	if (animator == nullptr) 
    {
		animator = std::make_unique<Animator>(testAnimation.get());
	}
    //animator = std::make_unique<Animator>(testAnimation.get());
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
