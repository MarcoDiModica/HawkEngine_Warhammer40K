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
	boneMatrices.reserve(MAX_BONE_MATRICES);

	meshBuffer = CreateStorageBuffer(MAX_MESHES * sizeof(GPUMesh),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	materialBuffer = CreateStorageBuffer(MAX_MATERIALS * sizeof(GPUMaterial),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	instanceBuffer = CreateStorageBuffer(MAX_INSTANCES * sizeof(GPUInstance),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	boneMatricesBuffer = CreateStorageBuffer(MAX_BONE_MATRICES * sizeof(glm::mat4),
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	if (!meshBuffer || !materialBuffer || !instanceBuffer || !boneMatricesBuffer) {
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
	if (boneMatricesBuffer) glDeleteBuffers(1, &boneMatricesBuffer);

	if (fence) {
		glDeleteSync(fence);
		fence = nullptr;
	}

	meshBuffer = 0;
	materialBuffer = 0;
	instanceBuffer = 0;
	boneMatricesBuffer = 0;

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
	boneMatrices.clear();
	
	currentBoneOffset = 0;
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
			LOG(LogType::LOG_WARNING, "Warning: �ndice de malla inv�lido en el mapa: %u", it->second);
		}
	}

	if (meshes.size() >= MAX_MESHES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado l�mite m�ximo de mallas registradas");
		return UINT32_MAX;
	}

	std::shared_ptr<Model> model = mesh->getModel();
	if (!model) {
		LOG(LogType::LOG_ERROR, "Error: La malla no tiene un modelo asociado");
		return UINT32_MAX;
	}

	uint32_t modelID = model->GetID();
	if (modelID == 0) {
		LOG(LogType::LOG_ERROR, "Error: El modelo no tiene un ID v�lido");
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
		LOG(LogType::LOG_ERROR, "Error: Buffers inv�lidos (VAO: %u, IBO: %u, VBO: %u) para malla '%s'",
			modelData.vA, modelData.iBID, modelData.vBPosID, model->GetMeshName().c_str());
		return UINT32_MAX;
	}

	if (!glIsVertexArray(modelData.vA) || !glIsBuffer(modelData.iBID) || !glIsBuffer(modelData.vBPosID)) {
		LOG(LogType::LOG_ERROR, "Error: Buffers GL inv�lidos para malla '%s' (VAO v�lido: %s, IBO v�lido: %s, VBO v�lido: %s)",
			model->GetMeshName().c_str(),
			glIsVertexArray(modelData.vA) ? "s�" : "NO",
			glIsBuffer(modelData.iBID) ? "s�" : "NO",
			glIsBuffer(modelData.vBPosID) ? "s�" : "NO");

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
	hash ^= std::hash<float>{}(material->spriteOffset.x) << 21;
	hash ^= std::hash<float>{}(material->spriteOffset.y) << 22;
	hash ^= std::hash<float>{}(material->spriteSize.x) << 23;
	hash ^= std::hash<float>{}(material->spriteSize.y) << 24;
	hash ^= std::hash<float>{}(material->sheetSize.x) << 25;
	hash ^= std::hash<float>{}(material->sheetSize.y) << 26;

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

	gpuMaterial.spriteOffset = material->spriteOffset;
	gpuMaterial.spriteSize = material->spriteSize;
	gpuMaterial.sheetSize = material->sheetSize;

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
			MarkMaterialDirty(it->second);
		}
		return it->second;
	}

	if (materials.size() >= MAX_MATERIALS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado l�mite m�ximo de materiales registrados");
		return UINT32_MAX;
	}

	GPUMaterial gpuMaterial;
	SetupGPUMaterial(gpuMaterial, material);

	uint32_t index = static_cast<uint32_t>(materials.size());
	materials.push_back(gpuMaterial);
	materialIndices[material] = index;

	materialHashes[material] = CalculateMaterialHash(material);

	MarkMaterialDirty(index);

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
		LOG(LogType::LOG_ERROR, "UpdateMaterial: �ndice de material fuera de rango: %u", materialIndex);
		return false;
	}

	SetupGPUMaterial(materials[materialIndex], material);

	materialHashes[material] = CalculateMaterialHash(material);

	MarkMaterialDirty(materialIndex);

	return true;
}

uint32_t BindlessManager::AddInstance(const GPUInstance& instance) {
	if (instances.size() >= MAX_INSTANCES) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado l�mite m�ximo de instancias registradas");
		return UINT32_MAX;
	}

	if (instance.meshIndex >= meshes.size() || instance.materialIndex >= materials.size()) {
		LOG(LogType::LOG_WARNING, "Warning: �ndices de malla o material inv�lidos");
		return UINT32_MAX;
	}

	uint32_t index = static_cast<uint32_t>(instances.size());
	instances.push_back(instance);

	AddInstanceUpdateRange(index, 1);

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
		LOG(LogType::LOG_ERROR, "CreateTextureHandle: ID de textura %u no es v�lido", textureId);
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
			errorDesc = "GL_OUT_OF_MEMORY - Posible l�mite de GPU alcanzado";
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
	UpdateBuffersIncremental();
}

void BindlessManager::NextFrame() {
	currentFrameIndex = (currentFrameIndex + 1) % FRAME_COUNT;
}

void BindlessManager::EndFrame() {
	if (fence) {
		glDeleteSync(fence);
	}
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void BindlessManager::ClearInstances() {
	instances.clear();
	ResetBoneMatricesPool();
}

GLuint BindlessManager::CreateStorageBuffer(size_t size, GLenum usage) {
	if (size == 0) {
		LOG(LogType::LOG_ERROR, "CreateStorageBuffer: Tama�o de buffer inv�lido (0)");
		return 0;
	}

	GLint maxBufferSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBufferSize);

	if (size > static_cast<size_t>(maxBufferSize)) {
		LOG(LogType::LOG_WARNING, "CreateStorageBuffer: Tama�o solicitado (%zu bytes) excede el m�ximo soportado (%d bytes), ajustando",
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
			errorMsg = "GL_INVALID_VALUE - Par�met	ro inv�lido";
			break;
		case GL_INVALID_OPERATION:
			errorMsg = "GL_INVALID_OPERATION - Operaci�n inv�lida";
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

	// �ndices para el cubo (6 caras, 2 tri�ngulos por cara)
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

uint32_t BindlessManager::AllocateBoneMatrices(uint32_t count) {
	if (currentBoneOffset + count > MAX_BONE_MATRICES) {
		LOG(LogType::LOG_WARNING, "Warning: No hay espacio suficiente para %u matrices de huesos", count);
		return UINT32_MAX;
	}

	uint32_t offset = currentBoneOffset;
	currentBoneOffset += count;

	if (boneMatrices.size() < currentBoneOffset) {
		boneMatrices.resize(currentBoneOffset);
	}

	return offset;
}

void BindlessManager::UpdateBoneMatrices(uint32_t offset, const std::vector<glm::mat4>& matrices) {
	if (offset == UINT32_MAX || offset + matrices.size() > boneMatrices.size()) {
		LOG(LogType::LOG_ERROR, "Error: Offset de matrices de huesos inválido");
		return;
	}

	std::copy(matrices.begin(), matrices.end(), boneMatrices.begin() + offset);

	AddBoneMatrixUpdateRange(offset, matrices.size());
}

void BindlessManager::ResetBoneMatricesPool() {
	currentBoneOffset = 0;
}

void BindlessManager::MarkMaterialDirty(uint32_t index) {
	if (index < materials.size()) {
		dirtyMaterials.insert(index);
		materialBufferDirty = true;
	}
}

void BindlessManager::MarkMeshDirty(uint32_t index) {
	if (index < meshes.size()) {
		dirtyMeshes.insert(index);
		meshBufferDirty = true;
	}
}

void BindlessManager::AddInstanceUpdateRange(uint32_t offset, uint32_t count) {
	for (auto& range : instanceUpdateRanges) {
		if (offset == range.offset + range.count) {
			range.count += count;
			instanceBufferDirty = true;
			return;
		}
		else if (offset + count == range.offset) {
			range.offset = offset;
			range.count += count;
			instanceBufferDirty = true;
			return;
		}
	}

	instanceUpdateRanges.push_back({ offset, count });
	instanceBufferDirty = true;
}

void BindlessManager::AddBoneMatrixUpdateRange(uint32_t offset, uint32_t count) {
	boneMatrixUpdateRanges.push_back({ offset, count });
	boneBufferDirty = true;
}

void BindlessManager::UpdateBuffersIncremental() {
	if (fence) {
		GLenum result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
		if (result == GL_TIMEOUT_EXPIRED) {
			return;
		}
		glDeleteSync(fence);
		fence = nullptr;
	}

	if (meshBufferDirty && !dirtyMeshes.empty()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshBuffer);

		for (uint32_t index : dirtyMeshes) {
			if (index < meshes.size()) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER,
					index * sizeof(GPUMesh),
					sizeof(GPUMesh),
					&meshes[index]);
			}
		}

		dirtyMeshes.clear();
		meshBufferDirty = false;
	}

	if (materialBufferDirty && !dirtyMaterials.empty()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialBuffer);

		for (uint32_t index : dirtyMaterials) {
			if (index < materials.size()) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER,
					index * sizeof(GPUMaterial),
					sizeof(GPUMaterial),
					&materials[index]);
			}
		}

		dirtyMaterials.clear();
		materialBufferDirty = false;
	}

	if (instanceBufferDirty && !instanceUpdateRanges.empty()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceBuffer);

		std::sort(instanceUpdateRanges.begin(), instanceUpdateRanges.end(),
			[](const BufferUpdateRange& a, const BufferUpdateRange& b) {
				return a.offset < b.offset;
			});

		std::vector<BufferUpdateRange> mergedRanges;
		mergedRanges.push_back(instanceUpdateRanges[0]);

		for (size_t i = 1; i < instanceUpdateRanges.size(); i++) {
			auto& last = mergedRanges.back();
			auto& current = instanceUpdateRanges[i];

			if (last.offset + last.count >= current.offset) {
				last.count = std::max(last.offset + last.count, current.offset + current.count) - last.offset;
			}
			else {
				mergedRanges.push_back(current);
			}
		}

		for (const auto& range : mergedRanges) {
			if (range.offset + range.count <= instances.size()) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER,
					range.offset * sizeof(GPUInstance),
					range.count * sizeof(GPUInstance),
					&instances[range.offset]);
			}
		}

		instanceUpdateRanges.clear();
		instanceBufferDirty = false;
	}

	if (boneBufferDirty && !boneMatrixUpdateRanges.empty()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, boneMatricesBuffer);

		for (const auto& range : boneMatrixUpdateRanges) {
			if (range.offset + range.count <= boneMatrices.size()) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER,
					range.offset * sizeof(glm::mat4),
					range.count * sizeof(glm::mat4),
					&boneMatrices[range.offset]);
			}
		}

		boneMatrixUpdateRanges.clear();
		boneBufferDirty = false;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
