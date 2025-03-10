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
	m_Animations = other.m_Animations;
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
        m_Animations = other.m_Animations;
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
	m_Animations = std::move(other.m_Animations);
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
		m_Animations = std::move(other.m_Animations);
    }
    return *this;
}

void Animation::SetUpAnimation(const std::string& animationPath, Model* model, int index)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[index];
    m_Duration = animation->mDuration;
    for (int i = 0; i < animation->mNumChannels; i++)
    {
        m_Animations.push_back(animation->mChannels[i]->mNodeName.C_Str());
    }
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

bool containsWord(const std::string& str, const std::string& word) {
    return str.find(word) != std::string::npos;
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
    int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        //
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
			boneInfoMap[boneName].name = boneName;
            
			boneInfoMap[boneName].parentName = model.GetBoneInfoMap()[channel->mNodeName.data].parentName;
            boneCount++;
        }

        boneInfoMap[boneName].name = boneName;
        if (containsWord(model.GetBoneInfoMap()[channel->mNodeName.data].parentName, "Armature"))
        {
            boneInfoMap[boneName].parentName = "";
        }
        else
        {
            boneInfoMap[boneName].parentName = model.GetBoneInfoMap()[channel->mNodeName.data].parentName;
        }

		m_Bones.push_back(Bone(channel->mNodeName.data, model.GetBoneInfoMap()[channel->mNodeName.data].parentName, model.GetBoneInfoMap()[channel->mNodeName.data].id, channel));
            
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
