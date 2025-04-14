#include "GPUDrivenRenderer.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include "MyGameEngine/ShaderManager.h"

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
}

void GPUDrivenRenderer::BeginFrame() {
	cullData.clear();
	drawCommands.clear();
	currentInstanceOffset = 0;
	visibleInstanceCount = 0;

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	BindlessManager::GetInstance().ClearInstances();
}

void GPUDrivenRenderer::EndFrame() {
}

void GPUDrivenRenderer::AddInstanceGroup(
	uint32_t meshIndex,
	const glm::vec4& boundingSphere,
	const std::vector<GPUInstance>& instances) {

	if (instances.empty() || meshIndex >= BindlessManager::GetInstance().GetMeshCount()) {
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
	cullItem.instanceOffset = currentInstanceOffset;
	cullItem.instanceCount = static_cast<uint32_t>(instances.size());

	for (const auto& instance : instances) {
		BindlessManager::GetInstance().AddInstance(instance);
	}

	currentInstanceOffset += static_cast<uint32_t>(instances.size());
	cullData.push_back(cullItem);
}

void GPUDrivenRenderer::PrepareDrawCommands(/*const Frustum& frustum*/) {
	if (cullData.empty()) {
		return;
	}

	glNamedBufferSubData(cullDataBuffer, 0,
		cullData.size() * sizeof(CullData),
		cullData.data());

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

	LOG(LogType::LOG_INFO, "Total de instancias visibles: %d", visibleInstanceCount);
	LOG(LogType::LOG_INFO, "Total de draw commands: %d", (int)drawCommands.size());
}

//void GPUDrivenRenderer::CPUFrustumCulling(const Frustum& frustum) {
//	drawCommands.clear();
//	visibleInstanceCount = 0;
//
//	GLuint zero = 0;
//	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);
//
//	for (const auto& cullItem : cullData) {
//		bool isVisible = true;
//
//		if (useFrustumCulling) {
//			glm::vec3 center(cullItem.boundingSphere);
//			float radius = cullItem.boundingSphere.w;
//
//			isVisible = frustum.SphereInFrustum(center, radius);
//		}
//
//		if (isVisible) {
//			GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
//			if (!meshData) continue;
//
//			DrawElementsCommand command;
//			command.count = meshData->indexCount;
//			command.instanceCount = cullItem.instanceCount;
//			command.firstIndex = 0;
//			command.baseVertex = 0;
//			command.baseInstance = cullItem.instanceOffset;
//
//			drawCommands.push_back(command);
//
//		}
//	}
//
//	if (!drawCommands.empty()) {
//		glNamedBufferSubData(drawCommandBuffer, 0,
//			drawCommands.size() * sizeof(DrawElementsCommand),
//			drawCommands.data());
//	}
//}

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	if (drawCommands.empty()) {
		return;
	}

	// Obtener shader para renderizado bindless
	GLuint bindlessShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::BINDLESS_PBR);

	if (bindlessShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo encontrar shader bindless para renderizado");
		return;
	}

	glUseProgram(bindlessShader);

	// Pasar matrices de cámara al shader
	glUniformMatrix4fv(glGetUniformLocation(bindlessShader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(bindlessShader, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Bind buffers para bindless rendering
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BindlessManager::GetInstance().GetMaterialBuffer());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BindlessManager::GetInstance().GetMeshBuffer());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BindlessManager::GetInstance().GetInstanceBuffer());

	glBindVertexArray(defaultVAO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

	if (GLEW_ARB_multi_draw_indirect) {
		LOG(LogType::LOG_INFO, "Usando multi-draw indirecto para renderizado bindless");
		glMultiDrawElementsIndirect(
			GL_TRIANGLES,
			GL_UNSIGNED_INT,
			nullptr,
			(GLsizei)drawCommands.size(),
			sizeof(DrawElementsCommand)
		);
	}
	else {
		LOG(LogType::LOG_INFO, "Usando fallback de multi-draw indirecto (no soportado por la GPU)");
		// Fallback cuando multi-draw indirect no está disponible
		for (size_t i = 0; i < drawCommands.size(); i++) {
			const DrawElementsCommand& cmd = drawCommands[i];

			// Obtener datos de malla para este draw command
			GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullData[i].meshIndex);
			if (!meshData) continue;

			// Bind VAO y buffers específicos para esta malla
			glBindVertexArray(meshData->vertexArray);

			glDrawElementsInstancedBaseVertexBaseInstance(
				GL_TRIANGLES,
				cmd.count,
				GL_UNSIGNED_INT,
				(void*)(intptr_t)(cmd.firstIndex * sizeof(GLuint)),
				cmd.instanceCount,
				cmd.baseVertex,
				cmd.baseInstance
			);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glUseProgram(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
}

bool GPUDrivenRenderer::CompileCullingShader() {
	// TODO: Implementar shader de culling en GPU
	// Por ahora retornamos false para indicar que no está implementado
	return false;
}