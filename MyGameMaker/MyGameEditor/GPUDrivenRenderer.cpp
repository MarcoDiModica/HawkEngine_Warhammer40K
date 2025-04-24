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

	for (int i = 0; i < 2; i++) {
		glCreateBuffers(1, &drawCommandBuffers[i]);
		glNamedBufferStorage(drawCommandBuffers[i],
			MAX_DRAW_COMMANDS * sizeof(DrawElementsCommand),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &cullDataBuffers[i]);
		glNamedBufferStorage(cullDataBuffers[i],
			MAX_DRAW_COMMANDS * sizeof(CullData),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &visibleCountBuffers[i]);
		glNamedBufferStorage(visibleCountBuffers[i],
			sizeof(GLuint),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);

		GLuint zero = 0;
		glNamedBufferSubData(visibleCountBuffers[i], 0, sizeof(GLuint), &zero);
	}

	if (useGPUCulling) {
		if (!CompileCullingShader()) {
			LOG(LogType::LOG_INFO, "No se pudo compilar shader de culling, usando CPU fallback");
			useGPUCulling = false;
		}
	}

	return true;
}

void GPUDrivenRenderer::Shutdown() {
	if (frameFence) {
		glDeleteSync(frameFence);
		frameFence = nullptr;
	}

	for (int i = 0; i < 2; i++) {
		if (drawCommandBuffers[i]) glDeleteBuffers(1, &drawCommandBuffers[i]);
		if (cullDataBuffers[i]) glDeleteBuffers(1, &cullDataBuffers[i]);
		if (visibleCountBuffers[i]) glDeleteBuffers(1, &visibleCountBuffers[i]);
	}

	if (cullingShader) glDeleteProgram(cullingShader);
	if (defaultVAO) glDeleteVertexArrays(1, &defaultVAO);

	cullingShader = 0;
	defaultVAO = 0;

	cullData.clear();
	drawCommands.clear();
	shaderBatches.clear();
}

void GPUDrivenRenderer::BeginFrame() {
	if (frameFence) {
		GLenum result = glClientWaitSync(frameFence, GL_SYNC_FLUSH_COMMANDS_BIT, 100000000); // 100ms timeout
		if (result == GL_TIMEOUT_EXPIRED) {
			LOG(LogType::LOG_WARNING, "BeginFrame: Timeout esperando a que la GPU termine");
		}
		glDeleteSync(frameFence);
		frameFence = nullptr;
	}

	cullData.clear();
	drawCommands.clear();
	shaderBatches.clear();
	currentInstanceOffset = 0;
	visibleInstanceCount = 0;

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffers[updateBufferIndex], 0, sizeof(GLuint), &zero);

	BindlessManager::GetInstance().ClearInstances();
}

void GPUDrivenRenderer::EndFrame() {
	if (frameFence) {
		glDeleteSync(frameFence);
	}
	frameFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	std::swap(updateBufferIndex, renderBufferIndex);

	LOG(LogType::LOG_INFO, "EndFrame: Buffers intercambiados - Render: %d, Update: %d",
		renderBufferIndex, updateBufferIndex);
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
	glNamedBufferSubData(visibleCountBuffers[updateBufferIndex], 0, sizeof(GLuint), &zero);

	glNamedBufferSubData(cullDataBuffers[updateBufferIndex], 0,
		cullData.size() * sizeof(CullData), cullData.data());

	if (!enableCulling) {
		ForceIncludeAllObjects();
	}
	else if (!useGPUCulling) {
		CPUFrustumCulling();
	}
	else {
		glUseProgram(cullingShader);
		SetCullingUniforms(viewMatrix, projMatrix, cameraPos);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cullDataBuffers[updateBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, drawCommandBuffers[updateBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visibleCountBuffers[updateBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BindlessManager::GetInstance().GetMeshBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, BindlessManager::GetInstance().GetInstanceBuffer());

		GLuint numGroups = (cullData.size() + 63) / 64;

		glDispatchCompute(numGroups, 1, 1);

		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glGetNamedBufferSubData(visibleCountBuffers[updateBufferIndex], 0, sizeof(GLuint), &visibleInstanceCount);

		drawCommands.resize(cullData.size());
		glGetNamedBufferSubData(drawCommandBuffers[updateBufferIndex], 0,
			cullData.size() * sizeof(DrawElementsCommand), drawCommands.data());

		LOG(LogType::LOG_INFO, "Draw commands generados: %zu", drawCommands.size());
		if (!drawCommands.empty()) {
			LOG(LogType::LOG_INFO, "Primer draw command: count=%u, instanceCount=%u",
				drawCommands[0].count, drawCommands[0].instanceCount);
		}
	}

	BatchCommandsByShaderType();

	LOG(LogType::LOG_INFO, "Total de instancias visibles: %d", visibleInstanceCount);
	LOG(LogType::LOG_INFO, "Total de comandos de dibujo: %d", (int)drawCommands.size());
	LOG(LogType::LOG_INFO, "Total de batches por shader: %d", (int)shaderBatches.size());
}

void GPUDrivenRenderer::ForceIncludeAllObjects() {
	drawCommands.clear();
	visibleInstanceCount = 0;

	for (const auto& cullItem : cullData) {
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
		if (!meshData) continue;

		if (!glIsVertexArray(meshData->vertexArray) || !glIsBuffer(meshData->indexBuffer)) {
			LOG(LogType::LOG_WARNING, "Invalid VAO/IBO for mesh %u: VAO=%u, IBO=%u",
				cullItem.meshIndex, meshData->vertexArray, meshData->indexBuffer);
			continue;
		}

		DrawElementsCommand command = {};
		command.count = meshData->indexCount;
		command.instanceCount = cullItem.instanceCount;
		command.firstIndex = 0;
		command.baseVertex = 0;
		command.baseInstance = cullItem.instanceOffset;

		drawCommands.push_back(command);
		visibleInstanceCount += cullItem.instanceCount;

		LOG(LogType::LOG_INFO, "Draw command %zu: mesh=%u, count=%u, instanceCount=%u, baseInstance=%u",
			drawCommands.size() - 1, cullItem.meshIndex, command.count, command.instanceCount, command.baseInstance);
	}

	if (drawCommands.empty()) {
		LOG(LogType::LOG_WARNING, "No draw commands generated!");
		return;
	}

	glNamedBufferSubData(
		drawCommandBuffers[updateBufferIndex],
		0,
		drawCommands.size() * sizeof(DrawElementsCommand),
		drawCommands.data()
	);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "Error uploading draw commands: 0x%X", error);
	}
	else {
		LOG(LogType::LOG_INFO, "Successfully uploaded %zu draw commands to GPU buffer", drawCommands.size());
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
		glNamedBufferSubData(
			drawCommandBuffers[updateBufferIndex],
			0,
			drawCommands.size() * sizeof(DrawElementsCommand),
			drawCommands.data()
		);
	}
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

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos) {
	if (shaderBatches.empty()) {
		LOG(LogType::LOG_INFO, "No hay objetos para renderizar");
		return;
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffers[renderBufferIndex]);

	for (const auto& [shaderType, batch] : shaderBatches) {
		switch (shaderType) {
		case ShaderType::UNLIT:
			RenderUnlitBatch(batch, viewMatrix, projMatrix);
			break;
		case ShaderType::PBR:
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

	while (glGetError() != GL_NO_ERROR) {}

	if (!glIsBuffer(drawCommandBuffers[renderBufferIndex])) {
		LOG(LogType::LOG_ERROR, "Draw command buffer is invalid!");
		return;
	}

	if (batch.commands.empty()) {
		LOG(LogType::LOG_WARNING, "Empty batch, nothing to render");
		return;
	}

	GLuint unlitShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::UNLIT);
	if (unlitShader == 0) {
		LOG(LogType::LOG_ERROR, "Failed to get UNLIT shader program");
		return;
	}

	glUseProgram(unlitShader);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUniformMatrix4fv(glGetUniformLocation(unlitShader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(unlitShader, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

	bool hasBindless = GLEW_ARB_bindless_texture && GLEW_ARB_gpu_shader_int64;
	bool hasSSBOs = GLEW_ARB_shader_storage_buffer_object;
	bool hasMDI = GLEW_ARB_multi_draw_indirect;

	GLint useBindlessLoc = glGetUniformLocation(unlitShader, "useBindlessMode");
	if (useBindlessLoc != -1) {
		glUniform1i(useBindlessLoc, (hasBindless && hasSSBOs) ? 1 : 0);
	}

	if (hasSSBOs) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BindlessManager::GetInstance().GetInstanceBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BindlessManager::GetInstance().GetMaterialBuffer());
	}

	glNamedBufferSubData(
		drawCommandBuffers[renderBufferIndex],
		0,
		batch.commands.size() * sizeof(DrawElementsCommand),
		batch.commands.data()
	);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffers[renderBufferIndex]);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "OpenGL error before drawing: 0x%X", error);
	}

	std::unordered_map<GLuint, std::vector<size_t>> vaoGroups;
	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		if (i >= batch.commands.size()) continue;

		uint32_t meshIndex = batch.meshIndices[i];
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		if (!meshData || meshData->vertexArray == 0) continue;

		vaoGroups[meshData->vertexArray].push_back(i);
	}

	LOG(LogType::LOG_INFO, "Rendering %zu VAO groups with %zu total commands",
		vaoGroups.size(), batch.commands.size());

	for (const auto& [vao, commandIndices] : vaoGroups) {
		if (commandIndices.empty()) continue;

		uint32_t meshIndex = batch.meshIndices[commandIndices[0]];
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		if (!meshData) continue;

		while (glGetError() != GL_NO_ERROR) {}

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		GLint boundIbo = 0;
		glGetVertexArrayiv(vao, GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundIbo);
		LOG(LogType::LOG_INFO, "VAO %u has IBO %u bound (should be %u)",
			vao, boundIbo, meshData->indexBuffer);

		GLenum bindError = glGetError();
		if (bindError != GL_NO_ERROR) {
			LOG(LogType::LOG_ERROR, "Error after binding VAO/IBO: 0x%X", bindError);
			continue;
		}

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffers[renderBufferIndex]);
		bindError = glGetError();
		if (bindError != GL_NO_ERROR) {
			LOG(LogType::LOG_ERROR, "Error binding indirect buffer: 0x%X", bindError);
			continue;
		}

		for (size_t cmdIdx : commandIndices) {
			GLintptr byteOffset = cmdIdx * sizeof(DrawElementsCommand);

			LOG(LogType::LOG_INFO, "Drawing command %zu with offset %lld", cmdIdx, (long long)byteOffset);

			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)byteOffset);

			GLenum drawError = glGetError();
			if (drawError != GL_NO_ERROR) {
				LOG(LogType::LOG_ERROR, "Error after DrawElementsIndirect: 0x%X", drawError);
			}
		}
	}

	//debug mesh info
	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		if (i >= batch.commands.size()) continue;

		uint32_t meshIndex = batch.meshIndices[i];
		DebugMeshInfo(meshIndex);
	}

	if (visibleInstanceCount == 0 && !batch.meshIndices.empty()) {
		LOG(LogType::LOG_INFO, "No instances rendered! Trying debug fallback render...");

		uint32_t meshIndex = batch.meshIndices[0];
		uint32_t materialIndex = batch.materialIndices[0];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		if (meshData && meshData->vertexArray && meshData->indexBuffer) {
			glBindVertexArray(meshData->vertexArray);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

			GLint modelMatrixLoc = glGetUniformLocation(unlitShader, "model");
			if (modelMatrixLoc != -1) {
				glm::mat4 identityModel(1.0f);
				glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(identityModel));
			}

			GLint materialIdxLoc = glGetUniformLocation(unlitShader, "materialIndex");
			if (materialIdxLoc != -1) {
				glUniform1ui(materialIdxLoc, materialIndex);
			}

			GLint useBindlessLoc = glGetUniformLocation(unlitShader, "useBindlessMode");
			if (useBindlessLoc != -1) {
				glUniform1i(useBindlessLoc, 0);
			}

			glDrawElements(GL_TRIANGLES, meshData->indexCount, GL_UNSIGNED_INT, 0);

			LOG(LogType::LOG_INFO, "Debug fallback: Drew mesh %u with VAO=%u, IBO=%u, indices=%u",
				meshIndex, meshData->vertexArray, meshData->indexBuffer, meshData->indexCount);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void GPUDrivenRenderer::BatchCommandsByShaderType() {
	shaderBatches.clear();

	for (size_t i = 0; i < cullData.size(); i++) {
		if (i >= drawCommands.size()) continue;

		const CullData& cullItem = cullData[i];

		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(cullItem.materialIndex);
		if (!materialData) {
			LOG(LogType::LOG_WARNING, "Material inválido en índice %u, omitiendo", cullItem.materialIndex);
			continue;
		}

		ShaderType shaderType = static_cast<ShaderType>(materialData->shaderType);

		ShaderBatch& batch = shaderBatches[shaderType];
		batch.shaderType = shaderType;

		batch.commands.push_back(drawCommands[i]);
		batch.meshIndices.push_back(cullItem.meshIndex);
		batch.materialIndices.push_back(cullItem.materialIndex);
	}

	for (const auto& [type, batch] : shaderBatches) {
		std::string shaderName;
		switch (type) {
		case ShaderType::PBR: shaderName = "PBR"; break;
		case ShaderType::UNLIT: shaderName = "UNLIT"; break;
		default: shaderName = "DESCONOCIDO";
		}

		LOG(LogType::LOG_INFO, "Batch para shader %s: %zu comandos",
			shaderName.c_str(), batch.commands.size());
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

	for (auto& frustumPlane : frustumPlanes) {
		float length = glm::length(glm::vec3(frustumPlane));
		if (length > 0.0001f) {
			frustumPlane /= length;
		}
	}

	glUniform4fv(glGetUniformLocation(cullingShader, "u_frustum.planes"),
		6, glm::value_ptr(frustumPlanes[0]));
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

	LOG(LogType::LOG_INFO, "Índices: %u", meshData->indexCount);
	LOG(LogType::LOG_INFO, "Vértices: %u", meshData->vertexCount);
	LOG(LogType::LOG_INFO, "ID de malla: %u", meshData->meshId);
	LOG(LogType::LOG_INFO, "Flags de atributos: 0x%X", meshData->attributeFlags);

	LOG(LogType::LOG_INFO, "Atributos habilitados:");
	if (meshData->attributeFlags & (1 << 0)) LOG(LogType::LOG_INFO, " - Posición");
	if (meshData->attributeFlags & (1 << 1)) LOG(LogType::LOG_INFO, " - TexCoord");
	if (meshData->attributeFlags & (1 << 2)) LOG(LogType::LOG_INFO, " - Normal");
	if (meshData->attributeFlags & (1 << 3)) LOG(LogType::LOG_INFO, " - Tangente");
	if (meshData->attributeFlags & (1 << 4)) LOG(LogType::LOG_INFO, " - Bitangente");
	if (meshData->attributeFlags & (1 << 5)) LOG(LogType::LOG_INFO, " - Color");

	if (glIsBuffer(meshData->positionBuffer)) {
		glBindBuffer(GL_ARRAY_BUFFER, meshData->positionBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tamaño del buffer de posición: %d bytes", bufferSize);

		if (bufferSize > 0) {
			float* positions = (float*)malloc(bufferSize);
			if (positions) {
				glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, positions);

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

	if (glIsBuffer(meshData->indexBuffer)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tamaño del buffer de índices: %d bytes", bufferSize);

		if (bufferSize > 0) {
			unsigned int* indices = (unsigned int*)malloc(bufferSize);
			if (indices) {
				glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, indices);

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