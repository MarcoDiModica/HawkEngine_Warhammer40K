#include "BindlessManager.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Material.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

BindlessManager& BindlessManager::GetInstance() {
	static BindlessManager instance;
	return instance;
}

bool BindlessManager::Initialize() {
	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_INFO, "Bindless textures no soportadas por esta GPU!");
		return false;
	}

	if (!GLEW_ARB_shader_storage_buffer_object) {
		LOG(LogType::LOG_INFO, "Shader Storage Buffer Objects no soportados por esta GPU!");
		return false;
	}

	meshes.reserve(MAX_MESHES);
	materials.reserve(MAX_MATERIALS);
	instances.reserve(MAX_INSTANCES);

	meshBuffer = CreateStorageBuffer(MAX_MESHES * sizeof(GPUMesh), GL_DYNAMIC_STORAGE_BIT);
	materialBuffer = CreateStorageBuffer(MAX_MATERIALS * sizeof(GPUMaterial), GL_DYNAMIC_STORAGE_BIT);
	instanceBuffer = CreateStorageBuffer(MAX_INSTANCES * sizeof(GPUInstance), GL_DYNAMIC_STORAGE_BIT);

	if (!meshBuffer || !materialBuffer || !instanceBuffer) {
		LOG(LogType::LOG_ERROR, "Error: No se pudieron crear los buffers de almacenamiento");
		Shutdown();
		return false;
	}

	CreateFallbackTexture();

	return true;
}

void BindlessManager::Shutdown() {
	for (auto& pair : textureHandles) {
		ReleaseTextureHandle(pair.second);
	}
	textureHandles.clear();

	if (meshBuffer) glDeleteBuffers(1, &meshBuffer);
	if (materialBuffer) glDeleteBuffers(1, &materialBuffer);
	if (instanceBuffer) glDeleteBuffers(1, &instanceBuffer);

	meshBuffer = 0;
	materialBuffer = 0;
	instanceBuffer = 0;

	meshes.clear();
	materials.clear();
	instances.clear();
	meshIndices.clear();
	materialIndices.clear();
}

uint32_t BindlessManager::RegisterMesh(Mesh* mesh) {
	if (!mesh) return UINT32_MAX;

	auto it = meshIndices.find(mesh);
	if (it != meshIndices.end()) {
		return it->second;
	}

	if (meshes.size() >= MAX_MESHES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de mallas registradas");
		return UINT32_MAX;
	}

	const auto& modelData = mesh->getModel()->GetModelData();

	//mirar la clase de ModelData
	GPUMesh gpuMesh;
	gpuMesh.vertexArray = modelData.vA;
	gpuMesh.indexBuffer = modelData.iBID;
	gpuMesh.vertexBuffer = modelData.vBPosID;
	gpuMesh.indexCount = static_cast<uint32_t>(modelData.indexData.size());
	gpuMesh.vertexCount = static_cast<uint32_t>(modelData.vertexData.size());
	gpuMesh.meshId = static_cast<uint32_t>(meshes.size());
	gpuMesh.padding = 0;

	uint32_t index = static_cast<uint32_t>(meshes.size());
	meshes.push_back(gpuMesh);
	meshIndices[mesh] = index;

	return index;
}

uint32_t BindlessManager::RegisterMaterial(const Material* material) {
    if (!material) return UINT32_MAX;

    auto it = materialIndices.find(material);
    if (it != materialIndices.end()) {
        return it->second;
    }

    if (materials.size() >= MAX_MATERIALS) {
        LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de materiales registrados");
        return UINT32_MAX;
    }

    GPUMaterial gpuMaterial;
    gpuMaterial.albedoColor = material->GetColor();
    gpuMaterial.pbrParams = glm::vec4(
        material->metallic,
        material->roughness,
        material->ao,
        0.0f
    );

    gpuMaterial.albedoTexture = fallbackTextureHandle.handle;
    gpuMaterial.normalTexture = fallbackTextureHandle.handle;
    gpuMaterial.metallicTexture = fallbackTextureHandle.handle;
    gpuMaterial.roughnessTexture = fallbackTextureHandle.handle;
    gpuMaterial.aoTexture = fallbackTextureHandle.handle;
    gpuMaterial.emissiveTexture = fallbackTextureHandle.handle;
    
    gpuMaterial.flags = 0;

    bool hasFallbackTextures = false;

    auto albedoMap = material->getImage();
    if (albedoMap && albedoMap->id() != 0) {
        BindlessHandle handle = CreateTextureHandle(albedoMap->id());
        if (handle.isResident) {
            gpuMaterial.albedoTexture = handle.handle;
            gpuMaterial.flags |= (1 << 0); 
        } else {
            hasFallbackTextures = true;
        }
    } else {
        hasFallbackTextures = true;
    }

    auto normalMap = material->getNormalMap();
    if (normalMap && normalMap->id() != 0) {
        BindlessHandle handle = CreateTextureHandle(normalMap->id());
        if (handle.isResident) {
            gpuMaterial.normalTexture = handle.handle;
            gpuMaterial.flags |= (1 << 1);
        } else {
            hasFallbackTextures = true;
        }
    } else {
        hasFallbackTextures = true;
    }

    auto metallicMap = material->getMetallicMap();
    if (metallicMap && metallicMap->id() != 0) {
        BindlessHandle handle = CreateTextureHandle(metallicMap->id());
        if (handle.isResident) {
            gpuMaterial.metallicTexture = handle.handle;
            gpuMaterial.flags |= (1 << 2);
        } else {
            hasFallbackTextures = true;
        }
    } else {
        hasFallbackTextures = true;
    }

    auto roughnessMap = material->getRoughnessMap();
    if (roughnessMap && roughnessMap->id() != 0) {
        BindlessHandle handle = CreateTextureHandle(roughnessMap->id());
        if (handle.isResident) {
            gpuMaterial.roughnessTexture = handle.handle;
            gpuMaterial.flags |= (1 << 3);
        } else {
            hasFallbackTextures = true;
        }
    } else {
        hasFallbackTextures = true;
    }

    auto aoMap = material->getAoMap();
    if (aoMap && aoMap->id() != 0) {
        BindlessHandle handle = CreateTextureHandle(aoMap->id());
        if (handle.isResident) {
            gpuMaterial.aoTexture = handle.handle;
            gpuMaterial.flags |= (1 << 4);
        } else {
            hasFallbackTextures = true;
        }
    } else {
        hasFallbackTextures = true;
    }

    if (hasFallbackTextures) {
        LOG(LogType::LOG_WARNING, "Material '%p' usando texturas fucsia fallback", material);
    }

    uint32_t index = static_cast<uint32_t>(materials.size());
    materials.push_back(gpuMaterial);
    materialIndices[material] = index;

    return index;
}

uint32_t BindlessManager::AddInstance(const GPUInstance& instance) {
	if (instances.size() >= MAX_INSTANCES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de instancias registradas");
		return UINT32_MAX;
	}

	if (instance.meshIndex >= meshes.size() || instance.materialIndex >= materials.size()) {
		LOG(LogType::LOG_WARNING, "Warning: Índices de malla o material inválidos");
		return UINT32_MAX;
	}

	uint32_t index = static_cast<uint32_t>(instances.size());
	instances.push_back(instance);
	return index;
}

GPUMesh* BindlessManager::GetMeshData(uint32_t index) {
	if (index >= meshes.size()) return nullptr;
	return &meshes[index];
}

GPUMaterial* BindlessManager::GetMaterialData(uint32_t index) {
	if (index >= materials.size()) return nullptr;
	return &materials[index];
}

GPUInstance* BindlessManager::GetInstanceData(uint32_t index) {
	if (index >= instances.size()) return nullptr;
	return &instances[index];
}

BindlessHandle BindlessManager::CreateTextureHandle(GLuint textureId) {
	BindlessHandle handle;

	if (textureId == 0) {
		return fallbackTextureHandle;
	}

	auto it = textureHandles.find(textureId);
	if (it != textureHandles.end()) {
		return it->second;
	}

	GLboolean isTexture = glIsTexture(textureId);
	if (isTexture == GL_FALSE) {
		LOG(LogType::LOG_ERROR, "El ID de textura %u no es un objeto de textura válido", textureId);
		return fallbackTextureHandle;
	}

	handle.handle = glGetTextureHandleARB(textureId);

	if (handle.handle == 0) {
		GLenum error = glGetError();
		LOG(LogType::LOG_ERROR, "No se pudo crear handle bindless para textura %u (Error OpenGL: 0x%X)", textureId, error);
		return fallbackTextureHandle;
	}

	glMakeTextureHandleResidentARB(handle.handle);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "Error al hacer residente el handle de textura %u (Error OpenGL: 0x%X)", textureId, error);
		handle.isResident = false;
		return fallbackTextureHandle;
	}

	handle.isResident = true;
	textureHandles[textureId] = handle;

	return handle;
}

void BindlessManager::ReleaseTextureHandle(BindlessHandle& handle) {
	if (handle.handle != 0 && handle.isResident) {
		glMakeTextureHandleNonResidentARB(handle.handle);
		handle.isResident = false;
	}
}

void BindlessManager::UpdateBuffers() {
	if (!meshes.empty()) {
		glNamedBufferSubData(meshBuffer, 0, meshes.size() * sizeof(GPUMesh), meshes.data());
	}

	if (!materials.empty()) {
		glNamedBufferSubData(materialBuffer, 0, materials.size() * sizeof(GPUMaterial), materials.data());
	}

	if (!instances.empty()) {
		glNamedBufferSubData(instanceBuffer, 0, instances.size() * sizeof(GPUInstance), instances.data());
	}
}

void BindlessManager::ClearInstances() {
	instances.clear();
}

GLuint BindlessManager::CreateStorageBuffer(size_t size, GLenum usage) {
	GLuint buffer;
	glCreateBuffers(1, &buffer);
	glNamedBufferStorage(buffer, size, nullptr, usage);
	return buffer;
}