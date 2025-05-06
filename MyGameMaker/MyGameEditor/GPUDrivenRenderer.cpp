#include "GPUDrivenRenderer.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include "MyGameEngine/ShaderManager.h"
#include "MyGameEngine/CameraComponent.h"

GPUDrivenRenderer& GPUDrivenRenderer::GetInstance() {
	static GPUDrivenRenderer instance;
	return instance;
}

bool GPUDrivenRenderer::Initialize() {
	if (!GLEW_ARB_multi_draw_indirect) {
		LOG(LogType::LOG_INFO, "Multi-draw indirect no soportado por esta GPU! Usando fallback.");
	}

	if (!GLEW_ARB_compute_shader) {
		LOG(LogType::LOG_INFO, "Compute Shaders no soportados por esta GPU!");
		useGPUCulling = false;
	}

	glCreateVertexArrays(1, &defaultVAO);

	glCreateBuffers(1, &drawCommandBuffer);
	glNamedBufferStorage(drawCommandBuffer,
		MAX_DRAW_COMMANDS * sizeof(DrawElementsCommand),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &cullDataBuffer);
	glNamedBufferStorage(cullDataBuffer,
		MAX_DRAW_COMMANDS * sizeof(CullData),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &visibleCountBuffer);
	glNamedBufferStorage(visibleCountBuffer,
		sizeof(GLuint),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	if (useGPUCulling) {
		if (!CompileCullingShader()) {
			LOG(LogType::LOG_INFO, "No se pudo compilar shader de culling, usando CPU fallback");
			useGPUCulling = false;
		}
	}

	return true;
}

void GPUDrivenRenderer::Shutdown() {
	if (cullingShader) glDeleteProgram(cullingShader);
	if (drawCommandBuffer) glDeleteBuffers(1, &drawCommandBuffer);
	if (cullDataBuffer) glDeleteBuffers(1, &cullDataBuffer);
	if (visibleCountBuffer) glDeleteBuffers(1, &visibleCountBuffer);
	if (defaultVAO) glDeleteVertexArrays(1, &defaultVAO);

	cullingShader = 0;
	drawCommandBuffer = 0;
	cullDataBuffer = 0;
	visibleCountBuffer = 0;
	defaultVAO = 0;

	cullData.clear();
	drawCommands.clear();
	shaderBatches.clear();
}

void GPUDrivenRenderer::BeginFrame() {
	cullData.clear();
	drawCommands.clear();
	shaderBatches.clear();
	currentInstanceOffset = 0;
	visibleInstanceCount = 0;

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);
}

void GPUDrivenRenderer::EndFrame() {
	// Implementación futura si es necesario
}

void GPUDrivenRenderer::AddInstanceGroup(
	uint32_t meshIndex,
	uint32_t materialIndex,
	const glm::vec4& boundingSphere,
	const std::vector<GPUInstance>& instances) {

	if (instances.empty() || meshIndex >= BindlessManager::GetInstance().GetMeshCount()) {
		return;
	}

	if (materialIndex >= BindlessManager::GetInstance().GetMaterialCount()) {
		LOG(LogType::LOG_WARNING, "Warning: Índice de material inválido: %u", materialIndex);
		return;
	}

	if (cullData.size() >= MAX_DRAW_COMMANDS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de comandos de dibujo");
		return;
	}

	CullData cullItem;
	cullItem.boundingSphere = boundingSphere;
	cullItem.drawID = static_cast<uint32_t>(cullData.size());
	cullItem.meshIndex = meshIndex;
	cullItem.materialIndex = materialIndex;
	cullItem.instanceOffset = currentInstanceOffset;
	cullItem.instanceCount = static_cast<uint32_t>(instances.size());

	for (const auto& instance : instances) {
		BindlessManager::GetInstance().AddInstance(instance);
	}

	currentInstanceOffset += static_cast<uint32_t>(instances.size());
	cullData.push_back(cullItem);
}

void GPUDrivenRenderer::PrepareDrawCommands(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos) {
	if (cullData.empty()) {
		LOG(LogType::LOG_INFO, "No hay datos de culling para procesar");
		return;
	}

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	glNamedBufferSubData(cullDataBuffer, 0,
		cullData.size() * sizeof(CullData), cullData.data());

	if (!enableCulling) { //3r
		ForceIncludeAllObjects();
	}
	else if (!useGPUCulling) { //2nd
		CPUFrustumCulling();
	}
	else { //1r
		glUseProgram(cullingShader);
		SetCullingUniforms(viewMatrix, projMatrix, cameraPos);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cullDataBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, drawCommandBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visibleCountBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BindlessManager::GetInstance().GetMeshBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, BindlessManager::GetInstance().GetInstanceBuffer());

		GLuint numGroups = (cullData.size() + 63) / 64;

		glDispatchCompute(numGroups, 1, 1);

		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glGetNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &visibleInstanceCount);

		drawCommands.resize(cullData.size());
		glGetNamedBufferSubData(drawCommandBuffer, 0,
			cullData.size() * sizeof(DrawElementsCommand), drawCommands.data());

		LOG(LogType::LOG_INFO, "Draw commands generados: %zu", drawCommands.size());
		if (!drawCommands.empty()) {
			LOG(LogType::LOG_INFO, "Primer draw command: count=%u, instanceCount=%u",
				drawCommands[0].count, drawCommands[0].instanceCount);
		}
	}

	BatchCommandsByShaderType();
}

void GPUDrivenRenderer::ForceIncludeAllObjects() {
	drawCommands.clear();
	visibleInstanceCount = 0;

	for (const auto& cullItem : cullData) {
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
		if (!meshData) continue;

		DrawElementsCommand command;
		command.count = meshData->indexCount;
		command.instanceCount = cullItem.instanceCount;
		command.firstIndex = 0;
		command.baseVertex = 0;
		command.baseInstance = cullItem.instanceOffset;

		drawCommands.push_back(command);
		visibleInstanceCount += cullItem.instanceCount;
	}

	if (!drawCommands.empty()) {
		glNamedBufferSubData(drawCommandBuffer, 0,
			drawCommands.size() * sizeof(DrawElementsCommand),
			drawCommands.data());
	}

	LOG(LogType::LOG_INFO, "Culling desactivado: Incluyendo todos los objetos (%zu)", drawCommands.size());
}

void GPUDrivenRenderer::SetCullingUniforms(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos) {
	glm::mat4 atrViewMatrix = viewMatrix;
	glm::mat4 atrProjMatrix = projMatrix;
	glm::vec3 atrCameraPos = cameraPos;

	glUniformMatrix4fv(glGetUniformLocation(cullingShader, "u_viewMatrix"),
		1, GL_FALSE, glm::value_ptr(atrViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(cullingShader, "u_projMatrix"),
		1, GL_FALSE, glm::value_ptr(atrProjMatrix));
	glUniform3fv(glGetUniformLocation(cullingShader, "u_cameraPosition"),
		1, glm::value_ptr(atrCameraPos));
	glUniform1i(glGetUniformLocation(cullingShader, "u_useFrustumCulling"),
		useFrustumCulling ? 1 : 0);
	glUniform1i(glGetUniformLocation(cullingShader, "u_useOcclusionCulling"),
		useOcclusionCulling ? 1 : 0);
	glUniform1f(glGetUniformLocation(cullingShader, "u_maxDrawDistance"),
		1000.0f);

	if (useFrustumCulling) {
		SetFrustumPlanes(viewMatrix, projMatrix);
	}
}

void GPUDrivenRenderer::CPUFrustumCulling() {
	drawCommands.clear();
	visibleInstanceCount = 0;

	for (const auto& cullItem : cullData) {
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
		if (!meshData) continue;

		DrawElementsCommand command;
		command.count = meshData->indexCount;
		command.instanceCount = cullItem.instanceCount;
		command.firstIndex = 0;
		command.baseVertex = 0;
		command.baseInstance = cullItem.instanceOffset;

		drawCommands.push_back(command);
		visibleInstanceCount += cullItem.instanceCount;
	}

	if (!drawCommands.empty()) {
		glNamedBufferSubData(drawCommandBuffer, 0,
			drawCommands.size() * sizeof(DrawElementsCommand),
			drawCommands.data());
	}
}

void GPUDrivenRenderer::SetFrustumPlanes(const glm::mat4& view, const glm::mat4& proj) {
	glm::mat4 vp = proj * view;

	glm::vec4 frustumPlanes[6];

	// Left plane
	frustumPlanes[0].x = vp[0][3] + vp[0][0];
	frustumPlanes[0].y = vp[1][3] + vp[1][0];
	frustumPlanes[0].z = vp[2][3] + vp[2][0];
	frustumPlanes[0].w = vp[3][3] + vp[3][0];

	// Right plane
	frustumPlanes[1].x = vp[0][3] - vp[0][0];
	frustumPlanes[1].y = vp[1][3] - vp[1][0];
	frustumPlanes[1].z = vp[2][3] - vp[2][0];
	frustumPlanes[1].w = vp[3][3] - vp[3][0];

	// Bottom plane
	frustumPlanes[2].x = vp[0][3] + vp[0][1];
	frustumPlanes[2].y = vp[1][3] + vp[1][1];
	frustumPlanes[2].z = vp[2][3] + vp[2][1];
	frustumPlanes[2].w = vp[3][3] + vp[3][1];

	// Top plane
	frustumPlanes[3].x = vp[0][3] - vp[0][1];
	frustumPlanes[3].y = vp[1][3] - vp[1][1];
	frustumPlanes[3].z = vp[2][3] - vp[2][1];
	frustumPlanes[3].w = vp[3][3] - vp[3][1];

	// Near plane
	frustumPlanes[4].x = vp[0][3] + vp[0][2];
	frustumPlanes[4].y = vp[1][3] + vp[1][2];
	frustumPlanes[4].z = vp[2][3] + vp[2][2];
	frustumPlanes[4].w = vp[3][3] + vp[3][2];

	// Far plane
	frustumPlanes[5].x = vp[0][3] - vp[0][2];
	frustumPlanes[5].y = vp[1][3] - vp[1][2];
	frustumPlanes[5].z = vp[2][3] - vp[2][2];
	frustumPlanes[5].w = vp[3][3] - vp[3][2];

	// Normalizar planos
	for (auto & frustumPlane : frustumPlanes) {
		float length = glm::length(glm::vec3(frustumPlane));
		frustumPlane /= length;
	}

	glUniform4fv(glGetUniformLocation(cullingShader, "u_frustum.planes"),
		6, glm::value_ptr(frustumPlanes[0]));
}

void GPUDrivenRenderer::BatchCommandsByShaderType() {
	shaderBatches.clear();

	for (size_t i = 0; i < cullData.size(); i++) {
		const CullData& cullItem = cullData[i];

		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(cullItem.materialIndex);
		if (!materialData) {
			LOG(LogType::LOG_WARNING, "Material inválido en índice %u, omitiendo", cullItem.materialIndex);
			continue;
		}

		ShaderType shaderType = static_cast<ShaderType>(materialData->shaderType);

		ShaderBatch& batch = shaderBatches[shaderType];
		batch.shaderType = shaderType;

		if (i < drawCommands.size()) {
			batch.commands.push_back(drawCommands[i]);
			batch.meshIndices.push_back(cullItem.meshIndex);
			batch.materialIndices.push_back(cullItem.materialIndex);
		}
	}

	for (const auto& [type, batch] : shaderBatches) {
		std::string shaderName;
		switch (type) {
		case ShaderType::PBR: shaderName = "PBR"; break;
		case ShaderType::UNLIT: shaderName = "UNLIT"; break;
		default: shaderName = "DESCONOCIDO";
		}
	}
}

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos) {
	if (shaderBatches.empty()) {
		LOG(LogType::LOG_INFO, "No hay objetos para renderizar");
		return;
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

	for (const auto& [shaderType, batch] : shaderBatches) {
		switch (shaderType) {
		case ShaderType::UNLIT:
			RenderUnlitBatch(batch, viewMatrix, projMatrix);
			break;
		case ShaderType::PBR:
			// Implementar renderizado PBR
			LOG(LogType::LOG_INFO, "Renderizado PBR no implementado aún");
			break;
		default:
			LOG(LogType::LOG_WARNING, "Tipo de shader desconocido: %d", (int)shaderType);
			break;
		}
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void GPUDrivenRenderer::RenderUnlitBatch(
	const ShaderBatch& batch,
	const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix) {

	if (batch.commands.empty()) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "No se pudo obtener el shader UNLIT");
		return;
	}

	shader->Bind();
	shader->SetUniformMat4("view", viewMatrix);
	shader->SetUniformMat4("projection", projMatrix);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BindlessManager::GetInstance().GetInstanceBuffer());

	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);

		if (!meshData || !materialData) continue;

		shader->SetUniformVec4("albedoColor", materialData->albedoColor);

		if (materialData->flags & (1 << 0)) {
			shader->SetUniform("u_HasTexture", 1);
			if (GLEW_ARB_bindless_texture && GLEW_ARB_gpu_shader_int64 && !intelGPU) {
				GLuint64 textureHandle = materialData->albedoTexture;
				if (textureHandle != 0) {
					shader->SetUniform("albedoTexture", textureHandle);
				}
			}
			else {
				GLuint textureID = 0;
				if (BindlessManager::GetInstance().GetTextureIDFromHandle(
					materialData->albedoTexture, textureID)) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textureID);
					shader->SetUniform("albedoTexture", 0);
				}
				else {
					LOG(LogType::LOG_ERROR, "Error al obtener ID de textura de handle: %u", materialData->albedoTexture);
				}
			}
		}
		else {
			shader->SetUniform("u_HasTexture", 0);
		}

		glBindVertexArray(meshData->vertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		if (i < batch.commands.size()) {
			const DrawElementsCommand& cmd = batch.commands[i];

			shader->SetUniform("instanceOffset", (int)cmd.baseInstance);

			glDrawElementsInstanced(
				GL_TRIANGLES,
				cmd.count,
				GL_UNSIGNED_INT,
				nullptr,
				cmd.instanceCount
			);

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				LOG(LogType::LOG_ERROR, "GL Error after draw: 0x%X", err);
			}
		}
	}

	glBindVertexArray(0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	shader->UnBind();
}

bool GPUDrivenRenderer::CompileCullingShader() {
	if (!GLEW_ARB_compute_shader) {
		LOG(LogType::LOG_ERROR, "Compute shaders no soportados, no se puede compilar el shader de culling");
		return false;
	}

	cullingShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::CULLING_COMPUTE);
	if (cullingShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo obtener el programa de shader de culling");
		return false;
	}

	LOG(LogType::LOG_INFO, "Shader de culling compilado exitosamente (ID: %u)", cullingShader);
	return true;
}

void GPUDrivenRenderer::DebugMeshInfo(uint32_t meshIndex) {
	GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
	if (!meshData) {
		LOG(LogType::LOG_ERROR, "DebugMeshInfo: Mesh %u no encontrado", meshIndex);
		return;
	}

	LOG(LogType::LOG_INFO, "=== Información detallada de GPUMesh %u ===", meshIndex);

	// Verificar IDs de buffer
	LOG(LogType::LOG_INFO, "VAO: %u (válido: %s)",
		meshData->vertexArray,
		glIsVertexArray(meshData->vertexArray) ? "sí" : "NO");

	LOG(LogType::LOG_INFO, "IBO: %u (válido: %s)",
		meshData->indexBuffer,
		glIsBuffer(meshData->indexBuffer) ? "sí" : "NO");

	LOG(LogType::LOG_INFO, "VBO Posición: %u (válido: %s)",
		meshData->positionBuffer,
		glIsBuffer(meshData->positionBuffer) ? "sí" : "NO");

	LOG(LogType::LOG_INFO, "VBO TexCoord: %u (válido: %s)",
		meshData->texCoordBuffer,
		glIsBuffer(meshData->texCoordBuffer) ? "sí" : "NO");

	LOG(LogType::LOG_INFO, "VBO Normal: %u (válido: %s)",
		meshData->normalBuffer,
		glIsBuffer(meshData->normalBuffer) ? "sí" : "NO");

	// Información de conteo
	LOG(LogType::LOG_INFO, "Índices: %u", meshData->indexCount);
	LOG(LogType::LOG_INFO, "Vértices: %u", meshData->vertexCount);
	LOG(LogType::LOG_INFO, "ID de malla: %u", meshData->meshId);
	LOG(LogType::LOG_INFO, "Flags de atributos: 0x%X", meshData->attributeFlags);

	// Decodificar flags de atributos
	LOG(LogType::LOG_INFO, "Atributos habilitados:");
	if (meshData->attributeFlags & (1 << 0)) LOG(LogType::LOG_INFO, " - Posición");
	if (meshData->attributeFlags & (1 << 1)) LOG(LogType::LOG_INFO, " - TexCoord");
	if (meshData->attributeFlags & (1 << 2)) LOG(LogType::LOG_INFO, " - Normal");
	if (meshData->attributeFlags & (1 << 3)) LOG(LogType::LOG_INFO, " - Tangente");
	if (meshData->attributeFlags & (1 << 4)) LOG(LogType::LOG_INFO, " - Bitangente");
	if (meshData->attributeFlags & (1 << 5)) LOG(LogType::LOG_INFO, " - Color");

	// Verificar y mostrar posiciones de vértices
	if (glIsBuffer(meshData->positionBuffer)) {
		glBindBuffer(GL_ARRAY_BUFFER, meshData->positionBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tamaño del buffer de posición: %d bytes", bufferSize);

		if (bufferSize > 0) {
			// Leer los datos del buffer
			float* positions = (float*)malloc(bufferSize);
			if (positions) {
				glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, positions);

				// Mostrar las primeras posiciones (hasta 5 vértices)
				int numVerts = std::min(5, (int)(bufferSize / (3 * sizeof(float))));
				LOG(LogType::LOG_INFO, "Primeras %d posiciones de vértices:", numVerts);

				for (int i = 0; i < numVerts; i++) {
					LOG(LogType::LOG_INFO, " Vértice %d: (%f, %f, %f)",
						i, positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
				}

				free(positions);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Verificar y mostrar índices
	if (glIsBuffer(meshData->indexBuffer)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tamaño del buffer de índices: %d bytes", bufferSize);

		if (bufferSize > 0) {
			// Leer los datos del buffer
			unsigned int* indices = (unsigned int*)malloc(bufferSize);
			if (indices) {
				glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, indices);

				// Mostrar los primeros índices (hasta 15 índices, o 5 triángulos)
				int numIndices = std::min(15, (int)(bufferSize / sizeof(unsigned int)));
				LOG(LogType::LOG_INFO, "Primeros %d índices:", numIndices);

				for (int i = 0; i < numIndices; i += 3) {
					if (i + 2 < numIndices) {
						LOG(LogType::LOG_INFO, " Triángulo %d: %u, %u, %u",
							i / 3, indices[i], indices[i + 1], indices[i + 2]);
					}
				}

				free(indices);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	LOG(LogType::LOG_INFO, "=== Fin de información de GPUMesh ===");
}