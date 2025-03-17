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

    void TransitionAnimations(int oldAnim, int newAnim, float timeToTransition);

    ComponentType GetType() const override {
        return ComponentType::ANIMATION; // Cambia a un tipo específico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    void SetAnimation(Animation* animation) {
        animation1 = std::make_unique<Animation>(*animation);
    }

	void AddAnimation(Animation* animation) {
		animations.push_back(std::make_unique<Animation>(*animation));
	}

	std::vector<std::unique_ptr<Animation>>& GetAnimations() {
		return animations;
	}

    Animation* GetAnimation() const {
        return animation1.get();
    }

    void SetAnimator(Animator* animatorr) {
        animator = std::make_unique<Animator>(*animatorr);
    }

    Animator* GetAnimator() const {
        return animator.get();
    }

	float GetAnimationDuration() const {
		return animation1->GetDuration();
	}

	float GetAnimationTime() const {
		return animator->GetCurrentMTime();
	}

	void SetAnimationPlayState(bool play) 
    {
		isPlaying = play;
	}

	bool GetAnimationPlayState() const 
    {
		return isPlaying;
	}

    int GetAnimationIndex();

	void SetAnimationIndex(int index) {
		animationIndex = index;
	}

	void PlayIndexAnimation(int index) 
    {
		if (index < 0 || index >= animations.size())
		{
			return;
		}
        SetAnimationIndex(index);
		SetAnimation(animations[index].get());
		animator->PlayAnimation(animation1.get());
	}

    void SetAnimationSpeed(float speed) 
    {
		animator->SetPlaySpeed(speed);
    }

    void SetNewAnimation(Animation* animation) {
        newAnimation = std::make_unique<Animation>(*animation);
    }

	float GetAnimationSpeed()
	{
		return animator->GetPlaySpeed();
	}


    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;


    float blendFactor = 0.0f;
    bool isBlending = false;
private:
    std::unique_ptr<Animator> animator;
    std::unique_ptr<Animation> animation1;
    std::unique_ptr<Animation> newAnimation;
    //vector of unitque ptr of animations
	std::vector<std::unique_ptr<Animation>> animations;
    int animationIndex = 0;
	bool isPlaying = true;
	float timeToTransition = 0.0f;



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

