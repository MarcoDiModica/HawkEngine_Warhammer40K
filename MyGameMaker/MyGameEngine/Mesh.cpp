#include "Mesh.h"
#include "GameObject.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include <assimp/Importer.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include "../MyGameEditor/Log.h"
#include <zlib.h>

#include <queue>
using namespace std;

Mesh::Mesh() :aabbMin(vec3(0.0f)), aabbMax(vec3(0.0f))
{
	_boundingBox = BoundingBox();
}


Mesh::~Mesh() {}



void Mesh::Load(const glm::vec3* vertices, size_t num_verts, const unsigned int* indices, size_t num_indexs)
{
	_boundingBox.min = _vertices.front();
	_boundingBox.max = _vertices.front();

	for (const auto& v : _vertices) {
		_boundingBox.min = glm::min(_boundingBox.min, glm::dvec3(v));
		_boundingBox.max = glm::max(_boundingBox.max, glm::dvec3(v));
	}

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

void Mesh::CalculateNormals() {
	_normals.resize(_vertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < _indices.size(); i += 3) {
		glm::vec3 v0 = _vertices[_indices[i]];
		glm::vec3 v1 = _vertices[_indices[i + 1]];
		glm::vec3 v2 = _vertices[_indices[i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		_normals[_indices[i]] += normal;
		_normals[_indices[i + 1]] += normal;
		_normals[_indices[i + 2]] += normal;
	}

	for (auto& normal : _normals) {
		normal = glm::normalize(normal);
	}

	//normals_buffer.LoadData(_normals.data(), _normals.size() * sizeof(glm::vec3));
}

void Mesh::calculateTangentSpace()
{
	_tangents.resize(_vertices.size(), vec3(0.0f));
	_bitangents.resize(_vertices.size(), vec3(0.0f));

	for (size_t i = 0; i < _indices.size(); i += 3) {
		uint32_t i0 = _indices[i];
		uint32_t i1 = _indices[i + 1];
		uint32_t i2 = _indices[i + 2];

		const vec3& v0 = _vertices[i0];
		const vec3& v1 = _vertices[i1];
		const vec3& v2 = _vertices[i2];

		if (_texCoords.size() <= i2) continue;

		const vec2& uv0 = _texCoords[i0];
		const vec2& uv1 = _texCoords[i1];
		const vec2& uv2 = _texCoords[i2];

		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		double r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		if (r != r) r = 0.0f;

		vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		_tangents[i0] += tangent;
		_tangents[i1] += tangent;
		_tangents[i2] += tangent;

		_bitangents[i0] += bitangent;
		_bitangents[i1] += bitangent;
		_bitangents[i2] += bitangent;
	}

	for (size_t i = 0; i < _vertices.size(); i++) {
		if (i >= _normals.size()) continue;

		const vec3& n = _normals[i];
		vec3& t = _tangents[i];

		t = normalize(t - n * dot(n, t));

		vec3& b = _bitangents[i];
		b = cross(n, t);
	}

	if (model && !model->GetModelData().vertexData.empty()) {
		for (size_t i = 0; i < model->GetModelData().vertexData.size() && i < _tangents.size(); i++) {
			model->GetModelData().vertexData[i].tangent = _tangents[i];
			model->GetModelData().vertexData[i].bitangent = _bitangents[i];
		}
	}
}

void Mesh::Draw() const
{
	////display();

	////if (drawBoundingbox) {

	//	
	////}

	//if (drawVertexNormals)
	//{
	//	glColor3f(0.0f, 0.0f, 1.0f); // Blue color for vertex normals
	//	glBegin(GL_LINES);

	//	for (size_t i = 0; i < _vertices.size(); ++i) {
	//		vec3 end = _vertices[i] + _normals[i] * 0.2;
	//		glVertex3fv(glm::value_ptr(_vertices[i]));
	//		glVertex3fv(glm::value_ptr(end));
	//	}

	//	glEnd();
	//	glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
	//}

	//if (drawTriangleNormals) {
	//	glColor3f(0.0f, 1.0f, 0.0f); // Green color for triangle normals
	//	glBegin(GL_LINES);

	//	for (size_t i = 0; i < _indices.size(); i += 3) {
	//		glm::vec3 v0 = _vertices[_indices[i]];
	//		glm::vec3 v1 = _vertices[_indices[i + 1]];
	//		glm::vec3 v2 = _vertices[_indices[i + 2]];

	//		glm::vec3 center = (v0 + v1 + v2) / 3.0f;
	//		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	//		glm::vec3 end = center + normal * 0.2f;

	//		glVertex3fv(glm::value_ptr(center));
	//		glVertex3fv(glm::value_ptr(end));
	//	}

	//	glEnd();
	//	glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white	
	//}
	//if (drawFaceNormals)
	//{
	//	glColor3f(1.0f, 0.0f, 0.0f); // Red color for face normals
	//	glm::vec3 center = ((glm::vec3)_boundingBox.min + (glm::vec3)_boundingBox.max) / 2.0f;
	//	glBegin(GL_LINES);

	//	for (size_t i = 0; i < _indices.size(); i += 3) {
	//		glm::vec3 v0 = _vertices[_indices[i]];
	//		glm::vec3 v1 = _vertices[_indices[i + 1]];
	//		glm::vec3 v2 = _vertices[_indices[i + 2]];

	//		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	//		glm::vec3 end = center + normal * 0.2f;

	//		glVertex3fv(glm::value_ptr(center));
	//		glVertex3fv(glm::value_ptr(end));
	//	}

	//	glEnd();
	//	glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
	//}

}

std::shared_ptr<Mesh> Mesh::CreateCube() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	Vertex vertices[8];

	vertices[0].position = vec3(-1.0f, -1.0f, 1.0f);
	vertices[1].position = vec3(1.0f, -1.0f, 1.0f);
	vertices[2].position = vec3(1.0f, 1.0f, 1.0f);
	vertices[3].position = vec3(-1.0f, 1.0f, 1.0f);
	vertices[4].position = vec3(-1.0f, -1.0f, -1.0f);
	vertices[5].position = vec3(1.0f, -1.0f, -1.0f);
	vertices[6].position = vec3(1.0f, 1.0f, -1.0f);
	vertices[7].position = vec3(-1.0f, 1.0f, -1.0f);

	vertices[0].texCoords = vec2(0.0f, 0.0f);
	vertices[1].texCoords = vec2(1.0f, 0.0f);
	vertices[2].texCoords = vec2(1.0f, 1.0f);
	vertices[3].texCoords = vec2(0.0f, 1.0f);
	vertices[4].texCoords = vec2(1.0f, 0.0f);
	vertices[5].texCoords = vec2(0.0f, 0.0f);
	vertices[6].texCoords = vec2(0.0f, 1.0f);
	vertices[7].texCoords = vec2(1.0f, 1.0f);

	vertices[0].normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[1].normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[2].normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[3].normal = vec3(0.0f, 0.0f, 1.0f);
	vertices[4].normal = vec3(0.0f, 0.0f, -1.0f);
	vertices[5].normal = vec3(0.0f, 0.0f, -1.0f);
	vertices[6].normal = vec3(0.0f, 0.0f, -1.0f);
	vertices[7].normal = vec3(0.0f, 0.0f, -1.0f);

	model->GetModelData().vertexData.assign(vertices, vertices + 8);

	model->GetModelData().indexData = {
		0, 1, 2, 0, 2, 3,
		1, 5, 6, 1, 6, 2,
		5, 4, 7, 5, 7, 6,
		4, 0, 3, 4, 3, 7,
		3, 2, 6, 3, 6, 7,
		4, 5, 1, 4, 1, 0
	};

	model->SetMeshName("Cube");

	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
	meshBBox->min = model->GetModelData().vertexData.front().position;
	meshBBox->max = model->GetModelData().vertexData.front().position;

	for (const auto& v : model->GetModelData().vertexData) {
		meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
		meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
	}

	mesh->setBoundingBox(*meshBBox);
	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cube");

	for (const auto& vertex : model->GetModelData().vertexData) {
		mesh->_vertices.push_back(vertex.position);
		mesh->_normals.push_back(vertex.normal);
		mesh->_texCoords.push_back(vertex.texCoords);
	}
	mesh->_indices = model->GetModelData().indexData;

	mesh->calculateTangentSpace();

	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateCylinder() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	const float radius = 1.0f;  // Radio del cilindro
	const float height = 2.0f;  // Altura del cilindro
	const int slices = 20;      // Número de divisiones

	// Altura dividida en dos (para las bases superior e inferior)
	float halfHeight = height / 2.0f;

	// Generar vértices para las bases superior e inferior
	for (int i = 0; i <= slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// Base inferior
		//model->GetModelData().vertexData.emplace_back(x, -halfHeight, z);

		//// Base superior
		//model->GetModelData().vertexData.emplace_back(x, halfHeight, z);
	}

	// Añadir los vértices centrales de las bases
	//model->GetModelData().vertexData.emplace_back(0.0f, -halfHeight, 0.0f); // Centro base inferior
	//model->GetModelData().vertexData.emplace_back(0.0f, halfHeight, 0.0f);  // Centro base superior

	// Generar índices para los lados del cilindro
	for (int i = 0; i < slices; ++i) {
		int base1 = i * 2;
		int base2 = base1 + 2;

		model->GetModelData().indexData.push_back(base1);
		model->GetModelData().indexData.push_back(base2);
		model->GetModelData().indexData.push_back(base1 + 1);

		model->GetModelData().indexData.push_back(base1 + 1);
		model->GetModelData().indexData.push_back(base2);
		model->GetModelData().indexData.push_back(base2 + 1);
	}

	// Generar índices para las bases
	int centerBottomIndex = model->GetModelData().vertexData.size() - 2;
	int centerTopIndex = model->GetModelData().vertexData.size() - 1;

	for (int i = 0; i < slices; ++i) {
		int baseIndex = i * 2;

		// Base inferior
		model->GetModelData().indexData.push_back(centerBottomIndex);
		model->GetModelData().indexData.push_back(baseIndex + 2);
		model->GetModelData().indexData.push_back(baseIndex);

		// Base superior
		model->GetModelData().indexData.push_back(centerTopIndex);
		model->GetModelData().indexData.push_back(baseIndex + 1);
		model->GetModelData().indexData.push_back(baseIndex + 3);
	}

	model->SetMeshName("Cylinder");

	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();


	meshBBox->min = model->GetModelData().vertexData.front().position;
	meshBBox->max = model->GetModelData().vertexData.front().position;

	for (const auto& v : model->GetModelData().vertexData) {
		meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
		meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
	}

	mesh->setBoundingBox(*meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cylinder");
	mesh->loadToOpenGL();

	return mesh;
}


std::shared_ptr<Mesh> Mesh::CreateSphere()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	const int stacks = 20;
	const int slices = 20;
	const float radius = 1.0f;

	std::shared_ptr<Model> model = std::make_shared<Model>();

	for (int i = 0; i <= stacks; ++i) {
		float V = i / (float)stacks;
		float phi = V * glm::pi<float>();

		for (int j = 0; j <= slices; ++j) {
			float U = j / (float)slices;
			float theta = U * (glm::pi<float>() * 2);

			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			Vertex vertex;

			vertex.position = glm::vec3(x, y, z) * radius;

			model->GetModelData().vertexData.push_back(vertex);
		}
	}

	for (int i = 0; i < slices * stacks + slices; ++i) {
		model->GetModelData().indexData.push_back(i);
		model->GetModelData().indexData.push_back(i + slices + 1);
		model->GetModelData().indexData.push_back(i + slices);

		model->GetModelData().indexData.push_back(i + slices + 1);
		model->GetModelData().indexData.push_back(i);
		model->GetModelData().indexData.push_back(i + 1);
	}

	model->SetMeshName("Sphere");

	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();


	meshBBox->min = model->GetModelData().vertexData.front().position;
	meshBBox->max = model->GetModelData().vertexData.front().position;

	for (const auto& v : model->GetModelData().vertexData) {
		meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
		meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
	}

	mesh->setBoundingBox(*meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Sphere");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreatePlane()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	model->GetModelData().vertexData = {
		Vertex {vec3(-1.0f, 0.0f, 1.0f)},
		Vertex {vec3(1.0f, 0.0f, 1.0f)},
		Vertex {vec3(1.0f, 0.0f, -1.0f)},
		Vertex {vec3(-1.0f, 0.0f, -1.0f)}
	};

	model->GetModelData().indexData = {
		0, 1, 2, 0, 2, 3
	};

	model->GetModelData().vertex_texCoords = {
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f)
	};

	model->SetMeshName("Plane");

	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();


	meshBBox->min = model->GetModelData().vertexData.front().position;
	meshBBox->max = model->GetModelData().vertexData.front().position;

	for (const auto& v : model->GetModelData().vertexData) {
		meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
		meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
	}

	mesh->setBoundingBox(*meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Plane");
	mesh->loadToOpenGL();

	return mesh;
}

std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;

void Mesh::SaveBinary(const std::string& filename) const {
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

	bool hasTexCoords = !_texCoords.empty();
	fout.write(reinterpret_cast<const char*>(&hasTexCoords), sizeof(bool));
	if (hasTexCoords) {
		fout.write(reinterpret_cast<const char*>(_texCoords.data()), numVertices * sizeof(glm::vec2));
	}

	bool hasTangentSpace = !_tangents.empty() && !_bitangents.empty();
	fout.write(reinterpret_cast<const char*>(&hasTangentSpace), sizeof(bool));
	if (hasTangentSpace) {
		fout.write(reinterpret_cast<const char*>(_tangents.data()), numVertices * sizeof(glm::vec3));
		fout.write(reinterpret_cast<const char*>(_bitangents.data()), numVertices * sizeof(glm::vec3));
	}

	fout.write(reinterpret_cast<const char*>(&_boundingBox.min), sizeof(glm::dvec3));
	fout.write(reinterpret_cast<const char*>(&_boundingBox.max), sizeof(glm::dvec3));
}

std::shared_ptr<Mesh> Mesh::LoadBinary(std::string& filename) {
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
	auto model = std::make_shared<Model>();

	uint32_t numVertices, numIndices;
	fin.read(reinterpret_cast<char*>(&numVertices), sizeof(numVertices));
	fin.read(reinterpret_cast<char*>(&numIndices), sizeof(numIndices));

	mesh->_vertices.resize(numVertices);
	mesh->_indices.resize(numIndices);
	mesh->_normals.resize(numVertices);
	fin.read(reinterpret_cast<char*>(mesh->_vertices.data()), numVertices * sizeof(glm::vec3));
	fin.read(reinterpret_cast<char*>(mesh->_indices.data()), numIndices * sizeof(unsigned int));
	fin.read(reinterpret_cast<char*>(mesh->_normals.data()), numVertices * sizeof(glm::vec3));

	bool hasTexCoords;
	fin.read(reinterpret_cast<char*>(&hasTexCoords), sizeof(bool));
	if (hasTexCoords) {
		mesh->_texCoords.resize(numVertices);
		fin.read(reinterpret_cast<char*>(mesh->_texCoords.data()), numVertices * sizeof(glm::vec2));
	}

	bool hasTangentSpace;
	fin.read(reinterpret_cast<char*>(&hasTangentSpace), sizeof(bool));

	if (hasTangentSpace) {
		mesh->_tangents.resize(numVertices);
		mesh->_bitangents.resize(numVertices);
		fin.read(reinterpret_cast<char*>(mesh->_tangents.data()), numVertices * sizeof(glm::vec3));
		fin.read(reinterpret_cast<char*>(mesh->_bitangents.data()), numVertices * sizeof(glm::vec3));
	}

	fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.min), sizeof(glm::dvec3));
	fin.read(reinterpret_cast<char*>(&mesh->_boundingBox.max), sizeof(glm::dvec3));

	model->GetModelData().indexData = mesh->_indices;
	model->GetModelData().vertexData.resize(numVertices);

	for (size_t i = 0; i < numVertices; i++) {
		model->GetModelData().vertexData[i].position = mesh->_vertices[i];
		model->GetModelData().vertexData[i].normal = mesh->_normals[i];

		if (hasTexCoords) {
			model->GetModelData().vertexData[i].texCoords = mesh->_texCoords[i];
		}

		if (hasTangentSpace) {
			model->GetModelData().vertexData[i].tangent = mesh->_tangents[i];
			model->GetModelData().vertexData[i].bitangent = mesh->_bitangents[i];
		}
	}

	if (hasTexCoords) {
		model->GetModelData().vertex_texCoords = mesh->_texCoords;
	}

	if (!mesh->_normals.empty()) {
		model->GetModelData().vertex_normals = mesh->_normals;
	}

	mesh->setModel(model);
	mesh->nameM = filename;

	mesh->loadToOpenGL();

	meshCache[fullPath] = mesh;
	return mesh;
}

void Mesh::loadToOpenGL() {
	glGenVertexArrays(1, &model->GetModelData().vA);
	glBindVertexArray(model->GetModelData().vA);

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> tangents;
	std::vector<vec3> bitangents;

	if (!model->GetModelData().vertexData.empty()) {
		for (const auto& vertex : model->GetModelData().vertexData) {
			positions.push_back(vertex.position);

			if (length(vertex.normal) > 0.0f) {
				normals.push_back(vertex.normal);
			}

			if (length(vertex.texCoords) > 0.0f) {
				texCoords.push_back(vertex.texCoords);
			}

			if (length(vertex.tangent) > 0.0f) {
				tangents.push_back(vertex.tangent);
			}

			if (length(vertex.bitangent) > 0.0f) {
				bitangents.push_back(vertex.bitangent);
			}
		}
	}
	else {
		positions = _vertices;
		normals = _normals;
		texCoords = _texCoords;
		tangents = _tangents;
		bitangents = _bitangents;
	}

	glGenBuffers(1, &model->GetModelData().vBPosID);
	glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBPosID);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0);

	if (!normals.empty()) {
		glGenBuffers(1, &model->GetModelData().vBNormalsID);
		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBNormalsID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0);
	}
	else if (!model->GetModelData().vertex_normals.empty()) {
		glGenBuffers(1, &model->GetModelData().vBNormalsID);
		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBNormalsID);
		glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_normals.size() * sizeof(vec3),
			model->GetModelData().vertex_normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	if (!texCoords.empty()) {
		glGenBuffers(1, &model->GetModelData().vBTCoordsID);
		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBTCoordsID);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, sizeof(vec2), (const void*)0);
	}
	else if (!model->GetModelData().vertex_texCoords.empty()) {
		glGenBuffers(1, &model->GetModelData().vBTCoordsID);
		glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBTCoordsID);
		glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_texCoords.size() * sizeof(vec2),
			model->GetModelData().vertex_texCoords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, sizeof(vec2), (const void*)0);
	}

	if (!tangents.empty()) {
		unsigned int tangentBuffer;
		glGenBuffers(1, &tangentBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(vec3), tangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	if (!bitangents.empty()) {
		unsigned int bitangentBuffer;
		glGenBuffers(1, &bitangentBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(vec3), bitangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0);
	}

	glCreateBuffers(1, &model->GetModelData().iBID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->GetModelData().iBID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->GetModelData().indexData.size() * sizeof(unsigned int),
		model->GetModelData().indexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}