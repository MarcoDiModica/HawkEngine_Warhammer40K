#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Animation.h"

class Animator
{
private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
	float m_PlaySpeed = 1;

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

    float GetCurrentMTime()
    {
		return m_CurrentTime;
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

};

