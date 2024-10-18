#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include <vector>
#include <GL/glew.h>
#include <string>

using namespace std;


class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    void Draw();
    void LoadMesh(const char* file_path);
	void LoadTexture();

private:
    void setupMesh();


public:
    unsigned int id_index = 0; // index in VRAM
    unsigned int num_index = 0;
    unsigned int* index = nullptr;

    unsigned int id_vertex = 0; // unique vertex in VRAM
    unsigned int num_vertices = 0;
    float* vertices = nullptr;

    bool active;
    bool drawWireframe;
    bool drawChecker;
    unsigned int textureID;
    std::string path;

private:
    unsigned int VAO, VBO, EBO;
  
};

#endif // !__MESH_H__