#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
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
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;

public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	~Bone();

	void Update(float animationTime);
	int GetPositionIndex(float timeStamp);
	int GetRotationIndex(float timeStamp);
	int GetScaleIndex(float timeStamp);

    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }

private:
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float timeStamp);
	glm::mat4 InterpolateRotation(float timeStamp);
	glm::mat4 InterpolateScaling(float timeStamp);

    
};

