#include "Animation.h"

Animation::Animation()
{
}



Animation::~Animation()
{
}

Animation::Animation(const Animation& other)
{
    m_Duration = other.m_Duration;
    m_TicksPerSecond = other.m_TicksPerSecond;
    m_Bones = other.m_Bones;
    m_RootNode = other.m_RootNode;
    m_BoneInfoMap = other.m_BoneInfoMap;
}

Animation& Animation::operator=(const Animation& other)
{
    if (this != &other)
    {
        m_Duration = other.m_Duration;
        m_TicksPerSecond = other.m_TicksPerSecond;
        m_Bones = other.m_Bones;
        m_RootNode = other.m_RootNode;
        m_BoneInfoMap = other.m_BoneInfoMap;
    }
    return *this;
}

Animation::Animation(Animation&& other) noexcept
{
    m_Duration = other.m_Duration;
    m_TicksPerSecond = other.m_TicksPerSecond;
    m_Bones = std::move(other.m_Bones);
    m_RootNode = std::move(other.m_RootNode);
    m_BoneInfoMap = std::move(other.m_BoneInfoMap);
}

Animation& Animation::operator=(Animation&& other) noexcept
{
    if (this != &other)
    {
        m_Duration = other.m_Duration;
        m_TicksPerSecond = other.m_TicksPerSecond;
        m_Bones = std::move(other.m_Bones);
        m_RootNode = std::move(other.m_RootNode);
        m_BoneInfoMap = std::move(other.m_BoneInfoMap);
    }
    return *this;
}

void Animation::SetUpAnimation(const std::string& animationPath, Mesh* model)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(m_RootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const std::string& name)
{
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, Mesh& model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
    int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        //ERROR HERE
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.push_back(Bone(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
