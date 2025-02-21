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

void Mesh::Draw() const
{
	//display();

	

	if (drawVertexNormals)
	{
		glColor3f(0.0f, 0.0f, 1.0f); // Blue color for vertex normals
		glBegin(GL_LINES);

		for (size_t i = 0; i < _vertices.size(); ++i) {
			glm::vec3 end = _vertices[i] + _normals[i] * 0.2f;
			glVertex3fv(glm::value_ptr(_vertices[i]));
			glVertex3fv(glm::value_ptr(end));
		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
	}

	if (drawTriangleNormals) {
		glColor3f(0.0f, 1.0f, 0.0f); // Green color for triangle normals
		glBegin(GL_LINES);

		for (size_t i = 0; i < _indices.size(); i += 3) {
			glm::vec3 v0 = _vertices[_indices[i]];
			glm::vec3 v1 = _vertices[_indices[i + 1]];
			glm::vec3 v2 = _vertices[_indices[i + 2]];

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
			glm::vec3 v0 = _vertices[_indices[i]];
			glm::vec3 v1 = _vertices[_indices[i + 1]];
			glm::vec3 v2 = _vertices[_indices[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
			glm::vec3 end = center + normal * 0.2f;

			glVertex3fv(glm::value_ptr(center));
			glVertex3fv(glm::value_ptr(end));
		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
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
	const int slices = 20;      // N�mero de divisiones

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	// Altura dividida en dos (para las bases superior e inferior)
	float halfHeight = height / 2.0f;

	// Generar v�rtices para las bases superior e inferior
	for (int i = 0; i <= slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// Base inferior
		vertices.emplace_back(x, -halfHeight, z);

		// Base superior
		vertices.emplace_back(x, halfHeight, z);
	}

	// A�adir los v�rtices centrales de las bases
	vertices.emplace_back(0.0f, -halfHeight, 0.0f); // Centro base inferior
	vertices.emplace_back(0.0f, halfHeight, 0.0f);  // Centro base superior

	// Generar �ndices para los lados del cilindro
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

	// Generar �ndices para las bases
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

	/*mesh->vertices_buffer.LoadData(mesh->_vertices.data(), numVertices * sizeof(glm::vec3));
	mesh->indices_buffer.LoadIndices(mesh->_indices.data(), numIndices);
	mesh->normals_buffer.LoadData(mesh->_normals.data(), numVertices * sizeof(glm::vec3));*/

	//texcords?colors?

	meshCache[fullPath] = mesh;
	mesh->nameM = filename;
	return mesh;
}

void Mesh::loadToOpenGL()
{
	(glGenVertexArrays(1, &model->GetModelData().vA));
	(glBindVertexArray(model->GetModelData().vA));

	//buffer de positions
	(glGenBuffers(1, &model->GetModelData().vBPosID));
	(glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBPosID));
	(glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertexData.size() * sizeof(vec3), model->GetModelData().vertexData.data(), GL_STATIC_DRAW));

	//position layout
	(glEnableVertexAttribArray(0));
	(glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0));

	//buffer de coordenades de textura
	if (model->GetModelData().vertex_texCoords.size() > 0)
	{
		(glGenBuffers(1, &model->GetModelData().vBTCoordsID));
		(glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBTCoordsID));
		(glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_texCoords.size() * sizeof(vec2), model->GetModelData().vertex_texCoords.data(), GL_STATIC_DRAW));

		//tex coord layout
		(glEnableVertexAttribArray(1));
		(glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(vec2), (const void*)0));
	}

	//buffer de normals
	if (model->GetModelData().vertex_normals.size() > 0)
	{
		(glGenBuffers(1, &model->GetModelData().vBNormalsID));
		(glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBNormalsID));
		(glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_normals.size() * sizeof(vec3), model->GetModelData().vertex_normals.data(), GL_STATIC_DRAW));

		//normal layout
		(glEnableVertexAttribArray(2));
		(glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3), (const void*)0));

		//load normals lines for debugging
		//loadNormalsToOpenGL();
		//loadFaceNormalsToOpenGL();
	}

	//buffer de index
	(glCreateBuffers(1, &model->GetModelData().iBID));
	(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->GetModelData().iBID));
	(glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->GetModelData().indexData.size() * sizeof(unsigned int), model->GetModelData().indexData.data(), GL_STATIC_DRAW));

	(glBindBuffer(GL_ARRAY_BUFFER, 0));
	(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	(glBindVertexArray(0));
}
