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
	if (!mesh) {
		LOG(LogType::LOG_ERROR, "Error: Intento de registrar una malla nula");
		return UINT32_MAX;
	}

	auto it = meshIndices.find(mesh);
	if (it != meshIndices.end()) {
		return it->second;
	}

	if (meshes.size() >= MAX_MESHES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de mallas registradas (%zu)", MAX_MESHES);
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

	GPUMesh gpuMesh;
	gpuMesh.vertexArray = modelData.vA;
	gpuMesh.indexBuffer = modelData.iBID;
	gpuMesh.vertexBuffer = modelData.vBPosID;
	gpuMesh.indexCount = static_cast<uint32_t>(modelData.indexData.size());
	gpuMesh.vertexCount = static_cast<uint32_t>(modelData.vertexData.size());
	gpuMesh.meshId = modelID; 
	gpuMesh.padding = 0;

	auto index = static_cast<uint32_t>(meshes.size());
	meshes.push_back(gpuMesh);
	meshIndices[mesh] = index;

	return index;
}

uint32_t BindlessManager::RegisterMaterial(const Material* material) {
	if (!material) {
		LOG(LogType::LOG_ERROR, "Error: Intento de registrar un material nulo");
		return UINT32_MAX;
	}

	auto it = materialIndices.find(material);
	if (it != materialIndices.end()) {
		return it->second;
	}

	uint32_t materialHash = 0;
	materialHash = std::hash<float>{}(material->GetColor().r) ^
		(std::hash<float>{}(material->GetColor().g) << 1) ^
		(std::hash<float>{}(material->GetColor().b) << 2) ^
		(std::hash<float>{}(material->GetColor().a) << 3) ^
		(std::hash<float>{}(material->metallic) << 4) ^
		(std::hash<float>{}(material->roughness) << 5) ^
		(std::hash<float>{}(material->ao) << 6);

	if (material->getImage()) materialHash ^= (material->getImage()->id() << 7);
	if (material->getNormalMap()) materialHash ^= (material->getNormalMap()->id() << 8);
	if (material->getMetallicMap()) materialHash ^= (material->getMetallicMap()->id() << 9);
	if (material->getRoughnessMap()) materialHash ^= (material->getRoughnessMap()->id() << 10);
	if (material->getAoMap()) materialHash ^= (material->getAoMap()->id() << 11);

	for (size_t i = 0; i < materials.size(); ++i) {
		if (glm::all(glm::epsilonEqual(materials[i].albedoColor, material->GetColor(), 0.001f)) &&
			glm::all(glm::epsilonEqual(materials[i].pbrParams,
				glm::vec4(material->metallic, material->roughness, material->ao, 0.0f),
				0.001f))) {

			bool texturesMatch = true;

			if (materials[i].flags == (materials[i].flags & materialHash)) {
				uint32_t existingIndex = static_cast<uint32_t>(i);
				materialIndices[material] = existingIndex;

				LOG(LogType::LOG_INFO, "Material similar encontrado (Idx=%u), reutilizando para '%p'",
					existingIndex, material);

				return existingIndex;
			}
		}
	}

	if (materials.size() >= MAX_MATERIALS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de materiales registrados (%zu)", MAX_MATERIALS);
		return UINT32_MAX;
	}

	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_ERROR, "Error: Intento de registrar material bindless sin soporte en GPU");
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

	if (fallbackTextureHandle.handle == 0 || !fallbackTextureHandle.isResident) {
		LOG(LogType::LOG_ERROR, "Error: Textura fallback no inicializada correctamente");
		CreateFallbackTexture();

		if (fallbackTextureHandle.handle == 0 || !fallbackTextureHandle.isResident) {
			LOG(LogType::LOG_ERROR, "Error fatal: No se pudo crear textura fallback");
			return UINT32_MAX;
		}
	}

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
		GLboolean isTexture = glIsTexture(albedoMap->id());
		if (isTexture == GL_FALSE) {
			fallbackReasons.push_back("Albedo: ID inválido");
			hasFallbackTextures = true;
		}
		else {
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
	}
	else {
		fallbackReasons.push_back("Albedo: No presente");
		hasFallbackTextures = true;
	}

	auto normalMap = material->getNormalMap();
	if (normalMap && normalMap->id() != 0) {
		GLboolean isTexture = glIsTexture(normalMap->id());
		if (isTexture == GL_FALSE) {
			fallbackReasons.push_back("Normal: ID inválido");
			hasFallbackTextures = true;
		}
		else {
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
	}
	else {
		fallbackReasons.push_back("Normal: No presente");
		hasFallbackTextures = true;
	}

	auto metallicMap = material->getMetallicMap();
	if (metallicMap && metallicMap->id() != 0) {
		GLboolean isTexture = glIsTexture(metallicMap->id());
		if (isTexture == GL_FALSE) {
			fallbackReasons.push_back("Metallic: ID inválido");
			hasFallbackTextures = true;
		}
		else {
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
	}
	else {
		fallbackReasons.push_back("Metallic: No presente");
		hasFallbackTextures = true;
	}

	auto roughnessMap = material->getRoughnessMap();
	if (roughnessMap && roughnessMap->id() != 0) {
		GLboolean isTexture = glIsTexture(roughnessMap->id());
		if (isTexture == GL_FALSE) {
			fallbackReasons.push_back("Roughness: ID inválido");
			hasFallbackTextures = true;
		}
		else {
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
	}
	else {
		fallbackReasons.push_back("Roughness: No presente");
		hasFallbackTextures = true;
	}

	auto aoMap = material->getAoMap();
	if (aoMap && aoMap->id() != 0) {
		GLboolean isTexture = glIsTexture(aoMap->id());
		if (isTexture == GL_FALSE) {
			fallbackReasons.push_back("AO: ID inválido");
			hasFallbackTextures = true;
		}
		else {
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

	GLint maxHandles = 0;

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