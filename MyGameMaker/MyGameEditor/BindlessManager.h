#pragma once

#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Log.h"
#include "MyGameEngine/Model.h"
#include "../MyGameEngine/Shaders.h" 

class Mesh;
class Material;
class Texture;

struct BindlessHandle {
	GLuint64 handle = 0;
	bool isResident = false;
};

struct GPUMaterial {
	glm::vec4 albedoColor;
	glm::vec4 pbrParams;    // x: metallic, y: roughness, z: ao, w: emissive
	GLuint64 albedoTexture;
	GLuint64 normalTexture;
	GLuint64 metallicTexture;
	GLuint64 roughnessTexture;
	GLuint64 aoTexture;
	GLuint64 emissiveTexture;
	uint32_t flags;         
	uint32_t shaderType;    // PBR, UNLIT, etc.
	float padding[2];      
};

struct GPUMesh {
	GLuint vertexArray;        // VAO de la malla
	GLuint indexBuffer;        // Índices de la malla
	GLuint positionBuffer;     // Buffer de posiciones
	GLuint texCoordBuffer;     // Buffer de coordenadas de textura
	GLuint normalBuffer;       // Buffer de normales
	GLuint tangentBuffer;      // Buffer de tangentes
	GLuint bitangentBuffer;    // Buffer de bitangentes
	GLuint colorBuffer;        // Buffer de colores
	uint32_t indexCount;       // Número de índices
	uint32_t vertexCount;      // Número de vértices
	uint32_t meshId;           // ID único de la malla
	uint32_t attributeFlags;   // Flags para indicar qué atributos están disponibles
	uint32_t indexOffset; // Corresponds to 'firstIndex'

	uint32_t baseVertexOffset;

	// uint32_t vertexByteOffset;
	// uint32_t indexByteOffset; 

};

struct GPUInstance {
	glm::mat4 modelMatrix;
	glm::mat4 prevModelMatrix; // Para motion blur
	glm::vec4 objectData;      // Datos adicionales del objeto
	uint32_t meshIndex;        // Índice a los datos de malla
	uint32_t materialIndex;    // Índice a los datos de material
	uint32_t objectId;         // ID único del objeto
	uint32_t flags;            // Flags de configuración
};

class BindlessManager {
public:
	static BindlessManager& GetInstance();

	bool Initialize();
	void Shutdown();

	uint32_t RegisterMesh(Mesh* mesh);
	uint32_t RegisterMaterial(const Material* material);

	bool UpdateMaterial(const Material* material);

	uint32_t AddInstance(const GPUInstance& instance);

	GPUMesh* GetMeshData(uint32_t index);
	GPUMaterial* GetMaterialData(uint32_t index);
	GPUInstance* GetInstanceData(uint32_t index);

	BindlessHandle CreateTextureHandle(GLuint textureId);
	void ReleaseTextureHandle(BindlessHandle& handle);

	GLuint GetMeshBuffer() const { return meshBuffers[renderBufferIndex]; }
	GLuint GetMaterialBuffer() const { return materialBuffers[renderBufferIndex]; }
	GLuint GetInstanceBuffer() const { return instanceBuffers[renderBufferIndex]; }

	void UpdateBuffers();
	void EndFrame();
	void ClearInstances();

	uint32_t GetMeshCount() const { return (uint32_t)meshes.size(); }
	uint32_t GetMaterialCount() const { return (uint32_t)materials.size(); }
	uint32_t GetInstanceCount() const { return (uint32_t)instances.size(); }

	GLuint GetIndexBuffer() const { return m_globalIndexBuffer; }
	GLuint GetGlobalVertexBuffer() const { return m_globalVertexBuffer; } 



	bool HasMaterialChanged(const Material* material);

	bool GetTextureIDFromHandle(GLuint64 handle, GLuint& outTextureID) {
		if (handle == 0) {
			return false;
		}

		for (const auto& pair : textureHandles) {
			if (pair.second.handle == handle) {
				outTextureID = pair.first;
				return true;
			}
		}

		if (handle == fallbackTextureHandle.handle) {
			outTextureID = fallbackTextureID;
			return true;
		}

		return false;
	}

private:
	BindlessManager() = default;
	~BindlessManager() = default;

	BindlessManager(const BindlessManager&) = delete;
	BindlessManager& operator=(const BindlessManager&) = delete;

	void SetupGPUMaterial(GPUMaterial& gpuMaterial, const Material* material);

	GLuint CreateStorageBuffer(size_t size, GLenum usage);
	void CreateFallbackCubeMesh();

	GLuint meshBuffers[2] = { 0, 0 };
	GLuint materialBuffers[2] = { 0, 0 };
	GLuint instanceBuffers[2] = { 0, 0 };

	int updateBufferIndex = 0;
	int renderBufferIndex = 1;

	GLsync fences[2] = { nullptr, nullptr };

	std::vector<GPUMesh> meshes;
	std::vector<GPUMaterial> materials;
	std::vector<GPUInstance> instances;

	std::unordered_map<const Mesh*, uint32_t> meshIndices;
	std::unordered_map<const Material*, uint32_t> materialIndices;
	std::unordered_map<GLuint, BindlessHandle> textureHandles;

	std::unordered_map<const Material*, uint64_t> materialHashes;

	GLuint m_globalIndexBuffer = 0;
	GLuint m_globalVertexBuffer = 0; 

	uint32_t m_totalIndices = 0; 
	uint32_t m_totalVertices = 0;

	static constexpr size_t MAX_MESHES = 1024;
	static constexpr size_t MAX_MATERIALS = 1024;
	static constexpr size_t MAX_INSTANCES = 1000;

	GLuint fallbackTextureID = 0;
	BindlessHandle fallbackTextureHandle;

	void CreateFallbackTexture() {
		const unsigned char pixelData[] = {
			255, 0, 255, 255,   255, 0, 255, 255,
			255, 0, 255, 255,   255, 0, 255, 255
		};

		glGenTextures(1, &fallbackTextureID);
		glBindTexture(GL_TEXTURE_2D, fallbackTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		fallbackTextureHandle.handle = glGetTextureHandleARB(fallbackTextureID);
		if (fallbackTextureHandle.handle != 0) {
			glMakeTextureHandleResidentARB(fallbackTextureHandle.handle);
			fallbackTextureHandle.isResident = true;
			LOG(LogType::LOG_INFO, "Textura de fallback fucsia creada correctamente");
		}
		else {
			LOG(LogType::LOG_ERROR, "Error: No se pudo crear handle bindless para textura fallback");
		}
	}

	GLuint fallbackVAO = 0;
	GLuint fallbackVBO = 0;
	GLuint fallbackIBO = 0;
	uint32_t fallbackIndexCount = 0;
	GPUMesh fallbackMesh;

	uint32_t GetFallbackMeshIndex() {
		if (meshes.empty()) {
			meshes.push_back(fallbackMesh);
			return 0;
		}

		for (uint32_t i = 0; i < meshes.size(); i++) {
			if (meshes[i].vertexArray == fallbackVAO) {
				return i;
			}
		}

		uint32_t index = static_cast<uint32_t>(meshes.size());
		meshes.push_back(fallbackMesh);
		return index;
	}

	std::unordered_map<Model*, uint32_t> modelIndices;
};