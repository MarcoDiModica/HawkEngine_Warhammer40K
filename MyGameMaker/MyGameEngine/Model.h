#pragma once

#include <vector>
#include <string>
#include "BoundingBox.h"

#include "types.h"

#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 100

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

struct Vertex
{
	vec3 position{};
	vec3 normal{};
	vec2 texCoords{};
	vec3 tangent{};
	vec3 bitangent{};
	int m_BoneIDs[MAX_BONE_INFLUENCE]{};
	float m_Weights[MAX_BONE_INFLUENCE]{};

	Vertex() : position(0.0f), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			m_BoneIDs[i] = -1;
			m_Weights[i] = 0.0f;
		}
	}

	Vertex(vec3 pos) : position(pos), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			m_BoneIDs[i] = -1;
			m_Weights[i] = 0.0f;
		}
	}
};

struct ModelData
{
	unsigned int vBPosID = -1, vBNormalsID = -1, vBColorsID = -1, vBTCoordsID = -1, iBID = -1, vA = -1;
	std::vector<Vertex> vertexData{};
	std::vector<unsigned int> indexData{};
	std::vector<vec2> vertex_texCoords{};
	std::vector<vec3> vertex_normals{};
	std::vector<vec3> vertex_colors{};
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

	const BoundingBox& getBoundingBox() const { return m_BoundingBox; }
private:
	std::string meshName;
	ModelData modelData;
	int materialIndex = -1;

	BoundingBox m_BoundingBox; // Bounding box de la malla

	//void calculateBoundingBox(); // Método privado para calcular la bounding box
};

