#pragma once
#ifdef min
#undef min
#endif
#include <vector>
#include <string>
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

	int m_BoneIDs[MAX_BONE_INFLUENCE];

	float m_Weights[MAX_BONE_INFLUENCE];
};

struct ModelData
{
	unsigned int vBPosID = -1;
	unsigned int vBNormalsID = -1;
	unsigned int vBColorsID = -1;
	unsigned int vBTCoordsID = -1;
	unsigned int vBTangentsID = -1;
	unsigned int vBBitangentsID = -1;
	unsigned int iBID = -1;
	unsigned int vA = -1;

	std::vector<Vertex> vertexData;
	std::vector<unsigned int> indexData;
	std::vector<vec2> vertex_texCoords;
	std::vector<vec3> vertex_normals;
	std::vector<vec3> vertex_colors;
	std::vector<vec3> vertex_tangents;  
	std::vector<vec3> vertex_bitangents;

	// Constructor por defecto
	ModelData() = default;

	// Constructor de copia
	ModelData(const ModelData& other) = default;

	// Constructor de movimiento
	ModelData(ModelData&& other) noexcept = default;

	// Operador de asignación
	ModelData& operator=(const ModelData& other) = default;

	// Operador de asignación por movimiento
	ModelData& operator=(ModelData&& other) noexcept = default;
};

class Model
{
public:
	Model() : m_ID(GenerateUniqueID()) {}
	~Model() {}

	std::string& GetMeshName() { return meshName; }
	ModelData& GetModelData() { return modelData; }

	void SetMeshName(const std::string& meshName) { this->meshName = meshName; }
	void SetModelData(const ModelData& modelData) {
		this->modelData = modelData;
	}

	void SetMaterialIndex(int index) { materialIndex = index; }
	int& GetMaterialIndex() { return materialIndex; }

	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	const BoundingBox& getBoundingBox() const { return m_BoundingBox; }
	void SetBoundingBox(const BoundingBox& bbox) { m_BoundingBox = bbox; }

	uint32_t GetID() const { return m_ID; }
	void SetID(uint32_t id) { m_ID = id; }

	static uint32_t GenerateUniqueID() { return s_NextID++; }

	bool isAnimated = true;
private:
	std::string meshName;
	ModelData modelData;
	int materialIndex = -1;

	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	BoundingBox m_BoundingBox;

	uint32_t m_ID = 0;
	static uint32_t s_NextID;

	//void calculateBoundingBox(); // Mï¿½todo privado para calcular la bounding box
};

class ModelRegistry
{
public:
	static void RegisterModel(std::shared_ptr<Model> model);
	static void UnregisterModel(uint32_t id);
	static std::shared_ptr<Model> GetModelByID(uint32_t id);

private:
	static std::unordered_map<uint32_t, std::weak_ptr<Model>> s_Models;
};