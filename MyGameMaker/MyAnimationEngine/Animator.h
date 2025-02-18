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

public:
    Animator(Animation* Animation);
	~Animator();

    Animator(const Animator& other);
    Animator& operator=(const Animator& other);

	void UpdateAnimation(float dt);
	void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }
};

