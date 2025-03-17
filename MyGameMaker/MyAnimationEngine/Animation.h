#pragma once
#ifdef min
#undef min
#endif
#include <string>
#include <memory>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Bone.h"
#include "MyGameEngine/Model.h"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
private:
	friend class SkeletalAnimationComponent;

    float m_Duration = 0;
    int m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::vector<std::string> m_Animations;
    std::string name;
public:
    Animation();
    ~Animation();

    Animation(const Animation& other);
    Animation& operator=(const Animation& other);

    Animation(Animation&& other) noexcept;
    Animation& operator=(Animation&& other) noexcept;

    void SetUpAnimation(aiScene* scene, Model* model, int index);

	Bone* FindBone(const std::string& name);

    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline int& GetTicksPerSecond2() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration; }

    inline float& GetDuration2() { return m_Duration; }

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }
    
    std::string GetName() { return name; }

	std::vector<std::string>& GetAnimations()
	{
		return m_Animations;
	}

	 auto GetMBoneInfoMap() { return m_BoneInfoMap; }

private:
    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

