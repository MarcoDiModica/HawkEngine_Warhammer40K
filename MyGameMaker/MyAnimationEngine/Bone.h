#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#ifdef min
#undef min
#endif
#include <string>
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyGameEngine/AssimpGLMHelpers.h"

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone
{
private:
	friend class SkeletalAnimationComponent;
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
	std::string m_ParentName;
    int m_ID;

public:
    Bone(const std::string& name, const std::string& parentName, int ID, const aiNodeAnim* channel);
	~Bone();

	void Update(float animationTime);
	int GetPositionIndex(float timeStamp);
	int GetRotationIndex(float timeStamp);
	int GetScaleIndex(float timeStamp);

    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
	std::string GetParentName() const { return m_ParentName; }
    int GetBoneID() { return m_ID; }

    void DrawTriangle();

    void SetLocalTransform(const glm::mat4& localTransform) { m_LocalTransform = localTransform; }

private:
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float timeStamp);
	glm::mat4 InterpolateRotation(float timeStamp);
	glm::mat4 InterpolateScaling(float timeStamp);

    
};

