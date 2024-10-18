#include "Mesh.h"
#include "GameObject.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/cimport.h"

#include <assimp/Importer.hpp>
#include <vector>

#include <string>
#include <GL/glew.h>
using namespace std;

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), num_vertices(0), num_index(0) {}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    delete[] vertices;
    delete[] index;
}

void Mesh::LoadMesh(const char* file_path) {
    const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes()) {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            num_vertices = scene->mMeshes[i]->mNumVertices;
            vertices = new float[num_vertices * 3];
            memcpy(vertices, scene->mMeshes[i]->mVertices, sizeof(float) * num_vertices * 3);

            if (scene->mMeshes[i]->HasFaces()) {
                num_index = scene->mMeshes[i]->mNumFaces * 3;
                index = new unsigned int[num_index]; // assume each face is a triangle
                for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j) {
                    memcpy(&index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(unsigned int));
                }
            }

            setupMesh();
        }
        aiReleaseImport(scene);
    }
    else {
        // Handle error
    }
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_index * sizeof(unsigned int), index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Mesh::Draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}