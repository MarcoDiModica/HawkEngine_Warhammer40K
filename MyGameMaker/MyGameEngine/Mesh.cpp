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
#include <unordered_map>
#include <unordered_set>
#include "../MyGameEditor/Log.h"

#include <queue>
using namespace std;

Mesh::Mesh() {}

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
	_vertices.assign(vertices, vertices + num_verts);
	_indices.assign(indices, indices + num_indexs);

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

void Mesh::loadTexCoords(const glm::vec2* texCoords, size_t num_texCoords)
{
	texCoords_buffer.LoadData(texCoords, num_texCoords * sizeof(glm::vec2));
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

	normals_buffer.LoadData(_normals.data(), _normals.size() * sizeof(glm::vec3));
}

void Mesh::Draw() const
{
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
}

void Mesh::LoadMesh(const char* file_path)
{
	cout << endl << file_path;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) {
		std::vector<glm::vec3> all_vertices;
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
				
			}
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
			if (mesh->HasNormals()) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
				}
			}

			// Copy colors
			if (mesh->HasVertexColors(0)) {
				for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
					all_colors.push_back(glm::u8vec3(mesh->mColors[0][j].r * 255, mesh->mColors[0][j].g * 255, mesh->mColors[0][j].b * 255));
				}
			}

			vertex_offset += mesh->mNumVertices;
		}

		// Load the combined mesh data
		Load(all_vertices.data(), all_vertices.size(), all_indices.data(), all_indices.size());

		if (!all_texCoords.empty()) {
			loadTexCoords(all_texCoords.data(), all_texCoords.size());
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
		cout << "Error loading mesh: " << file_path << endl;
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
	return mesh;
}