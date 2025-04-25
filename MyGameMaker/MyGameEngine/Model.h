#pragma once
#ifdef min
#undef min
#endif
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "BoundingBox.h"
#include "assimp/scene.h"
#include "types.h"

#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

enum class Shapes
{
	EMPTY,
	CUBE,
	PLANE,
	SPHERE,
	CYLINDER,
	CONE,
	TORUS,
	MESH
};

struct BoneInfo
{
	int id;
	std::string name;
	std::string parentName;
	glm::mat4 offset;
};

struct Vertex
{
	vec3 position;
	vec3 normal;   
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];

	Vertex() :
		position(0.0f),
		normal(0.0f),
		texCoord(0.0f),
		tangent(0.0f),
		bitangent(0.0f)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			m_BoneIDs[i] = -1;
			m_Weights[i] = 0.0f;
		}
	}

	explicit Vertex(const vec3& pos) :
		position(pos),
		normal(0.0f),
		texCoord(0.0f),
		tangent(0.0f),
		bitangent(0.0f)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			m_BoneIDs[i] = -1;
			m_Weights[i] = 0.0f;
		}
	}
};

struct ModelData
{
	unsigned int vBPosID = 0;
	unsigned int vBNormalsID = 0;
	unsigned int vBColorsID = 0;
	unsigned int vBTCoordsID = 0;
	unsigned int vBTangentsID = 0;
	unsigned int vBBitangentsID = 0;
	unsigned int vBBoneIDsID = 0;
	unsigned int vBBoneWeightsID = 0;
	unsigned int iBID = 0;
	unsigned int vA = 0;

	std::vector<Vertex> vertexData;
	std::vector<unsigned int> indexData;
	std::vector<glm::vec2> vertex_texCoords;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec3> vertex_colors;
	std::vector<glm::vec3> vertex_tangents;
	std::vector<glm::vec3> vertex_bitangents;

	ModelData() = default;
	ModelData(const ModelData& other) = default;
	ModelData(ModelData&& other) noexcept = default;
	ModelData& operator=(const ModelData& other) = default;
	ModelData& operator=(ModelData&& other) noexcept = default;

	/*void clearGLResources() {
		if (vA != 0) { glDeleteVertexArrays(1, &vA); vA = 0; }
		if (vBPosID != 0) { glDeleteBuffers(1, &vBPosID); vBPosID = 0; }
		if (vBNormalsID != 0) { glDeleteBuffers(1, &vBNormalsID); vBNormalsID = 0; }
		if (vBColorsID != 0) { glDeleteBuffers(1, &vBColorsID); vBColorsID = 0; }
		if (vBTCoordsID != 0) { glDeleteBuffers(1, &vBTCoordsID); vBTCoordsID = 0; }
		if (vBTangentsID != 0) { glDeleteBuffers(1, &vBTangentsID); vBTangentsID = 0; }
		if (vBBitangentsID != 0) { glDeleteBuffers(1, &vBBitangentsID); vBBitangentsID = 0; }
		if (vBBoneIDsID != 0) { glDeleteBuffers(1, &vBBoneIDsID); vBBoneIDsID = 0; }
		if (vBBoneWeightsID != 0) { glDeleteBuffers(1, &vBBoneWeightsID); vBBoneWeightsID = 0; }
		if (iBID != 0) { glDeleteBuffers(1, &iBID); iBID = 0; }
	}*/

	~ModelData() {
	}
};

class Model
{
public:
	Model();
	~Model();

	std::string& GetMeshName() { return meshName; }
	const std::string& GetMeshName() const { return meshName; }
	ModelData& GetModelData() { return modelData; }
	const ModelData& GetModelData() const { return modelData; }

	void SetMeshName(const std::string& meshName) { this->meshName = meshName; }
	void SetModelData(const ModelData& modelData) { this->modelData = modelData; }

	void SetMaterialIndex(int index) { materialIndex = index; }
	int GetMaterialIndex() const { return materialIndex; }

	std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
	const std::map<std::string, BoneInfo>& GetBoneInfoMap() const { return m_BoneInfoMap; }

	int& GetBoneCount() { return m_BoneCounter; }
	int GetBoneCount() const { return m_BoneCounter; }

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	const BoundingBox& getBoundingBox() const { return m_BoundingBox; }
	void SetBoundingBox(const BoundingBox& bbox) { m_BoundingBox = bbox; }

	uint32_t GetID() const { return m_ID; }
	void SetID(uint32_t id) { m_ID = id; }
	void GenerateUniqueID();

	/*void ReleaseResources() {
		modelData.clearGLResources();
	}*/

	bool isAnimated = false;

private:
	std::string meshName;
	ModelData modelData;
	int materialIndex = -1;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;
	BoundingBox m_BoundingBox;
	uint32_t m_ID = 0;

	static uint32_t nextModelID;
};