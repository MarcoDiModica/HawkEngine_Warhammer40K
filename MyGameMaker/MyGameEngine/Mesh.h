#ifndef __MESH_H__
#define __MESH_H__
#pragma once
#ifdef min
#undef min
#endif
#include <string>
#include <memory>
#include <GL/glew.h>
#include <yaml-cpp/yaml.h>

#include "BoundingBox.h"
#include "Model.h"

#define ATTR_POSITION     0
#define ATTR_TEXCOORD     1
#define ATTR_NORMAL       2
#define ATTR_TANGENT      3
#define ATTR_BITANGENT    4
#define ATTR_BONE_IDS     5
#define ATTR_BONE_WEIGHTS 6
#define ATTR_COLOR        7

class SceneSerializer;

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	const BoundingBox& boundingBox() const { return _boundingBox; }

	static std::shared_ptr<Mesh> CreateCube();
	static std::shared_ptr<Mesh> CreateSphere();
	static std::shared_ptr<Mesh> CreateCylinder();
	static std::shared_ptr<Mesh> CreatePlane();

	void LoadBones();
	void CalculateNormals();
	void CalculateTangents();

	static void drawBoundingBox(const BoundingBox& bbox);
	static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);
	inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

	bool drawBoundingbox = true;
	bool drawTriangleNormals = false;
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
	bool drawWireframe = false;

	glm::vec3 aabbMin;
	glm::vec3 aabbMax;

	void SaveBinary(const std::string& filename) const;
	static std::shared_ptr<Mesh> LoadBinary(std::string& filename);

	void setModel(std::shared_ptr<Model> model) { this->model = model; }
	std::shared_ptr<Model> getModel() const { return model; }

	void loadToOpenGL();
	void releaseOpenGLResources();

	void setBoundingBox(const BoundingBox& bbox) { _boundingBox = bbox; }

	bool validate(std::string& errorMessage) const;

protected:
	friend class SceneSerializer;
	friend class MeshRenderer;
	friend class ModelImporter;

	BoundingBox _boundingBox;
	std::shared_ptr<Model> model;

	std::string filePath;
	std::string meshPath;
	std::string nameM;

	YAML::Node encode() {
		YAML::Node node;
		std::string name = model ? model->GetMeshName() : "unnamed_mesh";
		node["name"] = name;

		SaveBinary(name);
		return node;
	}

	bool decode(const YAML::Node& node) {
		if (!node["name"])
			return false;

		std::string name = node["name"].as<std::string>();
		std::shared_ptr<Mesh> loadedMesh = LoadBinary(name);
		if (!loadedMesh) {
			return false;
		}

		setModel(loadedMesh->getModel());
		setBoundingBox(loadedMesh->boundingBox());

		return true;
	}
};

#endif // !__MESH_H__