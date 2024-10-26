#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include <vector>
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

#include "BufferObject.h"

class Mesh {
    BufferObject vertices_buffer;
    BufferObject indices_buffer;
    BufferObject texCoords_buffer;
    BufferObject normals_buffer;
    BufferObject colors_buffer;

    std::vector<glm::vec3> _vertices;
    std::vector<unsigned int> _indices;

public:
    Mesh();
    virtual ~Mesh();

    const auto& vertices() const { return _vertices; }
    const auto& indices() const { return _indices; }

    void Load(const glm::vec3* vertices, size_t num_verts, unsigned int* indices, size_t num_indexs);
    void loadTexCoords(const glm::vec2* texCoords, size_t num_texCoords);
    void LoadNormals(const glm::vec3* normals, size_t num_normals);
    void LoadColors(const glm::u8vec3* colors, size_t num_colors);
    void Draw() const;

    void LoadMesh(const char* file_path);
};

#endif // !__MESH_H__