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

	for (int i = 0; i < 2; i++) {
		meshBuffers[i] = CreateStorageBuffer(MAX_MESHES * sizeof(GPUMesh),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		materialBuffers[i] = CreateStorageBuffer(MAX_MATERIALS * sizeof(GPUMaterial),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		instanceBuffers[i] = CreateStorageBuffer(MAX_INSTANCES * sizeof(GPUInstance),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

		if (!meshBuffers[i] || !materialBuffers[i] || !instanceBuffers[i]) {
			LOG(LogType::LOG_ERROR, "Error: No se pudieron crear los buffers de almacenamiento (set %d)", i);
			Shutdown();
			return false;
		}
	}

	CreateFallbackTexture();

	updateBufferIndex = 0;
	renderBufferIndex = 1;

	LOG(LogType::LOG_INFO, "BindlessManager inicializado con sistema de doble buffer");
	return true;
}

void BindlessManager::Shutdown() {
	for (auto& pair : textureHandles) {
		ReleaseTextureHandle(pair.second);
	}
	textureHandles.clear();

	for (int i = 0; i < 2; i++) {
		if (meshBuffers[i]) glDeleteBuffers(1, &meshBuffers[i]);
		if (materialBuffers[i]) glDeleteBuffers(1, &materialBuffers[i]);
		if (instanceBuffers[i]) glDeleteBuffers(1, &instanceBuffers[i]);

		if (fences[i]) {
			glDeleteSync(fences[i]);
		}
	}

	meshBuffers[0] = meshBuffers[1] = 0;
	materialBuffers[0] = materialBuffers[1] = 0;
	instanceBuffers[0] = instanceBuffers[1] = 0;
	fences[0] = fences[1] = nullptr;

	meshes.clear();
	materials.clear();
	instances.clear();
	meshIndices.clear();
	materialIndices.clear();
}

uint32_t BindlessManager::RegisterMesh(Mesh* mesh) {
	if (!mesh) {
		LOG(LogType::LOG_ERROR, "Error: Intento de registrar una malla nula");
		return UINT32_MAX;
	}

	auto it = meshIndices.find(mesh);
	if (it != meshIndices.end()) {
		return it->second;
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

			LOG(LogType::LOG_INFO, "Malla '%s' ya registrada con ID=%u (Idx=%u), reutilizando",
				model->GetMeshName().c_str(), modelID, existingIndex);

			return existingIndex;
		}
	}

	const auto& modelData = model->GetModelData();

	if (modelData.vA == 0 || modelData.iBID == 0 || modelData.vBPosID == 0) {
		LOG(LogType::LOG_ERROR, "Error: Buffers inválidos (VAO: %u, IBO: %u, VBO: %u) para malla '%s'",
			modelData.vA, modelData.iBID, modelData.vBPosID, model->GetMeshName().c_str());
		return UINT32_MAX;
	}

	GPUMesh gpuMesh;
	gpuMesh.vertexArray = modelData.vA;
	gpuMesh.indexBuffer = modelData.iBID;
	gpuMesh.vertexBuffer = modelData.vBPosID;
	gpuMesh.indexCount = static_cast<uint32_t>(modelData.indexData.size());
	gpuMesh.vertexCount = static_cast<uint32_t>(modelData.vertexData.size());
	gpuMesh.meshId = modelID; 
	gpuMesh.padding = 0;

	uint32_t index = static_cast<uint32_t>(meshes.size());
	meshes.push_back(gpuMesh);
	meshIndices[mesh] = index;

	LOG(LogType::LOG_INFO, "Malla '%s' registrada: Idx=%u, ID=%u, VAO=%u, VBO=%u, IBO=%u, Vértices=%u, Índices=%u",
		model->GetMeshName().c_str(), index, modelID, gpuMesh.vertexArray,
		gpuMesh.vertexBuffer, gpuMesh.indexBuffer, gpuMesh.vertexCount, gpuMesh.indexCount);

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

	if (hasFallbackTextures) {
		std::string reasons = "";
		for (const auto& reason : fallbackReasons) {
			if (!reasons.empty()) reasons += ", ";
			reasons += reason;
		}
		LOG(LogType::LOG_WARNING, "Material '%p' usando texturas fucsia fallback: %s",
			material, reasons.c_str());
	}

	uint32_t index = static_cast<uint32_t>(materials.size());
	materials.push_back(gpuMaterial);
	materialIndices[material] = index;

	LOG(LogType::LOG_INFO, "Material registrado: Idx=%u, Color=(%f,%f,%f,%f), Flags=%u, Fallback=%s",
		index, gpuMaterial.albedoColor.r, gpuMaterial.albedoColor.g,
		gpuMaterial.albedoColor.b, gpuMaterial.albedoColor.a,
		gpuMaterial.flags, hasFallbackTextures ? "Sí" : "No");

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

	LOG(LogType::LOG_INFO, "Handle bindless creado exitosamente para textura %u (Handle: %llu)",
		textureId, handle.handle);

	return handle;
}

void BindlessManager::ReleaseTextureHandle(BindlessHandle& handle) {
	if (handle.handle != 0 && handle.isResident) {
		glMakeTextureHandleNonResidentARB(handle.handle);
		handle.isResident = false;
	}
}

void BindlessManager::UpdateBuffers() {
	if (fences[updateBufferIndex]) {
		GLenum result = glClientWaitSync(fences[updateBufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 10000000); // 10ms timeout

		if (result == GL_TIMEOUT_EXPIRED) {
			LOG(LogType::LOG_WARNING, "UpdateBuffers: Timeout esperando a que la GPU libere el buffer %d", updateBufferIndex);
		}

		glDeleteSync(fences[updateBufferIndex]);
		fences[updateBufferIndex] = nullptr;
	}

	GLuint currentMeshBuffer = meshBuffers[updateBufferIndex];
	GLuint currentMaterialBuffer = materialBuffers[updateBufferIndex];
	GLuint currentInstanceBuffer = instanceBuffers[updateBufferIndex];

	LOG(LogType::LOG_INFO, "UpdateBuffers: Actualizando conjunto de buffers %d (mesh=%u, material=%u, instance=%u)",
		updateBufferIndex, currentMeshBuffer, currentMaterialBuffer, currentInstanceBuffer);

	if (!meshes.empty()) {
		size_t requiredSize = meshes.size() * sizeof(GPUMesh);

		void* mappedData = glMapNamedBuffer(currentMeshBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, meshes.data(), requiredSize);
			glUnmapNamedBuffer(currentMeshBuffer);
			LOG(LogType::LOG_INFO, "UpdateBuffers: Buffer de mallas actualizado (%zu mallas, %zu bytes)",
				meshes.size(), requiredSize);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de mallas %d (Error: 0x%X)",
				updateBufferIndex, glGetError());
		}
	}

	if (!materials.empty()) {
		size_t requiredSize = materials.size() * sizeof(GPUMaterial);

		void* mappedData = glMapNamedBuffer(currentMaterialBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, materials.data(), requiredSize);
			glUnmapNamedBuffer(currentMaterialBuffer);
			LOG(LogType::LOG_INFO, "UpdateBuffers: Buffer de materiales actualizado (%zu materiales, %zu bytes)",
				materials.size(), requiredSize);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de materiales %d (Error: 0x%X)",
				updateBufferIndex, glGetError());
		}
	}

	if (!instances.empty()) {
		size_t requiredSize = instances.size() * sizeof(GPUInstance);

		void* mappedData = glMapNamedBuffer(currentInstanceBuffer, GL_WRITE_ONLY);
		if (mappedData) {
			memcpy(mappedData, instances.data(), requiredSize);
			glUnmapNamedBuffer(currentInstanceBuffer);
			LOG(LogType::LOG_INFO, "UpdateBuffers: Buffer de instancias actualizado (%zu instancias, %zu bytes)",
				instances.size(), requiredSize);
		}
		else {
			LOG(LogType::LOG_ERROR, "UpdateBuffers: Fallo al mapear buffer de instancias %d (Error: 0x%X)",
				updateBufferIndex, glGetError());
		}
	}
}

void BindlessManager::EndFrame() {
	std::swap(updateBufferIndex, renderBufferIndex);

	if (fences[renderBufferIndex]) {
		glDeleteSync(fences[renderBufferIndex]);
	}
	fences[renderBufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	LOG(LogType::LOG_INFO, "EndFrame: Buffers intercambiados - Render: %d, Update: %d",
		renderBufferIndex, updateBufferIndex);
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
			errorMsg = "GL_INVALID_VALUE - Parámetro inválido";
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

	LOG(LogType::LOG_INFO, "CreateStorageBuffer: Buffer creado exitosamente (ID: %u, Tamaño: %zu bytes, Flags: 0x%X)",
		buffer, size, usage);

	return buffer;
}