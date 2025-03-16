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


class SceneSerializer;


class Mesh {

    std::vector<glm::vec3> _normals;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
	std::vector<glm::vec2> _texCoords;

    BoundingBox _boundingBox;
    std::vector<Mesh> subMeshes;
    std::shared_ptr<Model> model;

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
        node["name"] = nameM;

        if (filePath.empty() || meshPath.empty() || nameM.empty()) {
            throw std::runtime_error("Mesh paths or name are not initialized.");
        }

        node["file_path"] = filePath;
        node["mesh_path"] = meshPath;

        if (_vertices.empty() || _indices.empty()) {
            throw std::runtime_error("Mesh vertices or indices are not initialized.");
        }

        node["vertices"] = _vertices;
        node["indices"] = _indices;
        node["tex_coords"] = _texCoords;
        node["normals"] = _normals;

        node["aabb_min"] = aabbMin;
        node["aabb_max"] = aabbMax;

        return node;
    }

    bool decode(const YAML::Node& node) {
        if (!node["file_path"] || !node["mesh_path"] || !node["name"])
            return false;

        filePath = node["file_path"].as<std::string>();
        meshPath = node["mesh_path"].as<std::string>();
        nameM = node["name"].as<std::string>();

        if (node["vertices"])
            _vertices = node["vertices"].as<std::vector<glm::vec3>>();
        if (node["indices"])
            _indices = node["indices"].as<std::vector<unsigned int>>();
        if (node["tex_coords"])
            _texCoords = node["tex_coords"].as<std::vector<glm::vec2>>();
        if (node["normals"])
            _normals = node["normals"].as<std::vector<glm::vec3>>();

        if (node["aabb_min"])
            aabbMin = node["aabb_min"].as<glm::vec3>();
        if (node["aabb_max"])
            aabbMax = node["aabb_max"].as<glm::vec3>();

        return true;
    }
};

#endif // !__MESH_H__