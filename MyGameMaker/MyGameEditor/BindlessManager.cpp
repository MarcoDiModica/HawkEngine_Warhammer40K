#include "BindlessManager.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Material.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <functional> 

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

	meshBuffer = CreateStorageBuffer(MAX_MESHES * sizeof(GPUMesh),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	materialBuffer = CreateStorageBuffer(MAX_MATERIALS * sizeof(GPUMaterial),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	instanceBuffer = CreateStorageBuffer(MAX_INSTANCES * sizeof(GPUInstance),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	if (!meshBuffer || !materialBuffer || !instanceBuffer) {
		LOG(LogType::LOG_ERROR, "Error: No se pudieron crear los buffers de almacenamiento");
		Shutdown();
		return false;
	}

	CreateFallbackTexture();
	CreateFallbackCubeMesh();

	LOG(LogType::LOG_INFO, "BindlessManager inicializado con un solo buffer");

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

	if (fence) {
		glDeleteSync(fence);
		fence = nullptr;
	}

	meshBuffer = 0;
	materialBuffer = 0;
	instanceBuffer = 0;

	if (fallbackTextureID) {
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
		LOG(LogType::LOG_ERROR, "Error: Intento de registrar una malla nula");
		return UINT32_MAX;
	}

	auto it = meshIndices.find(mesh);
	if (it != meshIndices.end()) {
		if (it->second < meshes.size()) {
			return it->second;
		}
		else {
			LOG(LogType::LOG_WARNING, "Warning: Índice de malla inválido en el mapa: %u", it->second);
		}
	}

	if (meshes.size() >= MAX_MESHES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de mallas registradas");
		return UINT32_MAX;
	}

	std::shared_ptr<Model> model = mesh->getModel();
	if (!model) {
		LOG(LogType::LOG_ERROR, "Error: La malla no tiene un modelo asociado");
		return UINT32_MAX;
	}

	uint32_t modelID = model->GetID();
	if (modelID == 0) {
		LOG(LogType::LOG_ERROR, "Error: El modelo no tiene un ID válido");
		return UINT32_MAX;
	}

	for (size_t i = 0; i < meshes.size(); ++i) {
		if (meshes[i].meshId == modelID) {
			uint32_t existingIndex = static_cast<uint32_t>(i);

			meshIndices[mesh] = existingIndex;

			return existingIndex;
		}
	}

	const auto& modelData = model->GetModelData();

	if (modelData.vA == 0 || modelData.iBID == 0 || modelData.vBPosID == 0) {
		LOG(LogType::LOG_ERROR, "Error: Buffers inválidos (VAO: %u, IBO: %u, VBO: %u) para malla '%s'",
			modelData.vA, modelData.iBID, modelData.vBPosID, model->GetMeshName().c_str());
		return UINT32_MAX;
	}

	if (!glIsVertexArray(modelData.vA) || !glIsBuffer(modelData.iBID) || !glIsBuffer(modelData.vBPosID)) {
		LOG(LogType::LOG_ERROR, "Error: Buffers GL inválidos para malla '%s' (VAO válido: %s, IBO válido: %s, VBO válido: %s)",
			model->GetMeshName().c_str(),
			glIsVertexArray(modelData.vA) ? "sí" : "NO",
			glIsBuffer(modelData.iBID) ? "sí" : "NO",
			glIsBuffer(modelData.vBPosID) ? "sí" : "NO");

		return GetFallbackMeshIndex();
		//return UINT32_MAX;
	}

	GPUMesh gpuMesh;
	gpuMesh.vertexArray = modelData.vA;
	gpuMesh.indexBuffer = modelData.iBID;
	gpuMesh.positionBuffer = modelData.vBPosID;
	gpuMesh.texCoordBuffer = modelData.vBTCoordsID;
	gpuMesh.normalBuffer = modelData.vBNormalsID;
	gpuMesh.tangentBuffer = modelData.vBTangentsID;
	gpuMesh.bitangentBuffer = modelData.vBBitangentsID;
	gpuMesh.colorBuffer = modelData.vBColorsID;

	gpuMesh.indexCount = modelData.indexData.size();
	gpuMesh.vertexCount = modelData.vertexData.size();
	gpuMesh.meshId = modelID;

	gpuMesh.attributeFlags = 0;
	if (modelData.vBPosID != 0) gpuMesh.attributeFlags |= (1 << 0);  // Position
	if (modelData.vBTCoordsID != 0) gpuMesh.attributeFlags |= (1 << 1);  // TexCoord
	if (modelData.vBNormalsID != 0) gpuMesh.attributeFlags |= (1 << 2);  // Normal
	if (modelData.vBTangentsID != 0) gpuMesh.attributeFlags |= (1 << 3);  // Tangent
	if (modelData.vBBitangentsID != 0) gpuMesh.attributeFlags |= (1 << 4);  // Bitangent
	if (modelData.vBColorsID != 0) gpuMesh.attributeFlags |= (1 << 5);  // Color

	uint32_t index = static_cast<uint32_t>(meshes.size());
	meshes.push_back(gpuMesh);
	meshIndices[mesh] = index;

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
	hash ^= std::hash<float>{}(material->emissiveColor.r) << 14;
	hash ^= std::hash<float>{}(material->emissiveColor.g) << 15;
	hash ^= std::hash<float>{}(material->emissiveColor.b) << 16;
	hash ^= std::hash<float>{}(material->emissiveIntensity) << 17;
	hash ^= std::hash<float>{}(material->heightScale) << 18;

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
	if (material->getEmissiveMap())
		hash ^= std::hash<unsigned int>{}(material->getEmissiveMap()->id()) << 19;
	if (material->getHeightMap())
		hash ^= std::hash<unsigned int>{}(material->getHeightMap()->id()) << 20;

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
		material->tonemapStrength
	);

	gpuMaterial.emissiveParams = glm::vec4(
		material->emissiveColor.r,
		material->emissiveColor.g,
		material->emissiveColor.b,
		material->emissiveIntensity
	);

	gpuMaterial.heightScale = material->heightScale;
	gpuMaterial.padding = 0.0f;

	gpuMaterial.shaderType = static_cast<uint32_t>(material->GetShaderType());

	gpuMaterial.albedoTexture = fallbackTextureHandle.handle;
	gpuMaterial.normalTexture = fallbackTextureHandle.handle;
	gpuMaterial.metallicTexture = fallbackTextureHandle.handle;
	gpuMaterial.roughnessTexture = fallbackTextureHandle.handle;
	gpuMaterial.aoTexture = fallbackTextureHandle.handle;
	gpuMaterial.emissiveTexture = fallbackTextureHandle.handle;
	gpuMaterial.heightTexture = fallbackTextureHandle.handle;

	gpuMaterial.flags = 0;

	bool hasFallbackTextures = false;
	std::vector<std::string> fallbackReasons;

	auto albedoMap = material->getImage();
	if (albedoMap && albedoMap->id() != 0) {
		BindlessHandle handle = CreateTextureHandle(albedoMap->id());
		if (handle.isResident) {
			gpuMaterial.albedoTexture = handle.handle;
			gpuMaterial.flags |= (1 << 0);
		}
		else {
			fallbackReasons.push_back("Albedo: Handle no residente");
			hasFallbackTextures = true;
		}
	}
	else {
		fallbackReasons.push_back("Albedo: No presente");
		hasFallbackTextures = true;
	}

	if (material->GetShaderType() == ShaderType::PBR) {
		// Normal map
		auto normalMap = material->getNormalMap();
		if (normalMap && normalMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(normalMap->id());
			if (handle.isResident) {
				gpuMaterial.normalTexture = handle.handle;
				gpuMaterial.flags |= (1 << 1);
			}
			else {
				fallbackReasons.push_back("Normal: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Normal: No presente");
			hasFallbackTextures = true;
		}

		auto metallicMap = material->getMetallicMap();
		if (metallicMap && metallicMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(metallicMap->id());
			if (handle.isResident) {
				gpuMaterial.metallicTexture = handle.handle;
				gpuMaterial.flags |= (1 << 2);
			}
			else {
				fallbackReasons.push_back("Metallic: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Metallic: No presente");
			hasFallbackTextures = true;
		}

		auto roughnessMap = material->getRoughnessMap();
		if (roughnessMap && roughnessMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(roughnessMap->id());
			if (handle.isResident) {
				gpuMaterial.roughnessTexture = handle.handle;
				gpuMaterial.flags |= (1 << 3);
			}
			else {
				fallbackReasons.push_back("Roughness: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Roughness: No presente");
			hasFallbackTextures = true;
		}

		auto aoMap = material->getAoMap();
		if (aoMap && aoMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(aoMap->id());
			if (handle.isResident) {
				gpuMaterial.aoTexture = handle.handle;
				gpuMaterial.flags |= (1 << 4);
			}
			else {
				fallbackReasons.push_back("AO: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("AO: No presente");
			hasFallbackTextures = true;
		}

		// Emissive map
		auto emissiveMap = material->getEmissiveMap();
		if (emissiveMap && emissiveMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(emissiveMap->id());
			if (handle.isResident) {
				gpuMaterial.emissiveTexture = handle.handle;
				gpuMaterial.flags |= (1 << 5);
			}
			else {
				fallbackReasons.push_back("Emissive: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Emissive: No presente");
			hasFallbackTextures = true;
		}

		// Height map
		auto heightMap = material->getHeightMap();
		if (heightMap && heightMap->id() != 0) {
			BindlessHandle handle = CreateTextureHandle(heightMap->id());
			if (handle.isResident) {
				gpuMaterial.heightTexture = handle.handle;
				gpuMaterial.flags |= (1 << 6);
			}
			else {
				fallbackReasons.push_back("Height: Handle no residente");
				hasFallbackTextures = true;
			}
		}
		else {
			fallbackReasons.push_back("Height: No presente");
			hasFallbackTextures = true;
		}
	}

	if (material->GetShaderType() == ShaderType::PBR) {
		gpuMaterial.flags |= (1 << 16);
	}
	else {
		gpuMaterial.flags &= ~(1 << 16);
	}

	if (hasFallbackTextures) {
		std::string reasons = "";
		for (const auto& reason : fallbackReasons) {
			if (!reasons.empty()) reasons += ", ";
			reasons += reason;
		}
		LOG(LogType::LOG_WARNING, "Material '%p' usando texturas fucsia fallback: %s",
			material, reasons.c_str());
	}
}

uint32_t BindlessManager::RegisterMaterial(const Material* material) {
	if (!material) return UINT32_MAX;

	auto it = materialIndices.find(material);
	if (it != materialIndices.end()) {
		if (HasMaterialChanged(material)) {
			LOG(LogType::LOG_INFO, "Material registrado ha cambiado, actualizando: Idx=%u", it->second);
			UpdateMaterial(material);
		}
		return it->second;
	}

	if (materials.size() >= MAX_MATERIALS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de materiales registrados");
		return UINT32_MAX;
	}

	GPUMaterial gpuMaterial;
	SetupGPUMaterial(gpuMaterial, material);

	uint32_t index = static_cast<uint32_t>(materials.size());
	materials.push_back(gpuMaterial);
	materialIndices[material] = index;

	materialHashes[material] = CalculateMaterialHash(material);

	return index;
}

bool BindlessManager::UpdateMaterial(const Material* material) {
	if (!material) return false;

	auto it = materialIndices.find(material);
	if (it == materialIndices.end()) {
		LOG(LogType::LOG_WARNING, "UpdateMaterial: Material no registrado previamente");
		return false;
	}

	uint32_t materialIndex = it->second;
	if (materialIndex >= materials.size()) {
		LOG(LogType::LOG_ERROR, "UpdateMaterial: Índice de material fuera de rango: %u", materialIndex);
		return false;
	}

	SetupGPUMaterial(materials[materialIndex], material);

	materialHashes[material] = CalculateMaterialHash(material);

	return true;
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
	handle.handle = 0;
	handle.isResident = false;

	if (textureId == 0) {
		LOG(LogType::LOG_WARNING, "CreateTextureHandle: ID de textura es 0, usando fallback");
		return fallbackTextureHandle;
	}

	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: Bindless textures no soportadas");
		return fallbackTextureHandle;
	}

	auto it = textureHandles.find(textureId);
	if (it != textureHandles.end()) {
		GLboolean isResident = glIsTextureHandleResidentARB(it->second.handle);
		if (isResident == GL_TRUE) {
			return it->second;
		}
		else {
			LOG(LogType::LOG_WARNING, "Handle existente para textura %u ya no es residente, recreando", textureId);
			// El handle existe pero no es residente, intentaremos recrearlo
		}
	}

	GLboolean isTexture = glIsTexture(textureId);
	if (isTexture == GL_FALSE) {
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: ID de textura %u no es válido", textureId);
		return fallbackTextureHandle;
	}

	GLint width = 0, height = 0, internalFormat = 0;
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (width == 0 || height == 0) {
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: Textura %u incompleta (W=%d, H=%d, Format=%d)",
			textureId, width, height, internalFormat);
		return fallbackTextureHandle;
	}

	handle.handle = glGetTextureHandleARB(textureId);
	if (handle.handle == 0) {
		GLenum error = glGetError();
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: Error al crear handle para textura %u (Error: 0x%X)",
			textureId, error);
		return fallbackTextureHandle;
	}

	glMakeTextureHandleResidentARB(handle.handle);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: Error al hacer residente el handle %llu para textura %u (Error: 0x%X)",
			handle.handle, textureId, error);

		std::string errorDesc;
		switch (error) {
		case GL_OUT_OF_MEMORY:
			errorDesc = "GL_OUT_OF_MEMORY - Posible límite de GPU alcanzado";
			break;
		case GL_INVALID_OPERATION:
			errorDesc = "GL_INVALID_OPERATION - Posible estado incorrecto de OpenGL";
			break;
		default:
			errorDesc = "Error desconocido";
		}
		LOG(LogType::LOG_ERROR, "Detalles del error: %s", errorDesc.c_str());

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
	if (fence) {
		GLenum result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 16666000); // 16.6ms timeout (60fps)

		if (result == GL_TIMEOUT_EXPIRED) {
			LOG(LogType::LOG_WARNING, "UpdateBuffers: Timeout esperando a que la GPU libere el buffer");
		}

		glDeleteSync(fence);
		fence = nullptr;
	}

	if (!meshes.empty()) {
		size_t requiredSize = meshes.size() * sizeof(GPUMesh);
		void* mappedData = glMapNamedBuffer(meshBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, meshes.data(), requiredSize);
			glUnmapNamedBuffer(meshBuffer);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de mallas (Error: 0x%X)",
				glGetError());
		}
	}

	if (!materials.empty()) {
		size_t requiredSize = materials.size() * sizeof(GPUMaterial);
		void* mappedData = glMapNamedBuffer(materialBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, materials.data(), requiredSize);
			glUnmapNamedBuffer(materialBuffer);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de materiales (Error: 0x%X)",
				glGetError());
		}
	}

	if (!instances.empty()) {
		size_t requiredSize = instances.size() * sizeof(GPUInstance);
		void* mappedData = glMapNamedBuffer(instanceBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, instances.data(), requiredSize);
			glUnmapNamedBuffer(instanceBuffer);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de instancias (Error: 0x%X)",
				glGetError());
		}
	}
}

void BindlessManager::EndFrame() {
	if (fence) {
		glDeleteSync(fence);
	}
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void BindlessManager::ClearInstances() {
	instances.clear();
}

GLuint BindlessManager::CreateStorageBuffer(size_t size, GLenum usage) {
	if (size == 0) {
		LOG(LogType::LOG_ERROR, "CreateStorageBuffer: Tamaño de buffer inválido (0)");
		return 0;
	}

	GLint maxBufferSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBufferSize);

	if (size > static_cast<size_t>(maxBufferSize)) {
		LOG(LogType::LOG_WARNING, "CreateStorageBuffer: Tamaño solicitado (%zu bytes) excede el máximo soportado (%d bytes), ajustando",
			size, maxBufferSize);
		size = static_cast<size_t>(maxBufferSize);
	}

	GLuint buffer = 0;
	glCreateBuffers(1, &buffer);

	if (buffer == 0) {
		LOG(LogType::LOG_ERROR, "CreateStorageBuffer: Fallo al crear el buffer");
		return 0;
	}

	glNamedBufferStorage(buffer, size, nullptr, usage);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string errorMsg = "Desconocido";

		switch (error) {
		case GL_OUT_OF_MEMORY:
			errorMsg = "GL_OUT_OF_MEMORY - No hay memoria disponible";
			break;
		case GL_INVALID_VALUE:
			errorMsg = "GL_INVALID_VALUE - Parámet	ro inválido";
			break;
		case GL_INVALID_OPERATION:
			errorMsg = "GL_INVALID_OPERATION - Operación inválida";
			break;
		}

		LOG(LogType::LOG_ERROR, "CreateStorageBuffer: Error al asignar almacenamiento de %zu bytes (Error: 0x%X - %s)",
			size, error, errorMsg.c_str());

		glDeleteBuffers(1, &buffer);
		return 0;
	}

	return buffer;
}

void BindlessManager::CreateFallbackCubeMesh() {
	const float vertices[] = {
		// Cara frontal
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		// Cara trasera
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	// Normal data for cube
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

	// Texcoords for cube
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

	// Índices para el cubo (6 caras, 2 triángulos por cara)
	const unsigned int indices[] = {
		// Cara frontal
		0, 1, 2,
		2, 3, 0,
		// Cara derecha
		1, 5, 6,
		6, 2, 1,
		// Cara trasera
		7, 6, 5,
		5, 4, 7,
		// Cara izquierda
		4, 0, 3,
		3, 7, 4,
		// Cara superior
		3, 2, 6,
		6, 7, 3,
		// Cara inferior
		4, 5, 1,
		1, 0, 4
	};

	fallbackIndexCount = 36;

	glGenVertexArrays(1, &fallbackVAO);
	glBindVertexArray(fallbackVAO);

	// Position buffer
	glGenBuffers(1, &fallbackVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fallbackVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// Normal buffer 
	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// TexCoord buffer
	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

	// Index buffer
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
	fallbackMesh.tangentBuffer = 0; // No tangents for fallback
	fallbackMesh.bitangentBuffer = 0; // No bitangents for fallback
	fallbackMesh.colorBuffer = 0; // No colors for fallback
	fallbackMesh.indexCount = fallbackIndexCount;
	fallbackMesh.vertexCount = 8;
	fallbackMesh.meshId = UINT32_MAX;
	fallbackMesh.attributeFlags = (1 << 0) | (1 << 1) | (1 << 2); // Has positions, normals, texcoords
}