#include "Animator.h"

Animator::Animator(Animation* Animation)
{
    m_CurrentTime = 0.0;
    m_CurrentAnimation = Animation;

    m_FinalBoneMatrices.reserve(200);

    for (int i = 0; i < 200; i++)
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

void Animator::TransitionToAnimation(Animation* pOldAnimation, Animation* pNewAnimation, float transitionDuration, float deltaTime)
{
   
    transitionTime += deltaTime;

    float blendFactor = transitionTime / transitionDuration;
    if (blendFactor > 1.0f)
    {
        blendFactor = 1.0f;
        m_CurrentAnimation = pNewAnimation;
    }

    BlendTwoAnimations(pOldAnimation, pNewAnimation, blendFactor, deltaTime);
}

void Animator::BlendTwoAnimations(Animation* pBaseAnimation, Animation* pLayeredAnimation, float blendFactor, float deltaTime)
{
    float a = 1.0f;
    float b = pBaseAnimation->GetDuration() / pLayeredAnimation->GetDuration();
    const float animSpeedMultiplierUp = (1.0f - blendFactor) * a + b * blendFactor; 

    a = pLayeredAnimation->GetDuration() / pBaseAnimation->GetDuration();
    b = 1.0f;
    const float animSpeedMultiplierDown = (1.0f - blendFactor) * a + b * blendFactor; 

    static float currentTimeBase = 0.0f;
    currentTimeBase += pBaseAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierUp * m_PlaySpeed;
    currentTimeBase = fmod(currentTimeBase, pBaseAnimation->GetDuration());

    static float currentTimeLayered = 0.0f;
    currentTimeLayered += pLayeredAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierDown * m_PlaySpeed;
    currentTimeLayered = fmod(currentTimeLayered, pLayeredAnimation->GetDuration());

    CalculateBlendedBoneTransform(pBaseAnimation, &pBaseAnimation->GetRootNode(), pLayeredAnimation, &pLayeredAnimation->GetRootNode(), currentTimeBase, currentTimeLayered, glm::mat4(1.0f), blendFactor);
}

void Animator::CalculateBlendedBoneTransform(
    Animation* pAnimationBase, const AssimpNodeData* node,
    Animation* pAnimationLayer, const AssimpNodeData* nodeLayered,
    const float currentTimeBase, const float currentTimeLayered,
    const glm::mat4& parentTransform,
    const float blendFactor)
{
    const std::string& nodeName = node->name;

    glm::mat4 nodeTransform = node->transformation;
    Bone* pBone = pAnimationBase->FindBone(nodeName);
    if (pBone)
    {
        pBone->Update(currentTimeBase);
        nodeTransform = pBone->GetLocalTransform();
    }

    glm::mat4 layeredNodeTransform = nodeLayered->transformation;
    pBone = pAnimationLayer->FindBone(nodeName);
    if (pBone)
    {
        pBone->Update(currentTimeLayered);
        layeredNodeTransform = pBone->GetLocalTransform();
    }

    const glm::quat rot0 = glm::quat_cast(nodeTransform);
    const glm::quat rot1 = glm::quat_cast(layeredNodeTransform);
    const glm::quat finalRot = glm::slerp(rot0, rot1, blendFactor);
    glm::mat4 blendedMat = glm::mat4_cast(finalRot);
    blendedMat[3] = (1.0f - blendFactor) * nodeTransform[3] + layeredNodeTransform[3] * blendFactor;

    glm::mat4 globalTransformation = parentTransform * blendedMat;

    const auto& boneInfoMap = pAnimationBase->GetMBoneInfoMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        const int index = boneInfoMap.at(nodeName).id;
        const glm::mat4& offset = boneInfoMap.at(nodeName).offset;

        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (size_t i = 0; i < node->children.size(); ++i)
        CalculateBlendedBoneTransform(pAnimationBase, &node->children[i], pAnimationLayer, &nodeLayered->children[i], currentTimeBase, currentTimeLayered, globalTransformation, blendFactor);
}