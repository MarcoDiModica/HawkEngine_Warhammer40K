#pragma once

#include <string>
#include <memory>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Bone.h"
#include "MyGameEngine/Mesh.h"

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
    float m_Duration = 0;
    int m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
public:
    Animation();
    ~Animation();

    Animation(const Animation& other);
    Animation& operator=(const Animation& other);

    void SetUpAnimation(const std::string& animationPath, Mesh* model);

	Bone* FindBone(const std::string& name);

    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration; }

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }
    
private:
    void ReadMissingBones(const aiAnimation* animation, Mesh& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

