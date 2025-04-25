#pragma once

#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Log.h"
#include "../MyGameEngine/Model.h"
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
	uint32_t flags;         // Bit flags for material features
	uint32_t shaderType;    // PBR, UNLIT, etc.
	float padding[2];       // For alignment
};

struct GPUMesh {
	GLuint vertexArray;        // VAO of the mesh
	GLuint indexBuffer;        // Index buffer
	GLuint positionBuffer;     // Position buffer
	GLuint texCoordBuffer;     // Texture coordinate buffer
	GLuint normalBuffer;       // Normal buffer
	GLuint tangentBuffer;      // Tangent buffer
	GLuint bitangentBuffer;    // Bitangent buffer
	GLuint colorBuffer;        // Color buffer
	GLuint boneIDBuffer;       // Bone ID buffer (new)
	GLuint boneWeightBuffer;   // Bone weight buffer (new)
	uint32_t indexCount;       // Number of indices
	uint32_t vertexCount;      // Number of vertices
	uint32_t meshId;           // Unique mesh ID
	uint32_t attributeFlags;   // Flags indicating which attributes are available
	float padding[2];          // For alignment
};

struct GPUInstance {
	glm::mat4 modelMatrix;
	glm::mat4 prevModelMatrix; // For motion blur
	glm::vec4 objectData;      // Additional object data
	uint32_t meshIndex;        // Index to mesh data
	uint32_t materialIndex;    // Index to material data
	uint32_t objectId;         // Unique object ID
	uint32_t flags;            // Configuration flags
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
	void ClearInstances();

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

	uint32_t GetMeshCount() const { return static_cast<uint32_t>(meshes.size()); }
	uint32_t GetMaterialCount() const { return static_cast<uint32_t>(materials.size()); }
	uint32_t GetInstanceCount() const { return static_cast<uint32_t>(instances.size()); }

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

	bool ValidateRenderer(std::string& errorMessage);

	GPUMesh& GetFallbackMesh() { return fallbackMesh; }
	BindlessHandle GetFallbackTextureHandle() const { return fallbackTextureHandle; }

private:
	BindlessManager() = default;
	~BindlessManager() = default;

	BindlessManager(const BindlessManager&) = delete;
	BindlessManager& operator=(const BindlessManager&) = delete;

	void SetupGPUMaterial(GPUMaterial& gpuMaterial, const Material* material);
	GLuint CreateStorageBuffer(size_t size, GLenum usage);
	void CreateFallbackCubeMesh();
	void CreateFallbackTexture();
	uint32_t GetFallbackMeshIndex();

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

	static constexpr size_t MAX_MESHES = 1024;
	static constexpr size_t MAX_MATERIALS = 1024;
	static constexpr size_t MAX_INSTANCES = 10000; 

	GLuint fallbackTextureID = 0;
	BindlessHandle fallbackTextureHandle;
	GLuint fallbackVAO = 0;
	GLuint fallbackVBO = 0;
	GLuint fallbackIBO = 0;
	uint32_t fallbackIndexCount = 0;
	GPUMesh fallbackMesh;
};