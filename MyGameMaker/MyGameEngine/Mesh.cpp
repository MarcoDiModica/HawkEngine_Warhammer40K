#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <zlib.h>
#include <queue>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "../MyGameEditor/Log.h"

Mesh::Mesh() :aabbMin(vec3(0.0f)), aabbMax(vec3(0.0f))
{
	_boundingBox = BoundingBox();
}


Mesh::~Mesh() {}


void Mesh::Load(const glm::vec3* vertices, size_t num_verts, const unsigned int* indices, size_t num_indexs)
{
	vertices_buffer.LoadData(vertices, num_verts * sizeof(glm::vec3));
	indices_buffer.LoadIndices(indices, num_indexs);
	texCoords_buffer.UnLoad();
	normals_buffer.UnLoad();
	colors_buffer.UnLoad();

	_vertices.clear();
	_indices.clear();
	_vertices.resize(num_verts);
	for (size_t i = 0; i < num_verts; ++i) {
		_vertices[i].position = vertices[i];
	}
	_indices.assign(indices, indices + num_indexs);

	_boundingBox.min = _vertices.front().position;
	_boundingBox.max = _vertices.front().position;

	for (const auto& v : _vertices) {
		_boundingBox.min = glm::min(_boundingBox.min, glm::dvec3(v.position));
		_boundingBox.max = glm::max(_boundingBox.max, glm::dvec3(v.position));
	}

	// ids
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	// weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, m_Weights));

	CalculateNormals();
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

void Mesh::loadTexCoords(const glm::vec2* texCoords, size_t num_texCoords)
{
	texCoords_buffer.LoadData(texCoords, num_texCoords * sizeof(glm::vec2));
	_texCoords.assign(texCoords, texCoords + num_texCoords);
}

void Mesh::LoadNormals(const glm::vec3* normals, size_t num_normals)
{
	normals_buffer.LoadData(normals, num_normals * sizeof(glm::vec3));
	_normals.assign(normals, normals + num_normals);
}

void Mesh::LoadColors(const glm::u8vec3* colors, size_t num_colors)
{
	colors_buffer.LoadData(colors, num_colors * sizeof(glm::u8vec3));
}

void Mesh::LoadBones() 
{

}

void Mesh::CalculateNormals() {
	_normals.resize(_vertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < _indices.size(); i += 3) {
		glm::vec3 v0 = _vertices[_indices[i]].position;
		glm::vec3 v1 = _vertices[_indices[i + 1]].position;
		glm::vec3 v2 = _vertices[_indices[i + 2]].position;

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		_normals[_indices[i]] += normal;
		_normals[_indices[i + 1]] += normal;
		_normals[_indices[i + 2]] += normal;
	}

	for (auto& normal : _normals) {
		normal = glm::normalize(normal);
	}

	normals_buffer.LoadData(_normals.data(), _normals.size() * sizeof(glm::vec3));
}

void Mesh::Draw() const
{
	//display();

	glEnable(GL_TEXTURE_2D);
	if (texCoords_buffer.Id()) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		texCoords_buffer.bind();
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}

	if (normals_buffer.Id()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		normals_buffer.bind();
		glNormalPointer(GL_FLOAT, 0, nullptr);
	}

	if (colors_buffer.Id()) {
		glEnableClientState(GL_COLOR_ARRAY);
		colors_buffer.bind();
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	vertices_buffer.bind();
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	indices_buffer.bind();

	if (drawWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, nullptr);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (colors_buffer.Id()) glDisableClientState(GL_COLOR_ARRAY);
	if (normals_buffer.Id()) glDisableClientState(GL_NORMAL_ARRAY);
	if (texCoords_buffer.Id()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (drawBoundingbox) {

		drawBoundingBox(_boundingBox);
	}
	glDisable(GL_TEXTURE_2D);
	if (drawVertexNormals)
	{
		glColor3f(0.0f, 0.0f, 1.0f); // Blue color for vertex normals
		glBegin(GL_LINES);

		for (size_t i = 0; i < _vertices.size(); ++i) {
			glm::vec3 end = _vertices[i].position + _normals[i] * 0.2f;
			glVertex3fv(glm::value_ptr(_vertices[i].position));
			glVertex3fv(glm::value_ptr(end));
		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
	}

	if (drawTriangleNormals) {
		glColor3f(0.0f, 1.0f, 0.0f); // Green color for triangle normals
		glBegin(GL_LINES);

		for (size_t i = 0; i < _indices.size(); i += 3) {
			glm::vec3 v0 = _vertices[_indices[i]].position;
			glm::vec3 v1 = _vertices[_indices[i + 1]].position;
			glm::vec3 v2 = _vertices[_indices[i + 2]].position;

			glm::vec3 center = (v0 + v1 + v2) / 3.0f;
			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
			glm::vec3 end = center + normal * 0.2f;

			glVertex3fv(glm::value_ptr(center));
			glVertex3fv(glm::value_ptr(end));
		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white	
	}
	if (drawFaceNormals)
	{
		glColor3f(1.0f, 0.0f, 0.0f); // Red color for face normals
		glm::vec3 center = ((glm::vec3)_boundingBox.min + (glm::vec3)_boundingBox.max) / 2.0f;
		glBegin(GL_LINES);

		for (size_t i = 0; i < _indices.size(); i += 3) {
			glm::vec3 v0 = _vertices[_indices[i]].position;
			glm::vec3 v1 = _vertices[_indices[i + 1]].position;
			glm::vec3 v2 = _vertices[_indices[i + 2]].position;

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
			glm::vec3 end = center + normal * 0.2f;

			glVertex3fv(glm::value_ptr(center));
			glVertex3fv(glm::value_ptr(end));
		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
	}

}

void Mesh::LoadMesh(const char* file_path)
{

	filePath = std::string(file_path);

	std::cout << std::endl << file_path;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) {
		std::vector<glm::vec3> all_vertices;
		std::vector<Vertex> all_vertex;
		std::vector<unsigned int> all_indices;
		std::vector<glm::vec2> all_texCoords;
		std::vector<glm::vec3> all_normals;
		std::vector<glm::u8vec3> all_colors;

		unsigned int vertex_offset = 0;

		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];

			// Copy vertices
			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
				all_vertices.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));	

				Vertex vertex;

				SetVertexBoneDataToDefault(vertex);

				vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
				all_vertex.push_back(vertex);

				if (mesh->HasNormals()) {

						all_normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
					
				}

				// Copy colors
				if (mesh->HasVertexColors(0)) {
				
						all_colors.push_back(glm::u8vec3(mesh->mColors[0][j].r * 255, mesh->mColors[0][j].g * 255, mesh->mColors[0][j].b * 255));
					
				}
				
			}
			ExtractBoneWeightForVertices(all_vertex, mesh, scene);

			LOG(LogType::LOG_ASSIMP, "Loaded vertices :%d for mesh %d", mesh->mNumVertices, i);
			// Copy indices
			for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
				aiFace& face = mesh->mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; k++) {
					all_indices.push_back(face.mIndices[k] + vertex_offset);
				}
			}
			LOG(LogType::LOG_ASSIMP, "Loaded faces :%d for mesh %d", mesh->mNumFaces, i);
			// Copy texture coordinates
			if (mesh->HasTextureCoords(0)) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_texCoords.push_back(glm::vec2(mesh->mTextureCoords[0][j].x, -mesh->mTextureCoords[0][j].y));
				}
			}


			
			// Copy normals
		

			vertex_offset += mesh->mNumVertices;
		}

		// Load the combined mesh data
		Load(all_vertices.data(), all_vertices.size(), all_indices.data(), all_indices.size());

		if (!all_texCoords.empty()) {
			loadTexCoords(all_texCoords.data(), all_texCoords.size());
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(1);
		}

		if (!all_normals.empty()) {
			LoadNormals(all_normals.data(), all_normals.size());
		}

		if (!all_colors.empty()) {
			LoadColors(all_colors.data(), all_colors.size());
		}

		aiReleaseImport(scene);
	}
	else {
		// Handle error
		std::cout << "Error loading mesh: " << file_path << std::endl;
	}
}

std::shared_ptr<Mesh> Mesh::CreateCube()
{

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	const glm::vec3 vertices[] = {
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f)
	};

	const unsigned int indices[] = {
		0, 1, 2, 0, 2, 3,
		1, 5, 6, 1, 6, 2,
		5, 4, 7, 5, 7, 6,
		4, 0, 3, 4, 3, 7,
		3, 2, 6, 3, 6, 7,
		4, 5, 1, 4, 1, 0
	};

	mesh->Load(vertices, 8, indices, 36);
	mesh->filePath = std::string("shapes/cube");
	return mesh;
}


std::shared_ptr<Mesh> Mesh::CreateCylinder() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	// Valores fijos para el cilindro
	const float radius = 1.0f;  // Radio del cilindro
	const float height = 2.0f;  // Altura del cilindro
	const int slices = 20;      // Número de divisiones

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	// Altura dividida en dos (para las bases superior e inferior)
	float halfHeight = height / 2.0f;

	// Generar vértices para las bases superior e inferior
	for (int i = 0; i <= slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// Base inferior
		vertices.emplace_back(x, -halfHeight, z);

		// Base superior
		vertices.emplace_back(x, halfHeight, z);
	}

	// Añadir los vértices centrales de las bases
	vertices.emplace_back(0.0f, -halfHeight, 0.0f); // Centro base inferior
	vertices.emplace_back(0.0f, halfHeight, 0.0f);  // Centro base superior

	// Generar índices para los lados del cilindro
	for (int i = 0; i < slices; ++i) {
		int base1 = i * 2;
		int base2 = base1 + 2;

		indices.push_back(base1);
		indices.push_back(base2);
		indices.push_back(base1 + 1);

		indices.push_back(base1 + 1);
		indices.push_back(base2);
		indices.push_back(base2 + 1);
	}

	// Generar índices para las bases
	int centerBottomIndex = vertices.size() - 2;
	int centerTopIndex = vertices.size() - 1;

	for (int i = 0; i < slices; ++i) {
		int baseIndex = i * 2;

		// Base inferior
		indices.push_back(centerBottomIndex);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex);

		// Base superior
		indices.push_back(centerTopIndex);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 3);
	}

	// Cargar los datos en la malla
	mesh->Load(vertices.data(), vertices.size(), indices.data(), indices.size());
	mesh->filePath = std::string("shapes/cylinder");

	return mesh;
}


std::shared_ptr<Mesh> Mesh::CreateSphere()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	const int stacks = 20;
	const int slices = 20;
	const float radius = 1.0f;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i <= stacks; ++i) {
		float V = i / (float)stacks;
		float phi = V * glm::pi<float>();

		for (int j = 0; j <= slices; ++j) {
			float U = j / (float)slices;
			float theta = U * (glm::pi<float>() * 2);

			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			vertices.push_back(glm::vec3(x, y, z) * radius);
		}
	}

	for (int i = 0; i < slices * stacks + slices; ++i) {
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	mesh->Load(vertices.data(), vertices.size(), indices.data(), indices.size());
	mesh->filePath = std::string("shapes/sphere");
	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreatePlane()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	const glm::vec3 vertices[] = {
		glm::vec3(-1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f)
	};

	const unsigned int indices[] = {
		0, 1, 2, 0, 2, 3
	};

	mesh->Load(vertices, 4, indices, 6);
	mesh->filePath = std::string("shapes/plane");
	return mesh;
}

void Mesh::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Mesh::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Mesh::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;

void Mesh::SaveBinary(const std::string& filename) const
{
	std::string fullPath = "Library/Mesh/" + filename + ".mesh";

	if (!std::filesystem::exists("Library/Mesh")) {
		std::filesystem::create_directory("Library/Mesh");
	}

	std::ofstream fout(fullPath, std::ios::binary);

	uint32_t numVertices = static_cast<uint32_t>(_vertices.size());
	uint32_t numIndices = static_cast<uint32_t>(_indices.size());
	fout.write(reinterpret_cast<char*>(&numVertices), sizeof(numVertices));
	fout.write(reinterpret_cast<char*>(&numIndices), sizeof(numIndices));

	fout.write(reinterpret_cast<const char*>(_vertices.data()), numVertices * sizeof(glm::vec3));
	fout.write(reinterpret_cast<const char*>(_indices.data()), numIndices * sizeof(unsigned int));
	fout.write(reinterpret_cast<const char*>(_normals.data()), numVertices * sizeof(glm::vec3));

	fout.write(reinterpret_cast<const char*>(&_boundingBox.min), sizeof(glm::dvec3));
	fout.write(reinterpret_cast<const char*>(&_boundingBox.max), sizeof(glm::dvec3));

	//texcords?colors?
}

std::shared_ptr<Mesh> Mesh::LoadBinary( std::string& filename)
{
	std::string fullPath = "Library/Mesh/" + filename + ".mesh";

	auto it = meshCache.find(fullPath);
	if (it != meshCache.end()) {
		return it->second;
	}

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		LOG(LogType::LOG_ERROR, "Error al cargar la malla: %s", fullPath.c_str());
		return nullptr;
	}

	auto mesh = std::make_shared<Mesh>();

	uint32_t numVertices, numIndices;
	fin.read(reinterpret_cast<char*>(&numVertices), sizeof(numVertices));
	fin.read(reinterpret_cast<char*>(&numIndices), sizeof(numIndices));

	mesh->_vertices.resize(numVertices);
	mesh->_indices.resize(numIndices);
	mesh->_normals.resize(numVertices);
	fin.read(reinterpret_cast<char*>(mesh->_vertices.data()), numVertices * sizeof(glm::vec3));
	fin.read(reinterpret_cast<char*>(mesh->_indices.data()), numIndices * sizeof(unsigned int));
	fin.read(reinterpret_cast<char*>(mesh->_normals.data()), numVertices * sizeof(glm::vec3));

	fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.min), sizeof(glm::dvec3));
	fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.max), sizeof(glm::dvec3));

	mesh->vertices_buffer.LoadData(mesh->_vertices.data(), numVertices * sizeof(glm::vec3));
	mesh->indices_buffer.LoadIndices(mesh->_indices.data(), numIndices);
	mesh->normals_buffer.LoadData(mesh->_normals.data(), numVertices * sizeof(glm::vec3));

	//texcords?colors?

	meshCache[fullPath] = mesh;
	mesh->nameM = filename;
	return mesh;
}

