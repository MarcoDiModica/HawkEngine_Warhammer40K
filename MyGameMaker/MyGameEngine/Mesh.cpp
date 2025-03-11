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

void Mesh::CalculateTangents() {
	// Resize tangent and bitangent arrays to match vertices
	std::vector<glm::vec3> tangents(_vertices.size(), glm::vec3(0.0f));
	std::vector<glm::vec3> bitangents(_vertices.size(), glm::vec3(0.0f));

	// Calculate tangents and bitangents for each triangle
	for (size_t i = 0; i < _indices.size(); i += 3) {
		unsigned int idx0 = _indices[i];
		unsigned int idx1 = _indices[i + 1];
		unsigned int idx2 = _indices[i + 2];

		glm::vec3& v0 = _vertices[idx0];
		glm::vec3& v1 = _vertices[idx1];
		glm::vec3& v2 = _vertices[idx2];

		// Get texture coordinates
		glm::vec2 uv0, uv1, uv2;
		if (i < model->GetModelData().vertex_texCoords.size()) {
			uv0 = model->GetModelData().vertex_texCoords[idx0];
			uv1 = model->GetModelData().vertex_texCoords[idx1];
			uv2 = model->GetModelData().vertex_texCoords[idx2];
		}
		else {
			// If no texture coordinates, use default
			uv0 = glm::vec2(0.0f, 0.0f);
			uv1 = glm::vec2(1.0f, 0.0f);
			uv2 = glm::vec2(0.0f, 1.0f);
		}

		// Calculate edges of the triangle
		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		// Calculate UV deltas
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		// Calculate tangent and bitangent
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent, bitangent;

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// Add to vertices (for averaging)
		tangents[idx0] += tangent;
		tangents[idx1] += tangent;
		tangents[idx2] += tangent;

		bitangents[idx0] += bitangent;
		bitangents[idx1] += bitangent;
		bitangents[idx2] += bitangent;
	}

	// Add calculated tangents and bitangents to model data
	model->GetModelData().vertex_tangents.resize(_vertices.size());
	model->GetModelData().vertex_bitangents.resize(_vertices.size());

	// Normalize all tangents and bitangents
	for (size_t i = 0; i < _vertices.size(); i++) {
		// Gram-Schmidt orthogonalization to make tangent perpendicular to normal
		glm::vec3 normal = glm::vec3(0.0f);
		if (i < model->GetModelData().vertex_normals.size()) {
			normal = model->GetModelData().vertex_normals[i];
		}
		else if (i < _normals.size()) {
			normal = _normals[i];
		}

		if (glm::length(normal) > 0.0f && glm::length(tangents[i]) > 0.0f) {
			// Orthogonalize tangent
			glm::vec3 t = glm::normalize(tangents[i]);
			t = glm::normalize(t - normal * glm::dot(normal, t));

			// Calculate handedness
			float handedness = (glm::dot(glm::cross(normal, t), bitangents[i]) < 0.0f) ? -1.0f : 1.0f;

			// Store final tangent and bitangent
			model->GetModelData().vertex_tangents[i] = t;
			model->GetModelData().vertex_bitangents[i] = handedness * glm::normalize(glm::cross(normal, t));
		}
		else {
			// Fallback to default tangent space
			model->GetModelData().vertex_tangents[i] = glm::vec3(1.0f, 0.0f, 0.0f);
			model->GetModelData().vertex_bitangents[i] = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}

	// Update OpenGL buffers with tangent and bitangent data
	glGenBuffers(1, &model->GetModelData().vBTangentsID);
	glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBTangentsID);
	glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_tangents.size() * sizeof(glm::vec3),
		model->GetModelData().vertex_tangents.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &model->GetModelData().vBBitangentsID);
	glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBBitangentsID);
	glBufferData(GL_ARRAY_BUFFER, model->GetModelData().vertex_bitangents.size() * sizeof(glm::vec3),
		model->GetModelData().vertex_bitangents.data(), GL_STATIC_DRAW);

	// Update VAO to include tangent and bitangent attributes
	glBindVertexArray(model->GetModelData().vA);

	// Tangent (attribute 3)
	glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBTangentsID);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	// Bitangent (attribute 4)
	glBindBuffer(GL_ARRAY_BUFFER, model->GetModelData().vBBitangentsID);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

	// Definimos los vértices de manera diferente para evitar compartir vértices entre caras
	// 24 vértices (4 por cada cara) con sus propias normales independientes

	// Posiciones de las 8 esquinas del cubo
	const vec3 v000(-1.0f, -1.0f, -1.0f);
	const vec3 v001(-1.0f, -1.0f, 1.0f);
	const vec3 v010(-1.0f, 1.0f, -1.0f);
	const vec3 v011(-1.0f, 1.0f, 1.0f);
	const vec3 v100(1.0f, -1.0f, -1.0f);
	const vec3 v101(1.0f, -1.0f, 1.0f);
	const vec3 v110(1.0f, 1.0f, -1.0f);
	const vec3 v111(1.0f, 1.0f, 1.0f);

	// Vector para almacenar los vértices
	std::vector<Vertex> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	std::vector<vec3> colors;
	std::vector<vec3> tangents;
	std::vector<vec3> bitangents;
	std::vector<unsigned int> indices;

	// Cara frontal (+Z)
	vertices.push_back({ v001 }); // 0
	vertices.push_back({ v101 }); // 1
	vertices.push_back({ v111 }); // 2
	vertices.push_back({ v011 }); // 3

	// Cara trasera (-Z)
	vertices.push_back({ v100 }); // 4
	vertices.push_back({ v000 }); // 5
	vertices.push_back({ v010 }); // 6
	vertices.push_back({ v110 }); // 7

	// Cara derecha (+X)
	vertices.push_back({ v101 }); // 8
	vertices.push_back({ v100 }); // 9
	vertices.push_back({ v110 }); // 10
	vertices.push_back({ v111 }); // 11

	// Cara izquierda (-X)
	vertices.push_back({ v000 }); // 12
	vertices.push_back({ v001 }); // 13
	vertices.push_back({ v011 }); // 14
	vertices.push_back({ v010 }); // 15

	// Cara superior (+Y)
	vertices.push_back({ v011 }); // 16
	vertices.push_back({ v111 }); // 17
	vertices.push_back({ v110 }); // 18
	vertices.push_back({ v010 }); // 19

	// Cara inferior (-Y)
	vertices.push_back({ v000 }); // 20
	vertices.push_back({ v100 }); // 21
	vertices.push_back({ v101 }); // 22
	vertices.push_back({ v001 }); // 23

	// Normals para cada vértice
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 0.0f, 1.0f));  // Cara frontal
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 0.0f, -1.0f)); // Cara trasera
	for (int i = 0; i < 4; i++) normals.push_back(vec3(1.0f, 0.0f, 0.0f));  // Cara derecha
	for (int i = 0; i < 4; i++) normals.push_back(vec3(-1.0f, 0.0f, 0.0f)); // Cara izquierda
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 1.0f, 0.0f));  // Cara superior
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, -1.0f, 0.0f)); // Cara inferior

	// Texcoords para cada cara (usando el mismo patrón para todas)
	for (int i = 0; i < 6; i++) {
		texCoords.push_back(vec2(0.0f, 0.0f)); // Esquina inferior izquierda
		texCoords.push_back(vec2(1.0f, 0.0f)); // Esquina inferior derecha
		texCoords.push_back(vec2(1.0f, 1.0f)); // Esquina superior derecha
		texCoords.push_back(vec2(0.0f, 1.0f)); // Esquina superior izquierda
	}

	// Colores para cada cara
	vec3 faceColors[6] = {
		vec3(1.0f, 0.5f, 0.5f), // Frontal (Rosa claro)
		vec3(0.5f, 0.5f, 1.0f), // Trasera (Azul claro)
		vec3(1.0f, 0.5f, 0.0f), // Derecha (Naranja)
		vec3(0.0f, 0.5f, 1.0f), // Izquierda (Azul marino)
		vec3(0.5f, 1.0f, 0.5f), // Superior (Verde claro)
		vec3(1.0f, 1.0f, 0.5f)  // Inferior (Amarillo claro)
	};

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			colors.push_back(faceColors[i]);
		}
	}

	// Tangentes y bitangentes para cada cara
	std::vector<vec3> faceTangents = {
		vec3(1.0f, 0.0f, 0.0f),  // Frontal
		vec3(-1.0f, 0.0f, 0.0f), // Trasera
		vec3(0.0f, 0.0f, -1.0f), // Derecha
		vec3(0.0f, 0.0f, 1.0f),  // Izquierda
		vec3(1.0f, 0.0f, 0.0f),  // Superior
		vec3(1.0f, 0.0f, 0.0f)   // Inferior
	};

	std::vector<vec3> faceBitangents = {
		vec3(0.0f, 1.0f, 0.0f),  // Frontal
		vec3(0.0f, 1.0f, 0.0f),  // Trasera
		vec3(0.0f, 1.0f, 0.0f),  // Derecha
		vec3(0.0f, 1.0f, 0.0f),  // Izquierda
		vec3(0.0f, 0.0f, -1.0f), // Superior
		vec3(0.0f, 0.0f, 1.0f)   // Inferior
	};

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			tangents.push_back(faceTangents[i]);
			bitangents.push_back(faceBitangents[i]);
		}
	}

	// Índices para cada cara (2 triángulos por cara)
	for (int i = 0; i < 6; i++) {
		int base = i * 4;
		indices.push_back(base);     // 0
		indices.push_back(base + 1); // 1
		indices.push_back(base + 2); // 2

		indices.push_back(base);     // 0
		indices.push_back(base + 2); // 2
		indices.push_back(base + 3); // 3
	}

	// Asignar los datos al modelo
	model->GetModelData().vertexData = vertices;
	model->GetModelData().indexData = indices;
	model->GetModelData().vertex_normals = normals;
	model->GetModelData().vertex_texCoords = texCoords;
	model->GetModelData().vertex_colors = colors;
	model->GetModelData().vertex_tangents = tangents;
	model->GetModelData().vertex_bitangents = bitangents;

	model->SetMeshName("Cube");

	// Calcular bounding box
	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
	meshBBox->min = vec3(-1.0f, -1.0f, -1.0f);
	meshBBox->max = vec3(1.0f, 1.0f, 1.0f);
	mesh->setBoundingBox(*meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cube");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateCylinder() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	// Parámetros del cilindro
	const float radius = 1.0f;
	const float height = 2.0f;
	const int slices = 20;
	const float halfHeight = height / 2.0f;

	// Vértice central de la base inferior
	Vertex bottomCenter;
	bottomCenter.position = vec3(0.0f, -halfHeight, 0.0f);
	model->GetModelData().vertexData.push_back(bottomCenter);
	model->GetModelData().vertex_texCoords.push_back(vec2(0.5f, 0.5f));
	model->GetModelData().vertex_normals.push_back(vec3(0.0f, -1.0f, 0.0f));
	model->GetModelData().vertex_colors.push_back(vec3(0.2f, 0.2f, 0.8f)); // Azul oscuro
	model->GetModelData().vertex_tangents.push_back(vec3(1.0f, 0.0f, 0.0f));
	model->GetModelData().vertex_bitangents.push_back(vec3(0.0f, 0.0f, -1.0f));

	// Vértices de la base inferior
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		Vertex vertex;
		vertex.position = vec3(x, -halfHeight, z);
		model->GetModelData().vertexData.push_back(vertex);

		// Coordenada de textura
		float u = (cos(angle) + 1.0f) / 2.0f;
		float v = (sin(angle) + 1.0f) / 2.0f;
		model->GetModelData().vertex_texCoords.push_back(vec2(u, v));

		// Normal apuntando hacia abajo
		model->GetModelData().vertex_normals.push_back(vec3(0.0f, -1.0f, 0.0f));

		// Color (azul en la base)
		model->GetModelData().vertex_colors.push_back(vec3(0.2f, 0.2f, 0.8f));

		// Tangente - perpendicular a la normal y apuntando hacia fuera
		vec3 tangent = glm::normalize(vec3(z, 0.0f, -x));
		model->GetModelData().vertex_tangents.push_back(tangent);

		// Bitangente - producto vectorial de normal y tangente
		vec3 normal = vec3(0.0f, -1.0f, 0.0f);
		vec3 bitangent = glm::cross(normal, tangent);
		model->GetModelData().vertex_bitangents.push_back(bitangent);
	}

	// Vértice central de la base superior
	Vertex topCenter;
	topCenter.position = vec3(0.0f, halfHeight, 0.0f);
	model->GetModelData().vertexData.push_back(topCenter);
	model->GetModelData().vertex_texCoords.push_back(vec2(0.5f, 0.5f));
	model->GetModelData().vertex_normals.push_back(vec3(0.0f, 1.0f, 0.0f));
	model->GetModelData().vertex_colors.push_back(vec3(0.8f, 0.2f, 0.2f)); // Rojo oscuro
	model->GetModelData().vertex_tangents.push_back(vec3(1.0f, 0.0f, 0.0f));
	model->GetModelData().vertex_bitangents.push_back(vec3(0.0f, 0.0f, -1.0f));

	// Vértices de la base superior
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		Vertex vertex;
		vertex.position = vec3(x, halfHeight, z);
		model->GetModelData().vertexData.push_back(vertex);

		// Coordenada de textura
		float u = (cos(angle) + 1.0f) / 2.0f;
		float v = (sin(angle) + 1.0f) / 2.0f;
		model->GetModelData().vertex_texCoords.push_back(vec2(u, v));

		// Normal apuntando hacia arriba
		model->GetModelData().vertex_normals.push_back(vec3(0.0f, 1.0f, 0.0f));

		// Color (rojo en la parte superior)
		model->GetModelData().vertex_colors.push_back(vec3(0.8f, 0.2f, 0.2f));

		// Tangente - perpendicular a la normal y apuntando hacia fuera
		vec3 tangent = glm::normalize(vec3(z, 0.0f, -x));
		model->GetModelData().vertex_tangents.push_back(tangent);

		// Bitangente - producto vectorial de normal y tangente
		vec3 normal = vec3(0.0f, 1.0f, 0.0f);
		vec3 bitangent = glm::cross(normal, tangent);
		model->GetModelData().vertex_bitangents.push_back(bitangent);
	}

	// Vértices para el cuerpo del cilindro
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// Vector normal radial (apunta hacia afuera)
		vec3 normal = glm::normalize(vec3(x, 0.0f, z));

		// Vector tangente (alrededor del cilindro)
		vec3 tangent = glm::normalize(vec3(-z, 0.0f, x));

		// Bitangente (a lo largo del eje Y)
		vec3 bitangent = vec3(0.0f, 1.0f, 0.0f);

		// Vértice inferior del cuerpo
		Vertex bottomVertex;
		bottomVertex.position = vec3(x, -halfHeight, z);
		model->GetModelData().vertexData.push_back(bottomVertex);

		// Coordenada de textura
		float u = static_cast<float>(i) / slices;
		model->GetModelData().vertex_texCoords.push_back(vec2(u, 0.0f));

		// Normal apuntando hacia afuera
		model->GetModelData().vertex_normals.push_back(normal);

		// Color (gradiente azul a verde en la parte inferior)
		model->GetModelData().vertex_colors.push_back(vec3(0.2f, 0.5f, 0.7f));

		// Tangente y bitangente
		model->GetModelData().vertex_tangents.push_back(tangent);
		model->GetModelData().vertex_bitangents.push_back(bitangent);

		// Vértice superior del cuerpo
		Vertex topVertex;
		topVertex.position = vec3(x, halfHeight, z);
		model->GetModelData().vertexData.push_back(topVertex);

		// Coordenada de textura
		model->GetModelData().vertex_texCoords.push_back(vec2(u, 1.0f));

		// Normal apuntando hacia afuera
		model->GetModelData().vertex_normals.push_back(normal);

		// Color (gradiente verde a rojo en la parte superior)
		model->GetModelData().vertex_colors.push_back(vec3(0.7f, 0.5f, 0.2f));

		// Tangente y bitangente
		model->GetModelData().vertex_tangents.push_back(tangent);
		model->GetModelData().vertex_bitangents.push_back(bitangent);
	}

	// Índices para la base inferior - CORRECCIÓN
	unsigned int bottomCenterIndex = 0;
	for (int i = 0; i < slices; ++i) {
		unsigned int current = i + 1;
		unsigned int next = (i + 1) % slices + 1;

		// CORRECCIÓN: Volvemos al orden original para la base inferior
		model->GetModelData().indexData.push_back(bottomCenterIndex);
		model->GetModelData().indexData.push_back(current);
		model->GetModelData().indexData.push_back(next);
	}

	// Índices para la base superior
	unsigned int topCenterIndex = slices + 1;
	for (int i = 0; i < slices; ++i) {
		unsigned int current = topCenterIndex + i + 1;
		unsigned int next = topCenterIndex + (i + 1) % slices + 1;

		model->GetModelData().indexData.push_back(topCenterIndex);
		model->GetModelData().indexData.push_back(next);
		model->GetModelData().indexData.push_back(current);
	}

	// Índices para el cuerpo del cilindro
	unsigned int bodyStartIndex = topCenterIndex + slices + 1;
	for (int i = 0; i < slices; ++i) {
		unsigned int bottomLeft = bodyStartIndex + i * 2;
		unsigned int bottomRight = bodyStartIndex + ((i + 1) % slices) * 2;
		unsigned int topLeft = bottomLeft + 1;
		unsigned int topRight = bottomRight + 1;

		// Triangulación del cuerpo (winding order correcto)
		model->GetModelData().indexData.push_back(bottomLeft);
		model->GetModelData().indexData.push_back(topRight);
		model->GetModelData().indexData.push_back(bottomRight);

		model->GetModelData().indexData.push_back(bottomLeft);
		model->GetModelData().indexData.push_back(topLeft);
		model->GetModelData().indexData.push_back(topRight);
	}

	model->SetMeshName("Cylinder");

	// Calcular bounding box
	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
	meshBBox->min = vec3(-radius, -halfHeight, -radius);
	meshBBox->max = vec3(radius, halfHeight, radius);
	mesh->setBoundingBox(*meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cylinder");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateSphere()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	const int stacks = 20;
	const int slices = 20;
	const float radius = 1.0f;

	// Generar vértices para la esfera
	for (int i = 0; i <= stacks; ++i) {
		float phi = i * glm::pi<float>() / stacks;
		float sinPhi = sin(phi);
		float cosPhi = cos(phi);

		for (int j = 0; j <= slices; ++j) {
			float theta = j * 2.0f * glm::pi<float>() / slices;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			// Posición del vértice
			float x = cosTheta * sinPhi;
			float y = cosPhi;
			float z = sinTheta * sinPhi;

			// Crear vértice
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z) * radius;
			model->GetModelData().vertexData.push_back(vertex);

			// Coordenadas de textura
			float u = static_cast<float>(j) / slices;
			float v = static_cast<float>(i) / stacks;
			model->GetModelData().vertex_texCoords.push_back(vec2(u, v));

			// Normal (misma dirección que la posición para una esfera)
			vec3 normal = vec3(x, y, z);
			model->GetModelData().vertex_normals.push_back(normal);

			// Color (gradiente basado en posición)
			vec3 color = vec3(
				(x + 1.0f) * 0.5f,  // R: -1 a 1 mapeado a 0 a 1
				(y + 1.0f) * 0.5f,  // G: -1 a 1 mapeado a 0 a 1
				(z + 1.0f) * 0.5f   // B: -1 a 1 mapeado a 0 a 1
			);
			model->GetModelData().vertex_colors.push_back(color);

			// Calcular tangente
			// Para una esfera, podemos derivar la tangente con respecto a theta
			vec3 tangent = vec3(
				-sinTheta * sinPhi,  // dx/dtheta
				0.0f,                // dy/dtheta
				cosTheta * sinPhi    // dz/dtheta
			);
			// Normalizar la tangente
			if (glm::length(tangent) > 0.0001f) {
				tangent = glm::normalize(tangent);
			}
			else {
				// Evitar vectores muy pequeños
				tangent = vec3(1.0f, 0.0f, 0.0f);
			}
			model->GetModelData().vertex_tangents.push_back(tangent);

			// Calcular bitangente (cross product de normal y tangente)
			vec3 bitangent = glm::cross(normal, tangent);
			model->GetModelData().vertex_bitangents.push_back(bitangent);
		}
	}

	// Generar índices
	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			// Triángulo 1
			model->GetModelData().indexData.push_back(first);
			model->GetModelData().indexData.push_back(second);
			model->GetModelData().indexData.push_back(first + 1);

			// Triángulo 2
			model->GetModelData().indexData.push_back(second);
			model->GetModelData().indexData.push_back(second + 1);
			model->GetModelData().indexData.push_back(first + 1);
		}
	}

	model->SetMeshName("Sphere");

	// Calcular bounding box
	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
	meshBBox->min = vec3(-radius, -radius, -radius);
	meshBBox->max = vec3(radius, radius, radius);
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

	// Vértices del plano (4 vértices)
	model->GetModelData().vertexData = {
		Vertex{vec3(-1.0f, 0.0f, 1.0f)},  // 0: Esquina inferior izquierda
		Vertex{vec3(1.0f, 0.0f, 1.0f)},   // 1: Esquina inferior derecha
		Vertex{vec3(1.0f, 0.0f, -1.0f)},  // 2: Esquina superior derecha
		Vertex{vec3(-1.0f, 0.0f, -1.0f)}  // 3: Esquina superior izquierda
	};

	// Índices para los triángulos (2 triángulos = 6 índices)
	model->GetModelData().indexData = {
		0, 1, 2,  // Primer triángulo
		0, 2, 3   // Segundo triángulo
	};

	// Coordenadas de textura
	model->GetModelData().vertex_texCoords = {
		vec2(0.0f, 0.0f),  // 0: Esquina inferior izquierda
		vec2(1.0f, 0.0f),  // 1: Esquina inferior derecha
		vec2(1.0f, 1.0f),  // 2: Esquina superior derecha
		vec2(0.0f, 1.0f)   // 3: Esquina superior izquierda
	};

	// Normales (todas apuntando hacia arriba)
	model->GetModelData().vertex_normals = {
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	};

	// Colores para los vértices (degradado de blanco a azul)
	model->GetModelData().vertex_colors = {
		vec3(1.0f, 1.0f, 1.0f),  // Blanco
		vec3(0.8f, 0.8f, 1.0f),  // Casi blanco
		vec3(0.4f, 0.4f, 1.0f),  // Azul claro
		vec3(0.0f, 0.0f, 1.0f)   // Azul
	};

	// Tangentes (a lo largo del eje X)
	model->GetModelData().vertex_tangents = {
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f)
	};

	// Bitangentes (a lo largo del eje Z negativo - producto vectorial de normal y tangente)
	model->GetModelData().vertex_bitangents = {
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f)
	};

	model->SetMeshName("Plane");

	// Calcular bounding box
	std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
	meshBBox->min = vec3(-1.0f, 0.0f, -1.0f);
	meshBBox->max = vec3(1.0f, 0.0f, 1.0f);
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
