#pragma once

#include <vector>
#include <string>
#include "BoundingBox.h"
#include "assimp/scene.h"

#include "types.h"

#define MAX_BONES 100
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

struct BoneInfo2
{
	int id;

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
	unsigned int vBPosID = -1, vBNormalsID = -1, vBColorsID = -1, vBTCoordsID = -1, iBID = -1, vA = -1;
	std::vector<Vertex> vertexData;
	std::vector<unsigned int> indexData;
	std::vector<vec2> vertex_texCoords;
	std::vector<vec3> vertex_normals;
	std::vector<vec3> vertex_colors;
};

class Model
{
public:
	Model() {}
	~Model() {}

	std::string& GetMeshName() { return meshName; }

	ModelData& GetModelData() { return modelData; }

	void SetMeshName(const std::string& meshName) { this->meshName = meshName; }

	void SetModelData(const ModelData& modelData) {
		this->modelData = modelData;
		//calculateBoundingBox(); // Recalcula la bounding box al actualizar los datos
	}

	void SetMaterialIndex(int index) { materialIndex = index; }

	int& GetMaterialIndex() { return materialIndex; }

	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	const BoundingBox& getBoundingBox() const { return m_BoundingBox; }

	bool isAnimated = true;
private:
	std::string meshName;
	ModelData modelData;
	int materialIndex = -1;

	std::map<std::string, BoneInfo2> m_BoneInfoMap;
	int m_BoneCounter = 0;

	BoundingBox m_BoundingBox; // Bounding box de la malla

	//void calculateBoundingBox(); // M�todo privado para calcular la bounding box
};

