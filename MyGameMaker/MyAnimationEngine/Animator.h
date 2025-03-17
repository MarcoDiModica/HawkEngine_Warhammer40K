#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Animation.h"
#include "../MyGameEngine/GameObject.h"

class Animator
{
private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
	std::vector<std::shared_ptr<GameObject>> m_BonesGameObjects;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
	float m_PlaySpeed = 1;
    float transitionTime = 0.0f;

public:
    Animator(Animation* Animation);
	~Animator();

    Animator(const Animator& other);
    Animator& operator=(const Animator& other);

    Animator(Animator&& other) noexcept;
    Animator& operator=(Animator&& other) noexcept;

	void UpdateAnimation(float dt);
	void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    void BlendTwoAnimations(Animation* pBaseAnimation, Animation* pLayeredAnimation, float blendFactor, float deltaTime);
    void CalculateBlendedBoneTransform( Animation* pAnimationBase, const AssimpNodeData* node, Animation* pAnimationLayer, const AssimpNodeData* nodeLayered,
        const float currentTimeBase, const float currentTimeLayered, const glm::mat4& parentTransform, const float blendFactor);

	void TransitionToAnimation(Animation* pOldAnimation, Animation* pNewAnimation, float transitionDuration, float deltaTime);

    float GetCurrentMTime()
    {
		return m_CurrentTime;
    }

	void SetCurrentMTime(float time)
	{
		m_CurrentTime = time;
	}

    void UpdateAnimTime(float time)
    {
		m_CurrentTime = time;
		UpdateAnimation(m_CurrentTime);
    }

	Animation* GetCurrentAnimation()
	{
		return m_CurrentAnimation;
	}

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

    void SetPlaySpeed(float playSpeed) 
    {
		m_PlaySpeed = playSpeed;
    }

    float GetPlaySpeed()
    {
        return m_PlaySpeed;
    }

    void AddBoneGameObject(std::shared_ptr<GameObject> boneGO)
    {
        m_BonesGameObjects.push_back(boneGO);
    }

	void SetTransitionTime(float time)
	{
		transitionTime = time;
	}

};

