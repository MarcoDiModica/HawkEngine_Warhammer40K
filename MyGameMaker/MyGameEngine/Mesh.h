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
	template <>
	struct convert<BoundingBox> {
		static Node encode(const BoundingBox& rhs) {
			Node node;
			node["min"] = rhs.min;
			node["max"] = rhs.max;
			return node;
		}
		static bool decode(const Node& node, BoundingBox& rhs) {
			if (!node["min"] || !node["max"])
				return false;
			rhs.min = node["min"].as<vec3>();
			rhs.max = node["max"].as<vec3>();
			return true;
		}
	};
	template <>
    struct convert<ModelData> {
        static Node encode(const ModelData& rhs) {
            Node node;
            node["vBPosID"] = rhs.vBPosID;
            node["vBNormalsID"] = rhs.vBNormalsID;
            node["vBColorsID"] = rhs.vBColorsID;
            node["vBTCoordsID"] = rhs.vBTCoordsID;
            node["vBTangentsID"] = rhs.vBTangentsID;
            node["vBBitangentsID"] = rhs.vBBitangentsID;
            node["iBID"] = rhs.iBID;
            node["vA"] = rhs.vA;
            node["vertexData"] = rhs.vertexData;
            node["indexData"] = rhs.indexData;
            node["vertex_texCoords"] = rhs.vertex_texCoords;
            node["vertex_normals"] = rhs.vertex_normals;
            node["vertex_colors"] = rhs.vertex_colors;
            node["vertex_tangents"] = rhs.vertex_tangents;
            node["vertex_bitangents"] = rhs.vertex_bitangents;
            return node;
        }
        static bool decode(const Node& node, ModelData& rhs) {
            if (!node["vBPosID"] || !node["vBNormalsID"] || !node["vBColorsID"] || !node["vBTCoordsID"] || !node["vBTangentsID"] || !node["vBBitangentsID"] || !node["iBID"] || !node["vA"] || !node["vertexData"] || !node["indexData"] || !node["vertex_texCoords"] || !node["vertex_normals"] || !node["vertex_colors"] || !node["vertex_tangents"] || !node["vertex_bitangents"])
                return false;
            rhs.vBPosID = node["vBPosID"].as<unsigned int>();
            rhs.vBNormalsID = node["vBNormalsID"].as<unsigned int>();
            rhs.vBColorsID = node["vBColorsID"].as<unsigned int>();
            rhs.vBTCoordsID = node["vBTCoordsID"].as<unsigned int>();
            rhs.vBTangentsID = node["vBTangentsID"].as<unsigned int>();
            rhs.vBBitangentsID = node["vBBitangentsID"].as<unsigned int>();
            rhs.iBID = node["iBID"].as<unsigned int>();
            rhs.vA = node["vA"].as<unsigned int>();
            rhs.vertexData = node["vertexData"].as<std::vector<Vertex>>();
            rhs.indexData = node["indexData"].as<std::vector<unsigned int>>();
            rhs.vertex_texCoords = node["vertex_texCoords"].as<std::vector<vec2>>();
            rhs.vertex_normals = node["vertex_normals"].as<std::vector<vec3>>();
            rhs.vertex_colors = node["vertex_colors"].as<std::vector<vec3>>();
            rhs.vertex_tangents = node["vertex_tangents"].as<std::vector<vec3>>();
            rhs.vertex_bitangents = node["vertex_bitangents"].as<std::vector<vec3>>();
            return true;
        }
    };
    template <>
    struct convert<Vertex> {
        static Node encode(const Vertex& rhs) {
            Node node;
            node["position"] = rhs.position;
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                node["boneIDs"].push_back(rhs.m_BoneIDs[i]);
                node["weights"].push_back(rhs.m_Weights[i]);
            }
            return node;
        }
        static bool decode(const Node& node, Vertex& rhs) {
            if (!node["position"] || !node["boneIDs"] || !node["weights"])
                return false;
            rhs.position = node["position"].as<vec3>();
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                rhs.m_BoneIDs[i] = node["boneIDs"][i].as<int>();
                rhs.m_Weights[i] = node["weights"][i].as<float>();
            }
            return true;
        }
    };
}

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
        auto model = getModel();
        node["name"] = model->GetMeshName();
        node["model_data"] = model->GetModelData();
        node["material_index"] = model->GetMaterialIndex();
        node["boundingbox"] = model->getBoundingBox();
        return node;
    }

    bool decode(const YAML::Node& node) {
        Model model;
		setModel(std::make_shared<Model>());
		if (node["name"]) {
			std::string name = node["name"].as<std::string>();
			getModel()->SetMeshName(name);
		}

		if (node["model_data"]) {
			ModelData modelData = node["model_data"].as<ModelData>();
			getModel()->SetModelData(modelData);
		}

        if (node["material_index"]) {
            int materialIndex = node["material_index"].as<int>();
            getModel()->SetMaterialIndex(materialIndex);
        }

		if (node["boundingbox"]) {
			BoundingBox bbox = node["boundingbox"].as<BoundingBox>();
			getModel()->SetBoundingBox(bbox);
		}

        return true;
    }
};

#endif // !__MESH_H__