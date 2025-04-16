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

	BindlessManager::GetInstance().ClearInstances();
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
	cullItem.materialIndex = materialIndex; // Guardar el índice del material
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

	// Después de preparar los draw commands, agrupamos por shader
	BatchCommandsByShaderType();

	LOG(LogType::LOG_INFO, "Total de instancias visibles: %d", visibleInstanceCount);
	LOG(LogType::LOG_INFO, "Total de draw commands: %d", (int)drawCommands.size());
	LOG(LogType::LOG_INFO, "Total de batches por shader: %d", (int)shaderBatches.size());
}

void GPUDrivenRenderer::BatchCommandsByShaderType() {
	// Limpiar batches anteriores
	shaderBatches.clear();

	for (size_t i = 0; i < cullData.size(); i++) {
		const CullData& cullItem = cullData[i];

		// Obtener el material para determinar el tipo de shader
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(cullItem.materialIndex);
		if (!materialData) {
			LOG(LogType::LOG_WARNING, "Material inválido en índice %u, omitiendo", cullItem.materialIndex);
			continue;
		}

		// Convertir el valor uint32_t a ShaderType
		ShaderType shaderType = static_cast<ShaderType>(materialData->shaderType);

		// Obtener o crear el batch para este tipo de shader
		ShaderBatch& batch = shaderBatches[shaderType];
		batch.shaderType = shaderType;

		// Añadir el comando al batch
		if (i < drawCommands.size()) {
			batch.commands.push_back(drawCommands[i]);
			batch.meshIndices.push_back(cullItem.meshIndex);
			batch.materialIndices.push_back(cullItem.materialIndex);
		}
	}

	// Registrar estadísticas
	for (const auto& [type, batch] : shaderBatches) {
		std::string shaderName;
		switch (type) {
		case ShaderType::PBR: shaderName = "PBR"; break;
		case ShaderType::UNLIT: shaderName = "UNLIT"; break;
		default: shaderName = "DESCONOCIDO";
		}

		LOG(LogType::LOG_INFO, "Batch de shader %s: %zu comandos",
			shaderName.c_str(), batch.commands.size());
	}
}

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	// Si no hay nada que renderizar, salir temprano
	if (shaderBatches.empty()) {
		LOG(LogType::LOG_INFO, "No hay objetos para renderizar");
		return;
	}

	// Guardar estado de OpenGL
	GLboolean depthTestEnabled;
	glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);

	GLboolean cullFaceEnabled;
	glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);

	// Configurar estado global de OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Preparar buffer de comandos de dibujo indirecto
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

	// Actualizar los buffers antes de renderizar
	BindlessManager::GetInstance().UpdateBuffers();

	// Renderizar cada batch de shader
	for (const auto& [shaderType, batch] : shaderBatches) {
		RenderUnlitBatch(batch, viewMatrix, projMatrix);
	}

	// Limpiar estado
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	// Restaurar estado de OpenGL
	if (!depthTestEnabled) glDisable(GL_DEPTH_TEST);
	if (!cullFaceEnabled) glDisable(GL_CULL_FACE);

	// Estadísticas de renderizado
	LOG(LogType::LOG_INFO, "Renderizado completado: %d instancias en %zu batches",
		visibleInstanceCount, shaderBatches.size());

	// Intercambiar buffers en BindlessManager al final del frame
	BindlessManager::GetInstance().EndFrame();
}

void GPUDrivenRenderer::RenderUnlitBatch(
	const ShaderBatch& batch,
	const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix) {

	if (batch.commands.empty()) return;

	// Temporarily disable culling and depth test to debug visibility issues
	GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);

	glDisable(GL_CULL_FACE);  // Disable face culling temporarily 

	// Get the shader
	Shaders* shader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "No se pudo obtener el shader UNLIT");
		return;
	}

	shader->Bind();
	shader->SetUniformMat4("view", viewMatrix);
	shader->SetUniformMat4("projection", projMatrix);

	// Loop through each mesh/material in the batch
	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);

		if (!meshData || !materialData) continue;

		// Set a bright color for visibility
		glm::vec4 debugColor(1.0f, 0.0f, 1.0f, 1.0f);  // Magenta - very visible
		shader->SetUniformVec4("color", debugColor);

		// Position the cube at the center of the view with some scale
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));  // Move it in front of camera
		model = glm::scale(model, glm::vec3(2.0f));  // Make it larger
		shader->SetUniformMat4("model", model);

		// Bind the VAO
		glBindVertexArray(meshData->vertexArray);

		// Make sure index buffer is bound
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		// Check which draw command to use
		if (i < batch.commands.size()) {
			const DrawElementsCommand& cmd = batch.commands[i];

			LOG(LogType::LOG_INFO, "Dibujando mesh %u, material %u: %u indices (VAO=%u, IBO=%u)",
				meshIndex, materialIndex, cmd.count, meshData->vertexArray, meshData->indexBuffer);

			// For debugging, draw wireframe
			GLint polygonMode[2];
			glGetIntegerv(GL_POLYGON_MODE, polygonMode);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// Draw elements
			glDrawElements(
				GL_TRIANGLES,
				cmd.count,
				GL_UNSIGNED_INT,
				(void*)(0)
			);

			// Restore polygon mode
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
		}
	}

	// Clean up state
	glBindVertexArray(0);
	shader->UnBind();

	// Restore OpenGL state
	if (depthEnabled) glEnable(GL_DEPTH_TEST);
	if (cullEnabled) glEnable(GL_CULL_FACE);
}

bool GPUDrivenRenderer::CompileCullingShader() {
	//pillar el shader de ShaderManager GetShaderPorgram
	cullingShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::CULLING_COMPUTE);
	if (cullingShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo obtener el programa de shader de culling");
		return false;
	}

	return true;
}