#include "BindlessManager.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Material.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <functional> 

BindlessManager& BindlessManager::GetInstance() {
	static BindlessManager instance;
	return instance;
}

bool BindlessManager::Initialize() {
	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_ERROR, "Bindless textures not supported by this GPU!");
		return false;
	}

	if (!GLEW_ARB_shader_storage_buffer_object) {
		LOG(LogType::LOG_ERROR, "Shader Storage Buffer Objects not supported by this GPU!");
		return false;
	}

	meshes.reserve(MAX_MESHES);
	materials.reserve(MAX_MATERIALS);
	instances.reserve(MAX_INSTANCES);

	for (int i = 0; i < 2; i++) {
		meshBuffers[i] = CreateStorageBuffer(MAX_MESHES * sizeof(GPUMesh),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

		materialBuffers[i] = CreateStorageBuffer(MAX_MATERIALS * sizeof(GPUMaterial),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

		instanceBuffers[i] = CreateStorageBuffer(MAX_INSTANCES * sizeof(GPUInstance),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

		if (!meshBuffers[i] || !materialBuffers[i] || !instanceBuffers[i]) {
			LOG(LogType::LOG_ERROR, "Failed to create storage buffers (set %d)", i);
			Shutdown();
			return false;
		}
	}

	CreateFallbackTexture();
	CreateFallbackCubeMesh();

	updateBufferIndex = 0;
	renderBufferIndex = 1;

	LOG(LogType::LOG_INFO, "BindlessManager initialized with double-buffering system");
	return true;
}

void BindlessManager::Shutdown() {
	for (auto& pair : textureHandles) {
		ReleaseTextureHandle(pair.second);
	}
	textureHandles.clear();

	for (int i = 0; i < 2; i++) {
		if (meshBuffers[i]) {
			glDeleteBuffers(1, &meshBuffers[i]);
			meshBuffers[i] = 0;
		}

		if (materialBuffers[i]) {
			glDeleteBuffers(1, &materialBuffers[i]);
			materialBuffers[i] = 0;
		}

		if (instanceBuffers[i]) {
			glDeleteBuffers(1, &instanceBuffers[i]);
			instanceBuffers[i] = 0;
		}

		if (fences[i]) {
			glDeleteSync(fences[i]);
			fences[i] = nullptr;
		}
	}

	if (fallbackTextureID) {
		if (fallbackTextureHandle.isResident) {
			glMakeTextureHandleNonResidentARB(fallbackTextureHandle.handle);
		}
		glDeleteTextures(1, &fallbackTextureID);
		fallbackTextureID = 0;
	}

	if (fallbackVAO) {
		glDeleteVertexArrays(1, &fallbackVAO);
		fallbackVAO = 0;
	}

	if (fallbackVBO) {
		glDeleteBuffers(1, &fallbackVBO);
		fallbackVBO = 0;
	}

	if (fallbackIBO) {
		glDeleteBuffers(1, &fallbackIBO);
		fallbackIBO = 0;
	}

	meshes.clear();
	materials.clear();
	instances.clear();
	meshIndices.clear();
	materialIndices.clear();
	materialHashes.clear();
}

uint32_t BindlessManager::RegisterMesh(Mesh* mesh) {
	if (!mesh) {
		LOG(LogType::LOG_ERROR, "Attempted to register a null mesh");
		return UINT32_MAX;
	}

	auto it = meshIndices.find(mesh);
	if (it != meshIndices.end()) {
		return it->second;
	}

	if (meshes.size() >= MAX_MESHES) {
		LOG(LogType::LOG_ERROR, "Maximum number of meshes reached (%zu)", MAX_MESHES);
		return UINT32_MAX;
	}

	std::shared_ptr<Model> model = mesh->getModel();
	if (!model) {
		LOG(LogType::LOG_ERROR, "Mesh has no associated model");
		return UINT32_MAX;
	}

	uint32_t modelID = model->GetID();
	if (modelID == 0) {
		model->GenerateUniqueID();
		modelID = model->GetID();
		LOG(LogType::LOG_INFO, "Generated new ID %u for model", modelID);
	}

	for (size_t i = 0; i < meshes.size(); ++i) {
		if (meshes[i].meshId == modelID) {
			uint32_t existingIndex = static_cast<uint32_t>(i);
			meshIndices[mesh] = existingIndex;

			LOG(LogType::LOG_INFO, "Mesh '%s' with ID=%u already registered (Idx=%u), reusing",
				model->GetMeshName().c_str(), modelID, existingIndex);

			return existingIndex;
		}
	}

	std::string validationError;
	if (!mesh->validate(validationError)) {
		LOG(LogType::LOG_ERROR, "Mesh validation failed: %s", validationError.c_str());

		if (model->GetModelData().vA == 0) {
			mesh->loadToOpenGL();
			LOG(LogType::LOG_INFO, "Attempted to load invalid mesh to OpenGL");
		}
	}

	const auto& modelData = model->GetModelData();

	if (modelData.vA == 0 || modelData.iBID == 0 || modelData.vBPosID == 0) {
		LOG(LogType::LOG_ERROR, "Invalid OpenGL buffers: VAO=%u, IBO=%u, VBO=%u for mesh '%s'",
			modelData.vA, modelData.iBID, modelData.vBPosID, model->GetMeshName().c_str());

		LOG(LogType::LOG_WARNING, "Using fallback mesh for '%s'", model->GetMeshName().c_str());
		return GetFallbackMeshIndex();
	}

	GPUMesh gpuMesh{};
	gpuMesh.vertexArray = modelData.vA;
	gpuMesh.indexBuffer = modelData.iBID;
	gpuMesh.positionBuffer = modelData.vBPosID;
	gpuMesh.texCoordBuffer = modelData.vBTCoordsID;
	gpuMesh.normalBuffer = modelData.vBNormalsID;
	gpuMesh.tangentBuffer = modelData.vBTangentsID;
	gpuMesh.bitangentBuffer = modelData.vBBitangentsID;
	gpuMesh.colorBuffer = modelData.vBColorsID;
	gpuMesh.boneIDBuffer = modelData.vBBoneIDsID;
	gpuMesh.boneWeightBuffer = modelData.vBBoneWeightsID;
	gpuMesh.indexCount = static_cast<uint32_t>(modelData.indexData.size());
	gpuMesh.vertexCount = static_cast<uint32_t>(modelData.vertexData.size());
	gpuMesh.meshId = modelID;

	gpuMesh.attributeFlags = 0;
	if (modelData.vBPosID != 0)           gpuMesh.attributeFlags |= (1 << 0);  // Position
	if (modelData.vBTCoordsID != 0)       gpuMesh.attributeFlags |= (1 << 1);  // TexCoord
	if (modelData.vBNormalsID != 0)       gpuMesh.attributeFlags |= (1 << 2);  // Normal
	if (modelData.vBTangentsID != 0)      gpuMesh.attributeFlags |= (1 << 3);  // Tangent
	if (modelData.vBBitangentsID != 0)    gpuMesh.attributeFlags |= (1 << 4);  // Bitangent
	if (modelData.vBColorsID != 0)        gpuMesh.attributeFlags |= (1 << 5);  // Color
	if (modelData.vBBoneIDsID != 0)       gpuMesh.attributeFlags |= (1 << 6);  // Bone IDs
	if (modelData.vBBoneWeightsID != 0)   gpuMesh.attributeFlags |= (1 << 7);  // Bone Weights
	if (model->isAnimated)                gpuMesh.attributeFlags |= (1 << 8);  // Is Animated

	uint32_t index = static_cast<uint32_t>(meshes.size());
	meshes.push_back(gpuMesh);
	meshIndices[mesh] = index;

	LOG(LogType::LOG_INFO, "Mesh '%s' registered: Idx=%u, ID=%u, VAO=%u, IBO=%u, Attrs=0x%X",
		model->GetMeshName().c_str(), index, modelID, gpuMesh.vertexArray,
		gpuMesh.indexBuffer, gpuMesh.attributeFlags);

	return index;
}

uint64_t CalculateMaterialHash(const Material* material) {
	if (!material) return 0;

	std::size_t hash = 0;

	hash = std::hash<int>{}(static_cast<int>(material->GetShaderType()));

	hash ^= std::hash<float>{}(material->GetColor().r) << 1;
	hash ^= std::hash<float>{}(material->GetColor().g) << 2;
	hash ^= std::hash<float>{}(material->GetColor().b) << 3;
	hash ^= std::hash<float>{}(material->GetColor().a) << 4;

	hash ^= std::hash<float>{}(material->metallic) << 5;
	hash ^= std::hash<float>{}(material->roughness) << 6;
	hash ^= std::hash<float>{}(material->ao) << 7;
	hash ^= std::hash<float>{}(material->tonemapStrength) << 8;

	if (material->getImage())
		hash ^= std::hash<unsigned int>{}(material->getImage()->id()) << 9;
	if (material->getNormalMap())
		hash ^= std::hash<unsigned int>{}(material->getNormalMap()->id()) << 10;
	if (material->getMetallicMap())
		hash ^= std::hash<unsigned int>{}(material->getMetallicMap()->id()) << 11;
	if (material->getRoughnessMap())
		hash ^= std::hash<unsigned int>{}(material->getRoughnessMap()->id()) << 12;
	if (material->getAoMap())
		hash ^= std::hash<unsigned int>{}(material->getAoMap()->id()) << 13;

	return hash;
}

bool BindlessManager::HasMaterialChanged(const Material* material) {
	if (!material) return false;

	auto it = materialHashes.find(material);
	if (it == materialHashes.end()) return true;

	uint64_t currentHash = CalculateMaterialHash(material);
	return currentHash != it->second;
}

void BindlessManager::SetupGPUMaterial(GPUMaterial& gpuMaterial, const Material* material) {
	gpuMaterial.albedoColor = material->GetColor();
	gpuMaterial.pbrParams = glm::vec4(
		material->metallic,
		material->roughness,
		material->ao,
		material->tonemapStrength  // Used as emissive strength for now
	);

	gpuMaterial.shaderType = static_cast<uint32_t>(material->GetShaderType());
	gpuMaterial.flags = 0;

	gpuMaterial.albedoTexture = fallbackTextureHandle.handle;
	gpuMaterial.normalTexture = fallbackTextureHandle.handle;
	gpuMaterial.metallicTexture = fallbackTextureHandle.handle;
	gpuMaterial.roughnessTexture = fallbackTextureHandle.handle;
	gpuMaterial.aoTexture = fallbackTextureHandle.handle;
	gpuMaterial.emissiveTexture = fallbackTextureHandle.handle;

	std::vector<std::string> fallbackReasons;
	bool hasFallbackTextures = false;

	auto albedoMap = material->getImage();
	if (albedoMap && albedoMap->id() != 0) {
		BindlessHandle handle = CreateTextureHandle(albedoMap->id());
		if (handle.isResident) {
			gpuMaterial.albedoTexture = handle.handle;
			gpuMaterial.flags |= (1 << 0); 
		}
		else {
			fallbackReasons.push_back("Albedo: Non-resident handle");
			hasFallbackTextures = true;
		}
	}
	else {
		fallbackReasons.push_back("Albedo: Missing");
		hasFallbackTextures = true;
	}

	if (material->GetShaderType() == ShaderType::PBR) {
		gpuMaterial.flags |= (1 << 16);  // PBR material flag

		// Normal map
		auto normalMap = material->getNormalMap();
		if (normalMap && normalMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(normalMap->id());
			if (handle.isResident) {
				gpuMaterial.normalTexture = handle.handle;
				gpuMaterial.flags |= (1 << 1);  // Normal texture bit
			}
			else {
				fallbackReasons.push_back("Normal: Non-resident handle");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Normal: Missing");
			hasFallbackTextures = true;
		}

		// Metallic map
		auto metallicMap = material->getMetallicMap();
		if (metallicMap && metallicMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(metallicMap->id());
			if (handle.isResident) {
				gpuMaterial.metallicTexture = handle.handle;
				gpuMaterial.flags |= (1 << 2);  // Metallic texture bit
			}
			else {
				fallbackReasons.push_back("Metallic: Non-resident handle");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Metallic: Missing");
			hasFallbackTextures = true;
		}

		// Roughness map
		auto roughnessMap = material->getRoughnessMap();
		if (roughnessMap && roughnessMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(roughnessMap->id());
			if (handle.isResident) {
				gpuMaterial.roughnessTexture = handle.handle;
				gpuMaterial.flags |= (1 << 3);  // Roughness texture bit
			}
			else {
				fallbackReasons.push_back("Roughness: Non-resident handle");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Roughness: Missing");
			hasFallbackTextures = true;
		}

		// AO map
		auto aoMap = material->getAoMap();
		if (aoMap && aoMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(aoMap->id());
			if (handle.isResident) {
				gpuMaterial.aoTexture = handle.handle;
				gpuMaterial.flags |= (1 << 4);  // AO texture bit
			}
			else {
				fallbackReasons.push_back("AO: Non-resident handle");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("AO: Missing");
			hasFallbackTextures = true;
		}
	}

	if (hasFallbackTextures) {
		std::string reasons = "";
		for (const auto& reason : fallbackReasons) {
			if (!reasons.empty()) reasons += ", ";
			reasons += reason;
		}
		LOG(LogType::LOG_WARNING, "Material %p using fallback textures: %s",
			material, reasons.c_str());
	}
}

uint32_t BindlessManager::RegisterMaterial(const Material* material) {
	if (!material) {
		LOG(LogType::LOG_ERROR, "Attempted to register a null material");
		return UINT32_MAX;
	}

	auto it = materialIndices.find(material);
	if (it != materialIndices.end()) {
		if (HasMaterialChanged(material)) {
			LOG(LogType::LOG_INFO, "Material with index %u has changed, updating", it->second);
			UpdateMaterial(material);
		}
		return it->second;
	}

	if (materials.size() >= MAX_MATERIALS) {
		LOG(LogType::LOG_ERROR, "Maximum number of materials reached (%zu)", MAX_MATERIALS);
		return UINT32_MAX;
	}

	GPUMaterial gpuMaterial{};
	SetupGPUMaterial(gpuMaterial, material);

	uint32_t index = static_cast<uint32_t>(materials.size());
	materials.push_back(gpuMaterial);
	materialIndices[material] = index;

	materialHashes[material] = CalculateMaterialHash(material);

	LOG(LogType::LOG_INFO, "Material registered: Idx=%u, Color=(%f,%f,%f,%f), ShaderType=%u, Flags=0x%X",
		index, gpuMaterial.albedoColor.r, gpuMaterial.albedoColor.g,
		gpuMaterial.albedoColor.b, gpuMaterial.albedoColor.a,
		gpuMaterial.shaderType, gpuMaterial.flags);

	return index;
}

bool BindlessManager::UpdateMaterial(const Material* material) {
	if (!material) {
		LOG(LogType::LOG_ERROR, "Attempted to update a null material");
		return false;
	}

	auto it = materialIndices.find(material);
	if (it == materialIndices.end()) {
		LOG(LogType::LOG_WARNING, "Cannot update material: not previously registered");
		return false;
	}

	uint32_t materialIndex = it->second;
	if (materialIndex >= materials.size()) {
		LOG(LogType::LOG_ERROR, "Material index out of range: %u (max: %zu)",
			materialIndex, materials.size() - 1);
		return false;
	}

	SetupGPUMaterial(materials[materialIndex], material);

	materialHashes[material] = CalculateMaterialHash(material);

	LOG(LogType::LOG_INFO, "Material updated: Idx=%u, ShaderType=%u, Flags=0x%X",
		materialIndex, materials[materialIndex].shaderType, materials[materialIndex].flags);

	return true;
}

uint32_t BindlessManager::AddInstance(const GPUInstance& instance) {
	if (instances.size() >= MAX_INSTANCES) {
		LOG(LogType::LOG_ERROR, "Maximum number of instances reached (%zu)", MAX_INSTANCES);
		return UINT32_MAX;
	}

	if (instance.meshIndex >= meshes.size()) {
		LOG(LogType::LOG_ERROR, "Invalid mesh index: %u (max: %zu)",
			instance.meshIndex, meshes.size() - 1);
		return UINT32_MAX;
	}

	if (instance.materialIndex >= materials.size()) {
		LOG(LogType::LOG_ERROR, "Invalid material index: %u (max: %zu)",
			instance.materialIndex, materials.size() - 1);
		return UINT32_MAX;
	}

	uint32_t index = static_cast<uint32_t>(instances.size());
	instances.push_back(instance);

	return index;
}

GPUMesh* BindlessManager::GetMeshData(uint32_t index) {
	if (index >= meshes.size()) {
		LOG(LogType::LOG_ERROR, "Mesh index out of range: %u (max: %zu)",
			index, meshes.size() > 0 ? meshes.size() - 1 : 0);
		return nullptr;
	}
	return &meshes[index];
}

GPUMaterial* BindlessManager::GetMaterialData(uint32_t index) {
	if (index >= materials.size()) {
		LOG(LogType::LOG_ERROR, "Material index out of range: %u (max: %zu)",
			index, materials.size() > 0 ? materials.size() - 1 : 0);
		return nullptr;
	}
	return &materials[index];
}

GPUInstance* BindlessManager::GetInstanceData(uint32_t index) {
	if (index >= instances.size()) {
		LOG(LogType::LOG_ERROR, "Instance index out of range: %u (max: %zu)",
			index, instances.size() > 0 ? instances.size() - 1 : 0);
		return nullptr;
	}
	return &instances[index];
}

BindlessHandle BindlessManager::CreateTextureHandle(GLuint textureId) {
	BindlessHandle handle{};

	if (textureId == 0) {
		LOG(LogType::LOG_WARNING, "Invalid texture ID (0), using fallback");
		return fallbackTextureHandle;
	}

	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_ERROR, "Bindless textures not supported");
		return fallbackTextureHandle;
	}

	auto it = textureHandles.find(textureId);
	if (it != textureHandles.end()) {
		if (glIsTextureHandleResidentARB(it->second.handle) == GL_TRUE) {
			return it->second;
		}
		else {
			LOG(LogType::LOG_WARNING, "Existing handle for texture %u is no longer resident, recreating",
				textureId);
		}
	}

	if (glIsTexture(textureId) == GL_FALSE) {
		LOG(LogType::LOG_ERROR, "Invalid texture ID: %u", textureId);
		return fallbackTextureHandle;
	}

	GLint width = 0, height = 0, internalFormat = 0;
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (width == 0 || height == 0) {
		LOG(LogType::LOG_ERROR, "Incomplete texture: %u (W=%d, H=%d, Format=%d)",
			textureId, width, height, internalFormat);
		return fallbackTextureHandle;
	}

	handle.handle = glGetTextureHandleARB(textureId);
	if (handle.handle == 0) {
		GLenum error = glGetError();
		LOG(LogType::LOG_ERROR, "Failed to create bindless handle for texture %u (Error: 0x%X)",
			textureId, error);
		return fallbackTextureHandle;
	}

	glMakeTextureHandleResidentARB(handle.handle);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "Failed to make handle resident for texture %u (Error: 0x%X)",
			textureId, error);

		std::string errorDesc;
		switch (error) {
		case GL_OUT_OF_MEMORY:
			errorDesc = "GL_OUT_OF_MEMORY - GPU memory limit reached";
			break;
		case GL_INVALID_OPERATION:
			errorDesc = "GL_INVALID_OPERATION - Incorrect OpenGL state";
			break;
		default:
			errorDesc = "Unknown error";
		}
		LOG(LogType::LOG_ERROR, "Error details: %s", errorDesc.c_str());

		return fallbackTextureHandle;
	}

	handle.isResident = true;
	textureHandles[textureId] = handle;

	LOG(LogType::LOG_INFO, "Created bindless handle for texture %u (Handle: %llu)",
		textureId, handle.handle);

	return handle;
}

void BindlessManager::ReleaseTextureHandle(BindlessHandle& handle) {
	if (handle.handle != 0 && handle.isResident) {
		glMakeTextureHandleNonResidentARB(handle.handle);
		handle.isResident = false;
		LOG(LogType::LOG_INFO, "Released bindless handle: %llu", handle.handle);
	}
}

void BindlessManager::UpdateBuffers() {
	if (fences[updateBufferIndex]) {
		GLenum result = glClientWaitSync(fences[updateBufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 100000000); // 100ms timeout

		if (result == GL_TIMEOUT_EXPIRED) {
			LOG(LogType::LOG_ERROR, "Timeout waiting for GPU to release buffer %d", updateBufferIndex);
		}

		glDeleteSync(fences[updateBufferIndex]);
		fences[updateBufferIndex] = nullptr;
	}

	GLuint currentMeshBuffer = meshBuffers[updateBufferIndex];
	GLuint currentMaterialBuffer = materialBuffers[updateBufferIndex];
	GLuint currentInstanceBuffer = instanceBuffers[updateBufferIndex];

	LOG(LogType::LOG_INFO, "Updating buffer set %d (Mesh=%u, Material=%u, Instance=%u)",
		updateBufferIndex, currentMeshBuffer, currentMaterialBuffer, currentInstanceBuffer);

	if (!meshes.empty()) {
		size_t dataSize = meshes.size() * sizeof(GPUMesh);

		void* mappedData = glMapNamedBuffer(currentMeshBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, meshes.data(), dataSize);
			glUnmapNamedBuffer(currentMeshBuffer);
			LOG(LogType::LOG_INFO, "Updated mesh buffer with %zu meshes (%zu bytes)",
				meshes.size(), dataSize);
		}
		else {
			GLenum error = glGetError();
			LOG(LogType::LOG_ERROR, "Failed to map mesh buffer %d (Error: 0x%X)",
				updateBufferIndex, error);
		}
	}

	if (!materials.empty()) {
		size_t dataSize = materials.size() * sizeof(GPUMaterial);

		void* mappedData = glMapNamedBuffer(currentMaterialBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, materials.data(), dataSize);
			glUnmapNamedBuffer(currentMaterialBuffer);
			LOG(LogType::LOG_INFO, "Updated material buffer with %zu materials (%zu bytes)",
				materials.size(), dataSize);
		}
		else {
			GLenum error = glGetError();
			LOG(LogType::LOG_ERROR, "Failed to map material buffer %d (Error: 0x%X)",
				updateBufferIndex, error);
		}
	}

	if (!instances.empty()) {
		size_t dataSize = instances.size() * sizeof(GPUInstance);

		void* mappedData = glMapNamedBuffer(currentInstanceBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, instances.data(), dataSize);
			glUnmapNamedBuffer(currentInstanceBuffer);
			LOG(LogType::LOG_INFO, "Updated instance buffer with %zu instances (%zu bytes)",
				instances.size(), dataSize);
		}
		else {
			GLenum error = glGetError();
			LOG(LogType::LOG_ERROR, "Failed to map instance buffer %d (Error: 0x%X)",
				updateBufferIndex, error);
		}
	}
}

void BindlessManager::EndFrame() {
	std::swap(updateBufferIndex, renderBufferIndex);

	if (fences[renderBufferIndex]) {
		glDeleteSync(fences[renderBufferIndex]);
	}
	fences[renderBufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	LOG(LogType::LOG_INFO, "End frame: Buffer indices swapped - Render: %d, Update: %d",
		renderBufferIndex, updateBufferIndex);
}

void BindlessManager::ClearInstances() {
	instances.clear();
	LOG(LogType::LOG_INFO, "Cleared all instances");
}

GLuint BindlessManager::CreateStorageBuffer(size_t size, GLenum usage) {
	if (size == 0) {
		LOG(LogType::LOG_ERROR, "Invalid buffer size: 0");
		return 0;
	}

	GLint maxBufferSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBufferSize);

	if (size > static_cast<size_t>(maxBufferSize)) {
		LOG(LogType::LOG_WARNING, "Requested buffer size (%zu bytes) exceeds maximum (%d bytes), adjusting",
			size, maxBufferSize);
		size = static_cast<size_t>(maxBufferSize);
	}

	GLuint buffer = 0;
	glCreateBuffers(1, &buffer);

	if (buffer == 0) {
		LOG(LogType::LOG_ERROR, "Failed to create storage buffer");
		return 0;
	}

	glNamedBufferStorage(buffer, size, nullptr, usage);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string errorDesc;
		switch (error) {
		case GL_OUT_OF_MEMORY:
			errorDesc = "GL_OUT_OF_MEMORY - Insufficient memory";
			break;
		case GL_INVALID_VALUE:
			errorDesc = "GL_INVALID_VALUE - Invalid parameter";
			break;
		case GL_INVALID_OPERATION:
			errorDesc = "GL_INVALID_OPERATION - Invalid operation";
			break;
		default:
			errorDesc = "Unknown error";
		}

		LOG(LogType::LOG_ERROR, "Failed to allocate buffer storage: %zu bytes (Error: 0x%X - %s)",
			size, error, errorDesc.c_str());

		glDeleteBuffers(1, &buffer);
		return 0;
	}

	LOG(LogType::LOG_INFO, "Created storage buffer: ID=%u, Size=%zu bytes, Flags=0x%X",
		buffer, size, usage);

	return buffer;
}

void BindlessManager::CreateFallbackTexture() {
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
		LOG(LogType::LOG_INFO, "Created fallback magenta texture (ID: %u, Handle: %llu)",
			fallbackTextureID, fallbackTextureHandle.handle);
	}
	else {
		LOG(LogType::LOG_ERROR, "Failed to create bindless handle for fallback texture");
	}
}

void BindlessManager::CreateFallbackCubeMesh() {
	const float vertices[] = {
		// Front face
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		// Back face
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	const float normals[] = {
		// Front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// Back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f
	};

	const float texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	const unsigned int indices[] = {
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Right face
		1, 5, 6,
		6, 2, 1,
		// Back face
		7, 6, 5,
		5, 4, 7,
		// Left face
		4, 0, 3,
		3, 7, 4,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		4, 5, 1,
		1, 0, 4
	};

	fallbackIndexCount = 36;

	glGenVertexArrays(1, &fallbackVAO);
	glBindVertexArray(fallbackVAO);

	glGenBuffers(1, &fallbackVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fallbackVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);  // Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);  // TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

	glGenBuffers(1, &fallbackIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fallbackIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	fallbackMesh.vertexArray = fallbackVAO;
	fallbackMesh.indexBuffer = fallbackIBO;
	fallbackMesh.positionBuffer = fallbackVBO;
	fallbackMesh.texCoordBuffer = texCoordBuffer;
	fallbackMesh.normalBuffer = normalBuffer;
	fallbackMesh.tangentBuffer = 0;
	fallbackMesh.bitangentBuffer = 0;
	fallbackMesh.colorBuffer = 0;
	fallbackMesh.boneIDBuffer = 0;
	fallbackMesh.boneWeightBuffer = 0;
	fallbackMesh.indexCount = fallbackIndexCount;
	fallbackMesh.vertexCount = 8;
	fallbackMesh.meshId = UINT32_MAX;
	fallbackMesh.attributeFlags = (1 << 0) | (1 << 1) | (1 << 2);  // Position, TexCoord, Normal

	LOG(LogType::LOG_INFO, "Created fallback cube mesh (VAO: %u, IBO: %u)",
		fallbackVAO, fallbackIBO);
}

uint32_t BindlessManager::GetFallbackMeshIndex() {
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

bool BindlessManager::ValidateRenderer(std::string& errorMessage) {
	std::stringstream errors;
	bool isValid = true;

	GLint majorVersion = 0, minorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	if (majorVersion < 4 || (majorVersion == 4 && minorVersion < 5)) {
		errors << "OpenGL version " << majorVersion << "." << minorVersion
			<< " detected. OpenGL 4.5 or higher is required. ";
		isValid = false;
	}

	if (!GLEW_ARB_bindless_texture) {
		errors << "ARB_bindless_texture extension not supported. ";
		isValid = false;
	}

	if (!GLEW_ARB_shader_storage_buffer_object) {
		errors << "ARB_shader_storage_buffer_object extension not supported. ";
		isValid = false;
	}

	if (!GLEW_ARB_indirect_parameters) {
		errors << "ARB_indirect_parameters extension not supported. ";
		isValid = false;
	}

	for (int i = 0; i < 2; i++) {
		if (meshBuffers[i] == 0) {
			errors << "Mesh buffer " << i << " is invalid (0). ";
			isValid = false;
		}

		if (materialBuffers[i] == 0) {
			errors << "Material buffer " << i << " is invalid (0). ";
			isValid = false;
		}

		if (instanceBuffers[i] == 0) {
			errors << "Instance buffer " << i << " is invalid (0). ";
			isValid = false;
		}
	}

	if (meshes.empty()) {
		errors << "No meshes registered. ";
		isValid = false;
	}

	if (materials.empty()) {
		errors << "No materials registered. ";
		isValid = false;
	}

	if (instances.empty()) {
		errors << "No instances to render. ";
		LOG(LogType::LOG_WARNING, "No instances to render");
	}

	if (fallbackTextureID == 0 || !fallbackTextureHandle.isResident) {
		errors << "Fallback texture not properly initialized. ";
		isValid = false;
	}

	if (fallbackVAO == 0 || fallbackIBO == 0) {
		errors << "Fallback mesh not properly initialized. ";
		isValid = false;
	}

	int checkedMaterials = 0;
	int invalidTextures = 0;

	for (size_t i = 0; i < materials.size() && checkedMaterials < 10; i++) {
		const auto& material = materials[i];

		if ((material.flags & (1 << 0)) && material.albedoTexture != 0) {
			if (glIsTextureHandleResidentARB(material.albedoTexture) == GL_FALSE) {
				errors << "Non-resident texture handle detected in material " << i << ". ";
				invalidTextures++;
				isValid = false;
			}
		}

		checkedMaterials++;
	}

	if (invalidTextures > 0) {
		errors << invalidTextures << " non-resident texture handles found in sampled materials. ";
	}

	GLint totalGPUMem = 0, availableGPUMem = 0;
	if (GLEW_NVX_gpu_memory_info) {
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalGPUMem);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableGPUMem);

		float memUsagePercent = 100.0f * (1.0f - (float)availableGPUMem / totalGPUMem);
		if (memUsagePercent > 90.0f) {
			errors << "High GPU memory usage: " << memUsagePercent << "%. ";
			LOG(LogType::LOG_WARNING, "High GPU memory usage: %.1f%%", memUsagePercent);
		}
	}

	/*GLint maxIndirectCount = 0;
	glGetIntegerv(GL_MAX_DRAW_INDIRECT_COUNT, &maxIndirectCount);
	if (instances.size() > (size_t)maxIndirectCount && maxIndirectCount > 0) {
		errors << "Instance count (" << instances.size() << ") exceeds maximum supported by hardware ("
			<< maxIndirectCount << "). ";
		isValid = false;
	}*/

	GLenum glError = glGetError();
	if (glError != GL_NO_ERROR) {
		errors << "OpenGL error detected: 0x" << std::hex << glError << std::dec << ". ";
		isValid = false;
	}

	errorMessage = errors.str();
	if (errorMessage.empty()) {
		errorMessage = "Bindless renderer validation passed.";
	}

	return isValid;
}