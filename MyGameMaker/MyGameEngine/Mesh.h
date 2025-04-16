#ifndef __MESH_H__
#define __MESH_H__
#pragma once
#ifdef min
#undef min
#endif
#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#ifdef YAML_CPP_DLL_EXPORTS
#define YAML_CPP_API __declspec(dllexport)
#else
#define YAML_CPP_API __declspec(dllimport)
#endif
#include <yaml-cpp/yaml.h>
#include <assimp/scene.h>
#include <MyGameEngine/AssimpGLMHelpers.h>

#include "BufferObject.h"
#include "BoundingBox.h"
#include "Model.h"

namespace YAML {
	template <>
	struct convert<vec2> {
		static Node encode(const vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}
		static bool decode(const Node& node, vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template <>
	struct convert<vec3> {
		static Node encode(const vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}
		static bool decode(const Node& node, vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

class SceneSerializer;


class Mesh {

    std::vector<glm::dvec3> _normals;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
	std::vector<glm::dvec2> _texCoords;

    BoundingBox _boundingBox;
    std::vector<Mesh> subMeshes;
    std::shared_ptr<Model> model;
    size_t id;

public:
    Mesh();
    virtual ~Mesh();

    const auto& vertices() const { return _vertices; }
    const auto& indices() const { return _indices; }
    const auto& boundingBox() const { return _boundingBox; }

    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateSphere();
    static std::shared_ptr<Mesh> CreateCylinder();
    static std::shared_ptr<Mesh> CreatePlane();

    //void Load(const glm::vec3* vertices, size_t num_verts, const unsigned int* indices, size_t num_indexs);
 
    void LoadBones();    
    void CalculateNormals();
    void CalculateTangents();
    void Draw() const;
    static void drawBoundingBox(const BoundingBox& bbox);
    static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);
    inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

    //void LoadMesh(const char* file_path);

    bool drawBoundingbox = true;
    bool drawTriangleNormals = false;
    bool drawVertexNormals = false;
    bool drawFaceNormals = false;
    bool drawWireframe = false;

    glm::vec3 aabbMin;
    glm::vec3 aabbMax;

    void SaveBinary(const std::string& filename) const;
    static std::shared_ptr<Mesh> LoadBinary( std::string& filename);

    void setModel(std::shared_ptr<Model> model) { this->model = model; }
	std::shared_ptr<Model> getModel() { return model; }

    void loadToOpenGL();

	void setBoundingBox(const BoundingBox& bbox) { _boundingBox = bbox; }

protected:
    friend class SceneSerializer;
    friend class MeshRenderer;
    friend class ModelImporter;

    std::string filePath;
    std::string meshPath;
    std::string nameM;

    YAML::Node encode() {
        YAML::Node node;
        std::string name = std::to_string(model->GetID());
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