#include "Animator.h"

Animator::Animator(Animation* Animation)
{
    m_CurrentTime = 0.0;
    m_CurrentAnimation = Animation;

    m_FinalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

Animator::~Animator()
{
}

Animator::Animator(const Animator& other)
{
    m_CurrentTime = other.m_CurrentTime;
    m_DeltaTime = other.m_DeltaTime;
    m_CurrentAnimation = other.m_CurrentAnimation;
    m_FinalBoneMatrices = other.m_FinalBoneMatrices;
	m_BonesGameObjects = other.m_BonesGameObjects;
}

Animator& Animator::operator=(const Animator& other)
{
    if (this != &other)
    {
        m_CurrentTime = other.m_CurrentTime;
        m_DeltaTime = other.m_DeltaTime;
        m_CurrentAnimation = other.m_CurrentAnimation;
        m_FinalBoneMatrices = other.m_FinalBoneMatrices;
		m_BonesGameObjects = other.m_BonesGameObjects;
    }
    return *this;
}

Animator::Animator(Animator&& other) noexcept
{
    m_CurrentTime = other.m_CurrentTime;
    m_DeltaTime = other.m_DeltaTime;
    m_CurrentAnimation = other.m_CurrentAnimation;
    m_FinalBoneMatrices = std::move(other.m_FinalBoneMatrices);
	m_BonesGameObjects = std::move(other.m_BonesGameObjects);
    other.m_CurrentAnimation = nullptr;
}

Animator& Animator::operator=(Animator&& other) noexcept
{
    if (this != &other)
    {
        m_CurrentTime = other.m_CurrentTime;
        m_DeltaTime = other.m_DeltaTime;
        m_CurrentAnimation = other.m_CurrentAnimation;
        m_FinalBoneMatrices = std::move(other.m_FinalBoneMatrices);
		m_BonesGameObjects = std::move(other.m_BonesGameObjects);
        other.m_CurrentAnimation = nullptr;
    }
    return *this;
}

void Animator::UpdateAnimation(float dt)
{
    m_DeltaTime = dt;
    if (m_CurrentAnimation)
    {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt * m_PlaySpeed;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* pAnimation)
{
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
		for (int i = 0; i < m_BonesGameObjects.size(); i++)
		{
			if (m_BonesGameObjects[i]->GetName() == nodeName)
			{
				m_BonesGameObjects[i]->GetTransform()->SetLocalMatrix(globalTransformation * offset);
				break;
			}
		}

        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}
