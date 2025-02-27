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

	//if (drawBoundingbox) {

		
	//}

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
	std::shared_ptr<Model> model = std::make_shared<Model>();

	model->GetModelData().vertexData = {
		// Front face
		Vertex{vec3(-1.0f, -1.0f, 1.0f)},
		Vertex{vec3(1.0f, -1.0f, 1.0f)},
		Vertex{vec3(1.0f, 1.0f, 1.0f)},
		Vertex{vec3(-1.0f, 1.0f, 1.0f)},
		// Back face
		Vertex{vec3(-1.0f, -1.0f, -1.0f)},
		Vertex{vec3(1.0f, -1.0f, -1.0f)},
		Vertex{vec3(1.0f, 1.0f, -1.0f)},
		Vertex{vec3(-1.0f, 1.0f, -1.0f)}
	};

	model->GetModelData().indexData = {
		0, 1, 2, 0, 2, 3,       // Front face
		1, 5, 6, 1, 6, 2,       // Right face
		5, 4, 7, 5, 7, 6,       // Back face
		4, 0, 3, 4, 3, 7,       // Left face
		3, 2, 6, 3, 6, 7,       // Top face
		4, 5, 1, 4, 1, 0        // Bottom face
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
	mesh->loadToOpenGL();

	return mesh;
}


std::shared_ptr<Mesh> Mesh::CreateCylinder() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	// Valores fijos para el cilindro
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
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
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

	std::vector<vec3> positions;

	for (int i = 0; i < model->GetModelData().vertexData.size(); i++)
	{
		positions.push_back(model->GetModelData().vertexData[i].position);
	}

	//buffer de positions
	(glGenBuffers(1, &model->GetModelData().vBPosID));
	(glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBPosID));
	(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW));

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
