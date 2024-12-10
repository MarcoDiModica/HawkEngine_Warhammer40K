#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include <vector>
#include <GL/glew.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "BufferObject.h"
#include "BoundingBox.h"

class SceneSerializer;

class Mesh {
    BufferObject vertices_buffer;
    BufferObject indices_buffer;
    BufferObject texCoords_buffer;
    BufferObject normals_buffer;
    BufferObject colors_buffer;

    std::vector<glm::vec3> _normals;
    std::vector<glm::vec3> _vertices;
    std::vector<unsigned int> _indices;

    BoundingBox _boundingBox;
    std::vector<Mesh> subMeshes;

public:
    Mesh();
    virtual ~Mesh();

    const auto& vertices() const { return _vertices; }
    const auto& indices() const { return _indices; }
    const auto& boundingBox() const { return _boundingBox; }

    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateSphere();
    static std::shared_ptr<Mesh> CreatePlane();

    void Load(const glm::vec3* vertices, size_t num_verts, const unsigned int* indices, size_t num_indexs);
    void loadTexCoords(const glm::vec2* texCoords, size_t num_texCoords);
    void LoadNormals(const glm::vec3* normals, size_t num_normals);
    void LoadColors(const glm::u8vec3* colors, size_t num_colors);
    void LoadCheckerTexture();
    void CalculateNormals();
    void Draw() const;
    static void drawBoundingBox(const BoundingBox& bbox);
    static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);
    inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

    void LoadMesh(const char* file_path);

    bool drawBoundingbox = true;
    bool drawTriangleNormals = false;
    bool drawVertexNormals = false;
    bool drawFaceNormals = false;
    bool drawWireframe = false;

    glm::vec3 aabbMin;
    glm::vec3 aabbMax;

    YAML::Node Encode() const;
    bool Decode(const YAML::Node& node);

    void Save(const std::string& filename);
    static std::shared_ptr<Mesh> Load(const std::string& filename);

protected:

    friend class MeshRenderer;
    friend class ModelImporter;

    std::string filePath;
    std::string meshPath;
    std::string name;

};

namespace YAML {
    template <>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node
                node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node,
            glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<double>();
            rhs.y = node[1].as<double>();
            rhs.z = node[2].as<double>();
            return true;

        }
    };
}

#endif // !__MESH_H__