#ifndef __MESH_H__
#define __MESH_H__
#pragma once

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

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

class SceneSerializer;

struct Vertex
{
    glm::vec3 position;

    int m_BoneIDs[MAX_BONE_INFLUENCE];

    float m_Weights[MAX_BONE_INFLUENCE];
};

struct BoneInfo
{
    int id;

    glm::mat4 offset;

};

class Mesh {
    BufferObject vertices_buffer;
    BufferObject indices_buffer;
    BufferObject texCoords_buffer;
    BufferObject normals_buffer;
    BufferObject colors_buffer;

    std::vector<glm::vec3> _normals;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
	std::vector<glm::vec2> _texCoords;

    std::map<std::string, BoneInfo> m_BoneInfoMap; 
    int m_BoneCounter = 0;

    BoundingBox _boundingBox;
    std::vector<Mesh> subMeshes;

public:
    Mesh();
    virtual ~Mesh();

    const auto& vertices() const { return _vertices; }
    const auto& indices() const { return _indices; }
    const auto& boundingBox() const { return _boundingBox; }

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateSphere();
    static std::shared_ptr<Mesh> CreateCylinder();
    static std::shared_ptr<Mesh> CreatePlane();

    void Load(const glm::vec3* vertices, size_t num_verts, const unsigned int* indices, size_t num_indexs, const int* boneIDs, const float* weights);
    void loadTexCoords(const glm::vec2* texCoords, size_t num_texCoords);
    void LoadNormals(const glm::vec3* normals, size_t num_normals);
    void LoadColors(const glm::u8vec3* colors, size_t num_colors);
    void LoadBones();
    void CalculateNormals();
    void Draw() const;
    static void drawBoundingBox(const BoundingBox& bbox);
    static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);
    inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

    void LoadMesh(const char* file_path);

    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    bool drawBoundingbox = true;
    bool drawTriangleNormals = false;
    bool drawVertexNormals = false;
    bool drawFaceNormals = false;
    bool drawWireframe = false;

    glm::vec3 aabbMin;
    glm::vec3 aabbMax;

    void SaveBinary(const std::string& filename) const;
    static std::shared_ptr<Mesh> LoadBinary( std::string& filename);

protected:
    friend class SceneSerializer;
    friend class MeshRenderer;
    friend class ModelImporter;

    std::string filePath;
    std::string meshPath;
    std::string nameM;

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

            rhs.x = static_cast<float>(node[0].as<double>());
            rhs.y = static_cast<float>(node[1].as<double>());
            rhs.z = static_cast<float>(node[2].as<double>());
            return true;

        }
    };
}

#endif // !__MESH_H__