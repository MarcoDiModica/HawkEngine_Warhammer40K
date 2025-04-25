#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "../MyGameEditor/Log.h"

static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;

Mesh::Mesh() : aabbMin(glm::vec3(0.0f)), aabbMax(glm::vec3(0.0f))
{
	_boundingBox = BoundingBox();
}

Mesh::~Mesh()
{
	releaseOpenGLResources();
}

void Mesh::releaseOpenGLResources()
{
	/*if (model) {
		model->ReleaseResources();
	}*/
}

void Mesh::drawBoundingBox(const BoundingBox& bbox) {
	glLineWidth(2.0);
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
	drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
	drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
	drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());
}

void Mesh::drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
	glBegin(GL_LINE_LOOP);
	glVertex3(v0);
	glVertex3(v1);
	glVertex3(v2);
	glVertex3(v3);
	glEnd();
}

void Mesh::CalculateNormals() {
	if (!model || model->GetModelData().vertexData.empty() || model->GetModelData().indexData.empty()) {
		LOG(LogType::LOG_ERROR, "Cannot calculate normals: Model data is empty");
		return;
	}

	auto& modelData = model->GetModelData();
	const auto& vertexData = modelData.vertexData;
	const auto& indexData = modelData.indexData;
	auto& normalData = modelData.vertex_normals;

	normalData.clear();
	normalData.resize(vertexData.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < indexData.size(); i += 3) {
		if (i + 2 >= indexData.size() ||
			indexData[i] >= vertexData.size() ||
			indexData[i + 1] >= vertexData.size() ||
			indexData[i + 2] >= vertexData.size()) {
			continue;
		}

		glm::vec3 v0 = vertexData[indexData[i]].position;
		glm::vec3 v1 = vertexData[indexData[i + 1]].position;
		glm::vec3 v2 = vertexData[indexData[i + 2]].position;

		glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
		float length = glm::length(normal);

		if (length < 0.0001f) {
			continue;
		}

		normal /= length; 

		normalData[indexData[i]] += normal;
		normalData[indexData[i + 1]] += normal;
		normalData[indexData[i + 2]] += normal;
	}

	for (auto& normal : normalData) {
		float length = glm::length(normal);
		if (length > 0.0001f) {
			normal /= length;
		}
		else {
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}

	for (size_t i = 0; i < vertexData.size(); i++) {
		if (i < normalData.size()) {
			modelData.vertexData[i].normal = normalData[i];
		}
	}

	if (modelData.vBNormalsID != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBNormalsID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, normalData.size() * sizeof(glm::vec3), normalData.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Mesh::LoadBones()
{
	if (!model || model->GetModelData().vertexData.empty()) {
		LOG(LogType::LOG_ERROR, "Cannot load bones: Model data is empty");
		return;
	}

	bool hasBoneData = false;
	for (const auto& vertex : model->GetModelData().vertexData) {
		if (vertex.m_BoneIDs[0] != -1) {
			hasBoneData = true;
			break;
		}
	}

	if (!hasBoneData) {
		return; 
	}

	model->isAnimated = true;

	if (model->GetModelData().vA != 0) {
		glBindVertexArray(model->GetModelData().vA);

		if (model->GetModelData().vBBoneIDsID == 0) {
			glGenBuffers(1, &model->GetModelData().vBBoneIDsID);
		}

		if (model->GetModelData().vBBoneWeightsID == 0) {
			glGenBuffers(1, &model->GetModelData().vBBoneWeightsID);
		}

		std::vector<glm::ivec4> boneIDs;
		std::vector<glm::vec4> weights;

		boneIDs.reserve(model->GetModelData().vertexData.size());
		weights.reserve(model->GetModelData().vertexData.size());

		for (const auto& vertex : model->GetModelData().vertexData) {
			glm::ivec4 ids;
			glm::vec4 vertexWeights;

			for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
				ids[i] = vertex.m_BoneIDs[i];
				vertexWeights[i] = vertex.m_Weights[i];
			}

			boneIDs.push_back(ids);
			weights.push_back(vertexWeights);
		}

		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBBoneIDsID);
		glBufferData(GL_ARRAY_BUFFER, boneIDs.size() * sizeof(glm::ivec4), boneIDs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTR_BONE_IDS);
		glVertexAttribIPointer(ATTR_BONE_IDS, 4, GL_INT, sizeof(glm::ivec4), (const void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBBoneWeightsID);
		glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec4), weights.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTR_BONE_WEIGHTS);
		glVertexAttribPointer(ATTR_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (const void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Mesh::CalculateTangents() {
	if (!model || model->GetModelData().vertexData.empty() || model->GetModelData().indexData.empty()) {
		LOG(LogType::LOG_ERROR, "Cannot calculate tangents: Model data is empty");
		return;
	}

	auto& modelData = model->GetModelData();
	const auto& vertexData = modelData.vertexData;
	const auto& indexData = modelData.indexData;
	const auto& texCoords = modelData.vertex_texCoords;
	auto& tangentData = modelData.vertex_tangents;
	auto& bitangentData = modelData.vertex_bitangents;

	if (modelData.vertex_normals.empty()) {
		CalculateNormals();
	}

	if (texCoords.empty() && vertexData.size() > 0) {
		LOG(LogType::LOG_WARNING, "Calculating tangents without texture coordinates");
	}

	tangentData.clear();
	bitangentData.clear();
	tangentData.resize(vertexData.size(), glm::vec3(0.0f));
	bitangentData.resize(vertexData.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < indexData.size(); i += 3) {
		if (i + 2 >= indexData.size() ||
			indexData[i] >= vertexData.size() ||
			indexData[i + 1] >= vertexData.size() ||
			indexData[i + 2] >= vertexData.size()) {
			continue;
		}

		unsigned int idx0 = indexData[i];
		unsigned int idx1 = indexData[i + 1];
		unsigned int idx2 = indexData[i + 2];

		glm::vec3 v0 = vertexData[idx0].position;
		glm::vec3 v1 = vertexData[idx1].position;
		glm::vec3 v2 = vertexData[idx2].position;

		glm::vec2 uv0 = (idx0 < texCoords.size()) ? texCoords[idx0] : glm::vec2(0.0f, 0.0f);
		glm::vec2 uv1 = (idx1 < texCoords.size()) ? texCoords[idx1] : glm::vec2(1.0f, 0.0f);
		glm::vec2 uv2 = (idx2 < texCoords.size()) ? texCoords[idx2] : glm::vec2(0.0f, 1.0f);

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float determinant = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		if (std::abs(determinant) < 0.0001f) {
			glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
			glm::vec3 tangent;

			if (std::abs(normal.x) < std::abs(normal.y) && std::abs(normal.x) < std::abs(normal.z)) {
				tangent = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (std::abs(normal.y) < std::abs(normal.z)) {
				tangent = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else {
				tangent = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
			}

			tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));
			glm::vec3 bitangent = glm::cross(normal, tangent);

			tangentData[idx0] += tangent;
			tangentData[idx1] += tangent;
			tangentData[idx2] += tangent;

			bitangentData[idx0] += bitangent;
			bitangentData[idx1] += bitangent;
			bitangentData[idx2] += bitangent;

			continue;
		}

		float f = 1.0f / determinant;

		glm::vec3 tangent, bitangent;

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		tangentData[idx0] += tangent;
		tangentData[idx1] += tangent;
		tangentData[idx2] += tangent;

		bitangentData[idx0] += bitangent;
		bitangentData[idx1] += bitangent;
		bitangentData[idx2] += bitangent;
	}

	const auto& normalData = modelData.vertex_normals;

	for (size_t i = 0; i < vertexData.size(); i++) {
		glm::vec3 normal = (i < normalData.size()) ? normalData[i] : glm::vec3(0.0f, 1.0f, 0.0f);

		if (glm::length(tangentData[i]) < 0.0001f) {
			if (std::abs(normal.x) < 0.707f) {
				tangentData[i] = glm::normalize(glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
			}
			else {
				tangentData[i] = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
			}
		}
		else {
			tangentData[i] = glm::normalize(tangentData[i]);
		}

		tangentData[i] = glm::normalize(tangentData[i] - normal * glm::dot(normal, tangentData[i]));

		bitangentData[i] = glm::normalize(glm::cross(normal, tangentData[i]));

		if (i < modelData.vertexData.size()) {
			modelData.vertexData[i].tangent = tangentData[i];
			modelData.vertexData[i].bitangent = bitangentData[i];
		}
	}

	if (modelData.vA != 0) {
		glBindVertexArray(modelData.vA);

		if (modelData.vBTangentsID == 0) {
			glGenBuffers(1, &modelData.vBTangentsID);
		}
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBTangentsID);
		glBufferData(GL_ARRAY_BUFFER, tangentData.size() * sizeof(glm::vec3), tangentData.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTR_TANGENT);
		glVertexAttribPointer(ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		if (modelData.vBBitangentsID == 0) {
			glGenBuffers(1, &modelData.vBBitangentsID);
		}
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBBitangentsID);
		glBufferData(GL_ARRAY_BUFFER, bitangentData.size() * sizeof(glm::vec3), bitangentData.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTR_BITANGENT);
		glVertexAttribPointer(ATTR_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Mesh::SaveBinary(const std::string& filename) const
{
	if (!model) {
		LOG(LogType::LOG_ERROR, "Error: Cannot save binary mesh - Model is null");
		return;
	}

	std::string fullPath = "Library/Mesh/";
	if (!std::filesystem::exists(fullPath)) {
		try {
			std::filesystem::create_directories(fullPath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			LOG(LogType::LOG_ERROR, "Error creating directory for mesh: %s - %s", fullPath.c_str(), e.what());
			return;
		}
	}

	fullPath += filename + ".mesh";

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		LOG(LogType::LOG_ERROR, "Error opening file for writing: %s", fullPath.c_str());
		return;
	}

	try {
		// Write mesh name
		uint32_t nameLength = static_cast<uint32_t>(model->GetMeshName().length());
		fout.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		fout.write(model->GetMeshName().c_str(), nameLength);

		// Write material index
		int32_t materialIndex = model->GetMaterialIndex();
		fout.write(reinterpret_cast<const char*>(&materialIndex), sizeof(materialIndex));

		// Write vertices
		const auto& vertices = model->GetModelData().vertexData;
		uint32_t numVertices = static_cast<uint32_t>(vertices.size());
		fout.write(reinterpret_cast<const char*>(&numVertices), sizeof(numVertices));
		fout.write(reinterpret_cast<const char*>(vertices.data()), numVertices * sizeof(Vertex));

		// Write indices
		const auto& indices = model->GetModelData().indexData;
		uint32_t numIndices = static_cast<uint32_t>(indices.size());
		fout.write(reinterpret_cast<const char*>(&numIndices), sizeof(numIndices));
		fout.write(reinterpret_cast<const char*>(indices.data()), numIndices * sizeof(unsigned int));

		// Write texture coordinates
		const auto& texCoords = model->GetModelData().vertex_texCoords;
		uint32_t numTexCoords = static_cast<uint32_t>(texCoords.size());
		fout.write(reinterpret_cast<const char*>(&numTexCoords), sizeof(numTexCoords));
		if (numTexCoords > 0) {
			fout.write(reinterpret_cast<const char*>(texCoords.data()), numTexCoords * sizeof(vec2));
		}

		// Write normals
		const auto& normals = model->GetModelData().vertex_normals;
		uint32_t numNormals = static_cast<uint32_t>(normals.size());
		fout.write(reinterpret_cast<const char*>(&numNormals), sizeof(numNormals));
		if (numNormals > 0) {
			fout.write(reinterpret_cast<const char*>(normals.data()), numNormals * sizeof(vec3));
		}

		// Write colors
		const auto& colors = model->GetModelData().vertex_colors;
		uint32_t numColors = static_cast<uint32_t>(colors.size());
		fout.write(reinterpret_cast<const char*>(&numColors), sizeof(numColors));
		if (numColors > 0) {
			fout.write(reinterpret_cast<const char*>(colors.data()), numColors * sizeof(vec3));
		}

		// Write tangents
		const auto& tangents = model->GetModelData().vertex_tangents;
		uint32_t numTangents = static_cast<uint32_t>(tangents.size());
		fout.write(reinterpret_cast<const char*>(&numTangents), sizeof(numTangents));
		if (numTangents > 0) {
			fout.write(reinterpret_cast<const char*>(tangents.data()), numTangents * sizeof(vec3));
		}

		// Write bitangents
		const auto& bitangents = model->GetModelData().vertex_bitangents;
		uint32_t numBitangents = static_cast<uint32_t>(bitangents.size());
		fout.write(reinterpret_cast<const char*>(&numBitangents), sizeof(numBitangents));
		if (numBitangents > 0) {
			fout.write(reinterpret_cast<const char*>(bitangents.data()), numBitangents * sizeof(vec3));
		}

		// Write bounding box
		fout.write(reinterpret_cast<const char*>(&_boundingBox.min), sizeof(glm::dvec3));
		fout.write(reinterpret_cast<const char*>(&_boundingBox.max), sizeof(glm::dvec3));

		// Write model ID
		uint32_t modelID = model->GetID();
		fout.write(reinterpret_cast<const char*>(&modelID), sizeof(modelID));

		// Write isAnimated flag
		bool isAnimated = model->isAnimated;
		fout.write(reinterpret_cast<const char*>(&isAnimated), sizeof(bool));

		LOG(LogType::LOG_INFO, "Mesh saved successfully: %s", fullPath.c_str());
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error writing mesh to file: %s - %s", fullPath.c_str(), e.what());
	}
}

std::shared_ptr<Mesh> Mesh::LoadBinary(std::string& filename)
{
	std::string fullPath = "Library/Mesh/" + filename + ".mesh";

	// Check cache first
	auto it = meshCache.find(fullPath);
	if (it != meshCache.end()) {
		LOG(LogType::LOG_INFO, "Returning cached mesh: %s", fullPath.c_str());
		return it->second;
	}

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		LOG(LogType::LOG_ERROR, "Error opening mesh file: %s", fullPath.c_str());
		return nullptr;
	}

	auto mesh = std::make_shared<Mesh>();
	mesh->setModel(std::make_shared<Model>());
	auto& modelData = mesh->model->GetModelData();

	try {
		// Read mesh name
		uint32_t nameLength;
		fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		std::string meshName(nameLength, '\0');
		fin.read(&meshName[0], nameLength);
		mesh->model->SetMeshName(meshName);

		// Read material index
		int32_t materialIndex;
		fin.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));
		mesh->model->SetMaterialIndex(materialIndex);

		// Read vertices
		uint32_t numVertices;
		fin.read(reinterpret_cast<char*>(&numVertices), sizeof(numVertices));
		modelData.vertexData.resize(numVertices);
		fin.read(reinterpret_cast<char*>(modelData.vertexData.data()), numVertices * sizeof(Vertex));

		// Read indices
		uint32_t numIndices;
		fin.read(reinterpret_cast<char*>(&numIndices), sizeof(numIndices));
		modelData.indexData.resize(numIndices);
		fin.read(reinterpret_cast<char*>(modelData.indexData.data()), numIndices * sizeof(unsigned int));

		// Read texture coordinates
		uint32_t numTexCoords;
		fin.read(reinterpret_cast<char*>(&numTexCoords), sizeof(numTexCoords));
		if (numTexCoords > 0) {
			modelData.vertex_texCoords.resize(numTexCoords);
			fin.read(reinterpret_cast<char*>(modelData.vertex_texCoords.data()), numTexCoords * sizeof(vec2));
		}

		// Read normals
		uint32_t numNormals;
		fin.read(reinterpret_cast<char*>(&numNormals), sizeof(numNormals));
		if (numNormals > 0) {
			modelData.vertex_normals.resize(numNormals);
			fin.read(reinterpret_cast<char*>(modelData.vertex_normals.data()), numNormals * sizeof(vec3));
		}

		// Read colors
		uint32_t numColors;
		fin.read(reinterpret_cast<char*>(&numColors), sizeof(numColors));
		if (numColors > 0) {
			modelData.vertex_colors.resize(numColors);
			fin.read(reinterpret_cast<char*>(modelData.vertex_colors.data()), numColors * sizeof(vec3));
		}

		// Read tangents
		uint32_t numTangents;
		fin.read(reinterpret_cast<char*>(&numTangents), sizeof(numTangents));
		if (numTangents > 0) {
			modelData.vertex_tangents.resize(numTangents);
			fin.read(reinterpret_cast<char*>(modelData.vertex_tangents.data()), numTangents * sizeof(vec3));
		}

		// Read bitangents
		uint32_t numBitangents;
		fin.read(reinterpret_cast<char*>(&numBitangents), sizeof(numBitangents));
		if (numBitangents > 0) {
			modelData.vertex_bitangents.resize(numBitangents);
			fin.read(reinterpret_cast<char*>(modelData.vertex_bitangents.data()), numBitangents * sizeof(vec3));
		}

		// Read bounding box
		fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.min), sizeof(glm::dvec3));
		fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.max), sizeof(glm::dvec3));

		// Read model ID
		uint32_t modelID;
		fin.read(reinterpret_cast<char*>(&modelID), sizeof(modelID));
		mesh->model->SetID(modelID);

		// Read isAnimated flag (if present in the file)
		if (fin.peek() != EOF) {
			bool isAnimated;
			fin.read(reinterpret_cast<char*>(&isAnimated), sizeof(bool));
			mesh->model->isAnimated = isAnimated;
		}

		// Load the mesh to GPU
		mesh->loadToOpenGL();

		// Cache the mesh
		meshCache[fullPath] = mesh;
		mesh->nameM = filename;
		mesh->filePath = filename;

		LOG(LogType::LOG_INFO, "Mesh loaded successfully: %s", fullPath.c_str());
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error reading mesh from file: %s - %s", fullPath.c_str(), e.what());
		return nullptr;
	}

	return mesh;
}

void Mesh::loadToOpenGL()
{
	if (!model) {
		LOG(LogType::LOG_ERROR, "Cannot load mesh to OpenGL: Model is null");
		return;
	}

	ModelData& modelData = model->GetModelData();

	// Clean up existing resources if any
	if (modelData.vA != 0) {
		glDeleteVertexArrays(1, &modelData.vA);
	}

	// Create and bind vertex array object
	glGenVertexArrays(1, &modelData.vA);
	glBindVertexArray(modelData.vA);

	// Extract positions from vertex data
	std::vector<vec3> positions;
	positions.reserve(modelData.vertexData.size());
	for (const auto& vertex : modelData.vertexData) {
		positions.push_back(vertex.position);
	}

	// Create and bind position buffer
	glGenBuffers(1, &modelData.vBPosID);
	glBindBuffer(GL_ARRAY_BUFFER, modelData.vBPosID);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);

	// Position attribute (location 0)
	glEnableVertexAttribArray(ATTR_POSITION);
	glVertexAttribPointer(ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const void*)0);

	// Texture coordinates buffer (if available)
	if (!modelData.vertex_texCoords.empty()) {
		glGenBuffers(1, &modelData.vBTCoordsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBTCoordsID);
		glBufferData(GL_ARRAY_BUFFER, modelData.vertex_texCoords.size() * sizeof(vec2),
			modelData.vertex_texCoords.data(), GL_STATIC_DRAW);

		// Texture coordinates attribute (location 1)
		glEnableVertexAttribArray(ATTR_TEXCOORD);
		glVertexAttribPointer(ATTR_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const void*)0);
	}

	// Normals buffer (if available)
	if (!modelData.vertex_normals.empty()) {
		glGenBuffers(1, &modelData.vBNormalsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBNormalsID);
		glBufferData(GL_ARRAY_BUFFER, modelData.vertex_normals.size() * sizeof(vec3),
			modelData.vertex_normals.data(), GL_STATIC_DRAW);

		// Normals attribute (location 2)
		glEnableVertexAttribArray(ATTR_NORMAL);
		glVertexAttribPointer(ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	// Tangents buffer (if available)
	if (!modelData.vertex_tangents.empty()) {
		glGenBuffers(1, &modelData.vBTangentsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBTangentsID);
		glBufferData(GL_ARRAY_BUFFER, modelData.vertex_tangents.size() * sizeof(vec3),
			modelData.vertex_tangents.data(), GL_STATIC_DRAW);

		// Tangents attribute (location 3)
		glEnableVertexAttribArray(ATTR_TANGENT);
		glVertexAttribPointer(ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	// Bitangents buffer (if available)
	if (!modelData.vertex_bitangents.empty()) {
		glGenBuffers(1, &modelData.vBBitangentsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBBitangentsID);
		glBufferData(GL_ARRAY_BUFFER, modelData.vertex_bitangents.size() * sizeof(vec3),
			modelData.vertex_bitangents.data(), GL_STATIC_DRAW);

		// Bitangents attribute (location 4)
		glEnableVertexAttribArray(ATTR_BITANGENT);
		glVertexAttribPointer(ATTR_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	// Colors buffer (if available)
	if (!modelData.vertex_colors.empty()) {
		glGenBuffers(1, &modelData.vBColorsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBColorsID);
		glBufferData(GL_ARRAY_BUFFER, modelData.vertex_colors.size() * sizeof(vec3),
			modelData.vertex_colors.data(), GL_STATIC_DRAW);

		// Colors attribute (location 7)
		glEnableVertexAttribArray(ATTR_COLOR);
		glVertexAttribPointer(ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	// Bone data (if this mesh is animated)
	bool hasBoneData = false;
	for (const auto& vertex : modelData.vertexData) {
		if (vertex.m_BoneIDs[0] != -1) {
			hasBoneData = true;
			break;
		}
	}

	if (hasBoneData) {
		model->isAnimated = true;

		// Extract bone IDs and weights
		std::vector<glm::ivec4> boneIDs;
		std::vector<glm::vec4> weights;

		boneIDs.reserve(modelData.vertexData.size());
		weights.reserve(modelData.vertexData.size());

		for (const auto& vertex : modelData.vertexData) {
			glm::ivec4 ids;
			glm::vec4 vertexWeights;

			for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
				ids[i] = vertex.m_BoneIDs[i];
				vertexWeights[i] = vertex.m_Weights[i];
			}

			boneIDs.push_back(ids);
			weights.push_back(vertexWeights);
		}

		// Bone IDs buffer
		glGenBuffers(1, &modelData.vBBoneIDsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBBoneIDsID);
		glBufferData(GL_ARRAY_BUFFER, boneIDs.size() * sizeof(glm::ivec4), boneIDs.data(), GL_STATIC_DRAW);

		// Bone IDs attribute (location 5)
		glEnableVertexAttribArray(ATTR_BONE_IDS);
		glVertexAttribIPointer(ATTR_BONE_IDS, 4, GL_INT, sizeof(glm::ivec4), (const void*)0);

		// Bone weights buffer
		glGenBuffers(1, &modelData.vBBoneWeightsID);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vBBoneWeightsID);
		glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec4), weights.data(), GL_STATIC_DRAW);

		// Bone weights attribute (location 6)
		glEnableVertexAttribArray(ATTR_BONE_WEIGHTS);
		glVertexAttribPointer(ATTR_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (const void*)0);
	}

	// Generate and bind element buffer for indices
	glGenBuffers(1, &modelData.iBID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelData.iBID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData.indexData.size() * sizeof(unsigned int),
		modelData.indexData.data(), GL_STATIC_DRAW);

	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate a unique ID for this model if it doesn't have one
	if (model->GetID() == 0) {
		model->GenerateUniqueID();
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "OpenGL error during mesh loading: 0x%X", error);
	}
	else {
		LOG(LogType::LOG_INFO, "Mesh loaded to OpenGL successfully: VAO=%u, Vertices=%zu, Indices=%zu",
			modelData.vA, modelData.vertexData.size(), modelData.indexData.size());
	}
}

bool Mesh::validate(std::string& errorMessage) const {
	std::stringstream errors;
	bool isValid = true;

	// Check if model exists
	if (!model) {
		errors << "Model is null. ";
		return false; // Critical error, no need to continue
	}

	const auto& modelData = model->GetModelData();

	// Check vertex array object
	if (modelData.vA == 0) {
		errors << "VAO is invalid (0). ";
		isValid = false;
	}

	// Check vertex buffer
	if (modelData.vBPosID == 0) {
		errors << "Position buffer is invalid (0). ";
		isValid = false;
	}

	// Check index buffer
	if (modelData.iBID == 0) {
		errors << "Index buffer is invalid (0). ";
		isValid = false;
	}

	// Check vertex data
	if (modelData.vertexData.empty()) {
		errors << "Vertex data is empty. ";
		isValid = false;
	}

	// Check index data
	if (modelData.indexData.empty()) {
		errors << "Index data is empty. ";
		isValid = false;
	}

	// Check for invalid indices
	for (size_t i = 0; i < modelData.indexData.size(); i++) {
		if (modelData.indexData[i] >= modelData.vertexData.size()) {
			errors << "Invalid index at position " << i << ": "
				<< modelData.indexData[i] << " (max: " << modelData.vertexData.size() - 1 << "). ";
			isValid = false;
			break; // Report just the first error to avoid huge error messages
		}
	}

	/*if (!_boundingBox.isValid()) {
		errors << "Bounding box is invalid. ";
		isValid = false;
	}*/

	// Check model ID
	if (model->GetID() == 0) {
		errors << "Model ID is 0. ";
		isValid = false;
	}

	// Check if texture coordinates are provided but buffer is missing
	if (!modelData.vertex_texCoords.empty() && modelData.vBTCoordsID == 0) {
		errors << "Texture coordinates exist but buffer is invalid. ";
		isValid = false;
	}

	// Check if normals are provided but buffer is missing
	if (!modelData.vertex_normals.empty() && modelData.vBNormalsID == 0) {
		errors << "Normals exist but buffer is invalid. ";
		isValid = false;
	}

	// Check for animated mesh inconsistencies
	if (model->isAnimated) {
		bool hasBoneData = false;
		for (const auto& vertex : modelData.vertexData) {
			if (vertex.m_BoneIDs[0] != -1) {
				hasBoneData = true;
				break;
			}
		}

		if (!hasBoneData) {
			errors << "Mesh is marked as animated but has no bone data. ";
			isValid = false;
		}

		if (modelData.vBBoneIDsID == 0 || modelData.vBBoneWeightsID == 0) {
			errors << "Animated mesh but bone buffers are invalid. ";
			isValid = false;
		}
	}

	// Check OpenGL state for this mesh
	if (modelData.vA != 0) {
		GLint currentVAO;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);

		glBindVertexArray(modelData.vA);

		// Check if required attributes are enabled
		GLint enabled;
		glGetVertexAttribiv(ATTR_POSITION, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		if (enabled == GL_FALSE) {
			errors << "Position attribute not enabled in VAO. ";
			isValid = false;
		}

		if (!modelData.vertex_texCoords.empty()) {
			glGetVertexAttribiv(ATTR_TEXCOORD, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
			if (enabled == GL_FALSE) {
				errors << "TexCoord attribute not enabled in VAO. ";
				isValid = false;
			}
		}

		if (!modelData.vertex_normals.empty()) {
			glGetVertexAttribiv(ATTR_NORMAL, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
			if (enabled == GL_FALSE) {
				errors << "Normal attribute not enabled in VAO. ";
				isValid = false;
			}
		}

		// Restore previous state
		glBindVertexArray(currentVAO);
	}

	errorMessage = errors.str();
	if (errorMessage.empty()) {
		errorMessage = "Mesh validation passed.";
	}

	return isValid;
}