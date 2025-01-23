#pragma once
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"
#include <vector>
#include <string>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
    glm::vec2 uv;
	glm::vec4 boneIDs = glm::vec4(0.0f);
	glm::vec4 boneWeights = glm::vec4(0.0f);
};

struct Bone {
    int id = 0;
	std::string name = "";
	glm::mat4 offset = glm::mat4(1.0f);
    std::vector<Bone> children = {};
};

struct BoneTransformTrack {
    std::vector<float> positionTimeStamps = {};
	std::vector<float> rotationTimeStamps = {};
	std::vector<float> scaleTimeStamps = {};
	std::vector<glm::vec3> positions = {};
	std::vector<glm::quat> rotations = {};
	std::vector<glm::vec3> scales = {};
};

struct Animation {
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

class SkeletalAnimationComponent : public Component {
public:
    SkeletalAnimationComponent(GameObject* owner);

	std::unique_ptr<Component> Clone(GameObject* owner) override;

    ComponentType GetType() const override { return ComponentType::ANIMATION; }

	inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 matrix);
	inline glm::vec3 assimpToGlmVec3(aiVector3D vec);
	inline glm::quat assimpToGlmQuat(aiQuaternion quat);
	unsigned int createVertexArray(std::vector<Vertex>& vertices, std::vector<unsigned int> indices);
	unsigned int createTexture(std::string filepath);
	inline unsigned int createShader(const char* vertexStr, const char* fragmentStr);
	std::shared_ptr<Bone> GetSkeleton() const;
	std::shared_ptr<Animation> GetAnimation() const;
	std::shared_ptr<aiMesh> GetMesh() const;
	bool ReadSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable);
	void LoadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& verticesOutput, std::vector<unsigned int>& indicesOutput, Bone& skeletonOutput, unsigned int& boneCount);
	void LoadAnimation(const aiScene* scene, Animation* animation);
	void GetPose(Animation& animation, Bone& skeleton, float dt, std::vector<glm::mat4>& output, glm::mat4& parentTransform, glm::mat4& globalInverseTransform);
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

private:
	unsigned int diffuseTexture = 0;
	aiMesh* meshes;

	std::vector<Vertex> vertices = {};
	std::vector<unsigned int> indices = {};
	unsigned int boneCount = 0;
	Animation animation;
	unsigned int vao = 0;
	Bone skeleton;


};

