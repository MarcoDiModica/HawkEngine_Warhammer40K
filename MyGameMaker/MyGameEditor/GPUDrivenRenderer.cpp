#include "GPUDrivenRenderer.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include "MyGameEngine/ShaderManager.h"
#include "MyGameEngine/CameraComponent.h"
#include "ForwardPlus.h"
#include "App.h"
#include "UISceneWindow.h"
#include "MyGUI.h"
#include "UIGameView.h"

GPUDrivenRenderer& GPUDrivenRenderer::GetInstance() {
	static GPUDrivenRenderer instance;
	return instance;
}

void GPUDrivenRenderer::InitializeShadows()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG(LogType::LOG_ERROR, "Shadow Framebuffer is not complete! Status: 0x%x", status);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	InitializeShadows();

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
}

void GPUDrivenRenderer::AddInstanceGroup(
	uint32_t meshIndex,
	uint32_t materialIndex,
	const std::vector<GPUInstance>& instances) {

	if (instances.empty() || meshIndex >= BindlessManager::GetInstance().GetMeshCount()) {
		return;
	}

	if (materialIndex >= BindlessManager::GetInstance().GetMaterialCount()) {
		LOG(LogType::LOG_WARNING, "Warning: �ndice de material inv�lido: %u", materialIndex);
		return;
	}

	if (cullData.size() >= MAX_DRAW_COMMANDS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado l�mite m�ximo de comandos de dibujo");
		return;
	}

	CullData cullItem;
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

void GPUDrivenRenderer::PrepareDrawCommands() {
	if (cullData.empty()) {
		LOG(LogType::LOG_INFO, "No hay datos de culling para procesar");
		return;
	}

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	glNamedBufferSubData(cullDataBuffer, 0,
		cullData.size() * sizeof(CullData), cullData.data());

	ForceIncludeAllObjects();

	BatchCommandsByShaderType();
}

void GPUDrivenRenderer::ForceIncludeAllObjects() {
	drawCommands.clear();
	visibleInstanceCount = 0;

	for (const auto& cullItem : cullData) {
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
		if (!meshData) {
			LOG(LogType::LOG_WARNING, "Mesh no encontrado para indice %u, saltando...", cullItem.meshIndex);
			continue;
		}

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
	else {
		LOG(LogType::LOG_WARNING, "No hay draw commands para generar");
	}

	LOG(LogType::LOG_INFO, "Procesando objetos filtrados por frustum: %zu objetos, %d instancias visibles",
		drawCommands.size(), visibleInstanceCount);
}

void GPUDrivenRenderer::BatchCommandsByShaderType() {
	shaderBatches.clear();

	for (size_t i = 0; i < cullData.size(); i++) {
		const CullData& cullItem = cullData[i];

		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(cullItem.materialIndex);
		if (!materialData) {
			LOG(LogType::LOG_WARNING, "Material invalido en �ndice %u, omitiendo", cullItem.materialIndex);
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
		case ShaderType::UI: shaderName = "UI"; break;
		default: shaderName = "DESCONOCIDO";
		}
	}
}

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos, bool isEditor) {
	if (shaderBatches.empty()) {
		LOG(LogType::LOG_INFO, "No hay objetos para renderizar");
		return;
	}

	for (const auto& [shaderType, batch] : shaderBatches) {
		if (shaderType == ShaderType::PBR) {
			RenderShadowBatch(batch, ForwardPlusLighting::GetInstance().GetLightSpaceMatrix(), isEditor);
		}
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

	for (const auto& [shaderType, batch] : shaderBatches) {
		switch (shaderType) {
		case ShaderType::UNLIT:
			RenderUnlitBatch(batch, viewMatrix, projMatrix);
			break;
		case ShaderType::PBR:
			RenderPBRBatch(batch, viewMatrix, projMatrix, cameraPos, ForwardPlusLighting::GetInstance().GetLightSpaceMatrix());
			break;
		default:
			LOG(LogType::LOG_WARNING, "Tipo de shader desconocido: %d", (int)shaderType);
			break;
		}
	}

    auto it = shaderBatches.find(ShaderType::UI);
    if (it != shaderBatches.end()) {
        RenderUIBatch(it->second, viewMatrix, projMatrix);
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void GPUDrivenRenderer::RenderShadowBatch(const ShaderBatch& batch, glm::mat4 lightSpaceMatrix, bool isEditor)
{
	if (batch.commands.empty()) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(ShaderType::DEPTH);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "No se pudo obtener el shader SHADOW");
		return;
	}

	shader->Bind();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLuint currentFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&currentFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glClear(GL_DEPTH_BUFFER_BIT);

	shader->SetUniformMat4("lightSpaceMatrix", lightSpaceMatrix);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BindlessManager::GetInstance().GetInstanceBuffer());

	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];
		
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);

		if (!meshData || !materialData) continue;

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
		}
	}

	glBindVertexArray(0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader->UnBind();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
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

	bindlessErrorDetected = false;
	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);

		if (!meshData || !materialData) continue;

		shader->SetUniformVec4("albedoColor", materialData->albedoColor);

		if (materialData->flags & (1 << 0)) {
			shader->SetUniform("u_HasTexture", 1);
			if (GLEW_ARB_bindless_texture && GLEW_ARB_gpu_shader_int64 && !bindlessErrorDetected) {
				GLuint64 textureHandle = materialData->albedoTexture;
				if (textureHandle != 0) {
					shader->SetUniform("albedoTexture", textureHandle);

					GLenum error = glGetError();
					if (error != GL_NO_ERROR) {
						LOG(LogType::LOG_ERROR, "Error detectado al usar textura bindless: 0x%X", error);
						bindlessErrorDetected = true;

						GLuint textureID = 0;
						if (BindlessManager::GetInstance().GetTextureIDFromHandle(
							materialData->albedoTexture, textureID)) {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, textureID);
							shader->SetUniform("albedoTexture", 0);
						}
					}
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

void GPUDrivenRenderer::RenderPBRBatch(
	const ShaderBatch& batch,
	const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix,
	const glm::vec3& cameraPos,
	const glm::mat4& lightSpaceMatrix) {

	if (batch.commands.empty()) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(ShaderType::PBR);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "No se pudo obtener el shader PBR");
		return;
	}

	shader->Bind();
	shader->SetUniformMat4("view", viewMatrix);
	shader->SetUniformMat4("projection", projMatrix);
	shader->SetUniformVec3("cameraPos", cameraPos);
	shader->SetUniformMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader->SetUniform("u_HasShadowMap", 1);
	shader->SetUniform("shadowMap", 7);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ForwardPlusLighting::GetInstance().GetPointLightBuffer());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ForwardPlusLighting::GetInstance().GetDirectionalLightBuffer());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ForwardPlusLighting::GetInstance().GetLightGridBuffer());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ForwardPlusLighting::GetInstance().GetLightIndicesBuffer());

	// Update Forward+ uniforms every frame
	shader->SetUniform("useForwardPlus", 1);
	shader->SetUniform("tileSize", ForwardPlusLighting::GetInstance().GetTileSize());
	glm::vec2 tileSize = glm::vec2(
		ForwardPlusLighting::GetInstance().GetTilesX() * ForwardPlusLighting::GetInstance().GetTileSize(),
		ForwardPlusLighting::GetInstance().GetTilesY() * ForwardPlusLighting::GetInstance().GetTileSize());
	shader->SetUniformVec2("screenSize", tileSize);
	shader->SetUniform("numLights", ForwardPlusLighting::GetInstance().GetTotalLights());
	shader->SetUniform("maxLightsPerTile", ForwardPlusLighting::GetInstance().GetMaxLightsPerTile());

	GLint uniformCheck;
	glGetUniformiv(shader->GetProgram(), glGetUniformLocation(shader->GetProgram(), "useForwardPlus"), &uniformCheck);
	LOG(LogType::LOG_INFO, "Forward+ Uniform State: %d", uniformCheck);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BindlessManager::GetInstance().GetInstanceBuffer());

	bindlessErrorDetected = true;
	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);
		//get instance data
		GPUInstance* instanceData = BindlessManager::GetInstance().GetInstanceData(i);

		if (!meshData || !materialData) continue;

		shader->SetUniformVec4("albedoColor", materialData->albedoColor);
		shader->SetUniform("metallicFactor", materialData->pbrParams.x);
		shader->SetUniform("roughnessFactor", materialData->pbrParams.y);
		shader->SetUniform("aoFactor", materialData->pbrParams.z);
		shader->SetUniform("tonemapStrength", materialData->pbrParams.w);

		shader->SetUniformVec3("emissiveColor", glm::vec3(materialData->emissiveParams));
		shader->SetUniform("emissiveIntensity", materialData->emissiveParams.w);

		shader->SetUniform("heightScale", materialData->heightScale);

		

		if (GLEW_ARB_bindless_texture && GLEW_ARB_gpu_shader_int64 && !bindlessErrorDetected) {
			HandleTextureBindings(shader, "albedoMap", "u_HasAlbedoMap", materialData->albedoTexture);
			HandleTextureBindings(shader, "normalMap", "u_HasNormalMap", materialData->normalTexture);
			HandleTextureBindings(shader, "metallicMap", "u_HasMetallicMap", materialData->metallicTexture);
			HandleTextureBindings(shader, "roughnessMap", "u_HasRoughnessMap", materialData->roughnessTexture);
			HandleTextureBindings(shader, "aoMap", "u_HasAoMap", materialData->aoTexture);
			HandleTextureBindings(shader, "emissiveMap", "u_HasEmissiveMap", materialData->emissiveTexture);
			HandleTextureBindings(shader, "heightMap", "u_HasHeightMap", materialData->heightTexture);
			HandleTextureBindings(shader, "shadowMap", "u_HasShadowMap", depthMap);
		}
		else {
			BindRegularTextures(shader, materialData);
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
				LOG(LogType::LOG_ERROR, "GL Error after PBR draw: 0x%X", err);
			}
		}
	}

	glBindVertexArray(0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	shader->UnBind();
}

void GPUDrivenRenderer::RenderUIBatch(const ShaderBatch& batch, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
	if (batch.commands.empty()) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(ShaderType::UI);
	if (!shader) {
		LOG(LogType::LOG_ERROR, "No se pudo obtener el shader UI");
		return;
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->Bind();

	glm::mat4 uiViewMatrix = glm::mat4(1.0f);
	shader->SetUniformMat4("view", uiViewMatrix);

	glm::mat4 uiProjection;
	float width = 0, height = 0;

#ifdef _BUILD
	width = Application->window->width();
	height = Application->window->height();
#else
	width = Application->gui->UIGameViewPanel->GetWidth();
	height = Application->gui->UIGameViewPanel->GetHeight();
#endif

	uiProjection = glm::ortho(
		0.0f, width,
		height, 0.0f,
		-1.0f, 1.0f
	);

	shader->SetUniformMat4("projection", uiProjection);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BindlessManager::GetInstance().GetInstanceBuffer());

	for (size_t i = 0; i < batch.meshIndices.size(); i++) {
		uint32_t meshIndex = batch.meshIndices[i];
		uint32_t materialIndex = batch.materialIndices[i];

		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
		GPUMaterial* materialData = BindlessManager::GetInstance().GetMaterialData(materialIndex);

		if (!meshData || !materialData) continue;

		shader->SetUniformVec4("modColor", materialData->albedoColor);

		if (materialData->flags & (1 << 0)) {
			shader->SetUniform("u_HasTexture", 1);
			GLuint textureID = 0;
			if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->albedoTexture, textureID)) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureID);
				shader->SetUniform("texture1", 0);
			}
		}
		else {
			shader->SetUniform("u_HasTexture", 0);
		}

		shader->SetUniformVec2("SpriteOffset", materialData->spriteOffset);
		shader->SetUniformVec2("SpriteSize", materialData->spriteSize);
		shader->SetUniformVec2("SheetSize", materialData->sheetSize);

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
		}
	}

	glBindVertexArray(0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	shader->UnBind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void GPUDrivenRenderer::HandleTextureBindings(Shaders* shader, const char* textureName, const char* hasTextureName, GLuint64 textureHandle) {
	if (textureHandle != 0) {
		shader->SetUniform(hasTextureName, 1);
		shader->SetUniform(textureName, textureHandle);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			LOG(LogType::LOG_ERROR, "Error al usar textura bindless %s: 0x%X", textureName, error);
			bindlessErrorDetected = true;
		}
	}
	else {
		shader->SetUniform(hasTextureName, 0);
	}
}

void GPUDrivenRenderer::BindRegularTextures(Shaders* shader, GPUMaterial* materialData) {
	GLuint textureID = 0;

	// Albedo texture
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->albedoTexture, textureID)) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasAlbedoMap", 1);
		shader->SetUniform("albedoMap", 0);
	}
	else {
		shader->SetUniform("u_HasAlbedoMap", 0);
	}

	// Normal map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->normalTexture, textureID)) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasNormalMap", 1);
		shader->SetUniform("normalMap", 1);
	}
	else {
		shader->SetUniform("u_HasNormalMap", 0);
	}

	// Metallic map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->metallicTexture, textureID)) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasMetallicMap", 1);
		shader->SetUniform("metallicMap", 2);
	}
	else {
		shader->SetUniform("u_HasMetallicMap", 0);
	}

	// Roughness map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->roughnessTexture, textureID)) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasRoughnessMap", 1);
		shader->SetUniform("roughnessMap", 3);
	}
	else {
		shader->SetUniform("u_HasRoughnessMap", 0);
	}

	// AO map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->aoTexture, textureID)) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasAoMap", 1);
		shader->SetUniform("aoMap", 4);
	}
	else {
		shader->SetUniform("u_HasAoMap", 0);
	}

	// Emissive map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->emissiveTexture, textureID)) {
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasEmissiveMap", 1);
		shader->SetUniform("emissiveMap", 5);
	}
	else {
		shader->SetUniform("u_HasEmissiveMap", 0);
	}

	// Height map
	if (BindlessManager::GetInstance().GetTextureIDFromHandle(materialData->heightTexture, textureID)) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasHeightMap", 1);
		shader->SetUniform("heightMap", 6);
	}
	else {
		shader->SetUniform("u_HasHeightMap", 0);
	}

	// Shadow map

	if (BindlessManager::GetInstance().GetTextureIDFromHandle(depthMap, textureID)) {
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shader->SetUniform("u_HasShadowMap", 1);
		shader->SetUniform("shadowMap", 7);
	}
	else {
		shader->SetUniform("u_HasShadowMap", 0);
	}
}

#pragma region Debug
void GPUDrivenRenderer::DebugMeshInfo(uint32_t meshIndex) {
	GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(meshIndex);
	if (!meshData) {
		LOG(LogType::LOG_ERROR, "DebugMeshInfo: Mesh %u no encontrado", meshIndex);
		return;
	}

	LOG(LogType::LOG_INFO, "=== Informaci�n detallada de GPUMesh %u ===", meshIndex);

	// Verificar IDs de buffer
	LOG(LogType::LOG_INFO, "VAO: %u (v�lido: %s)",
		meshData->vertexArray,
		glIsVertexArray(meshData->vertexArray) ? "s�" : "NO");

	LOG(LogType::LOG_INFO, "IBO: %u (v�lido: %s)",
		meshData->indexBuffer,
		glIsBuffer(meshData->indexBuffer) ? "s�" : "NO");

	LOG(LogType::LOG_INFO, "VBO Posici�n: %u (v�lido: %s)",
		meshData->positionBuffer,
		glIsBuffer(meshData->positionBuffer) ? "s�" : "NO");

	LOG(LogType::LOG_INFO, "VBO TexCoord: %u (v�lido: %s)",
		meshData->texCoordBuffer,
		glIsBuffer(meshData->texCoordBuffer) ? "s�" : "NO");

	LOG(LogType::LOG_INFO, "VBO Normal: %u (v�lido: %s)",
		meshData->normalBuffer,
		glIsBuffer(meshData->normalBuffer) ? "s�" : "NO");

	// Informaci�n de conteo
	LOG(LogType::LOG_INFO, "�ndices: %u", meshData->indexCount);
	LOG(LogType::LOG_INFO, "V�rtices: %u", meshData->vertexCount);
	LOG(LogType::LOG_INFO, "ID de malla: %u", meshData->meshId);
	LOG(LogType::LOG_INFO, "Flags de atributos: 0x%X", meshData->attributeFlags);

	// Decodificar flags de atributos
	LOG(LogType::LOG_INFO, "Atributos habilitados:");
	if (meshData->attributeFlags & (1 << 0)) LOG(LogType::LOG_INFO, " - Posici�n");
	if (meshData->attributeFlags & (1 << 1)) LOG(LogType::LOG_INFO, " - TexCoord");
	if (meshData->attributeFlags & (1 << 2)) LOG(LogType::LOG_INFO, " - Normal");
	if (meshData->attributeFlags & (1 << 3)) LOG(LogType::LOG_INFO, " - Tangente");
	if (meshData->attributeFlags & (1 << 4)) LOG(LogType::LOG_INFO, " - Bitangente");
	if (meshData->attributeFlags & (1 << 5)) LOG(LogType::LOG_INFO, " - Color");

	// Verificar y mostrar posiciones de v�rtices
	if (glIsBuffer(meshData->positionBuffer)) {
		glBindBuffer(GL_ARRAY_BUFFER, meshData->positionBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tama�o del buffer de posici�n: %d bytes", bufferSize);

		if (bufferSize > 0) {
			// Leer los datos del buffer
			float* positions = (float*)malloc(bufferSize);
			if (positions) {
				glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, positions);

				// Mostrar las primeras posiciones (hasta 5 v�rtices)
				int numVerts = std::min(5, (int)(bufferSize / (3 * sizeof(float))));
				LOG(LogType::LOG_INFO, "Primeras %d posiciones de v�rtices:", numVerts);

				for (int i = 0; i < numVerts; i++) {
					LOG(LogType::LOG_INFO, " V�rtice %d: (%f, %f, %f)",
						i, positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
				}

				free(positions);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Verificar y mostrar �ndices
	if (glIsBuffer(meshData->indexBuffer)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBuffer);

		GLint bufferSize = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		LOG(LogType::LOG_INFO, "Tama�o del buffer de �ndices: %d bytes", bufferSize);

		if (bufferSize > 0) {
			// Leer los datos del buffer
			unsigned int* indices = (unsigned int*)malloc(bufferSize);
			if (indices) {
				glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, indices);

				// Mostrar los primeros �ndices (hasta 15 �ndices, o 5 tri�ngulos)
				int numIndices = std::min(15, (int)(bufferSize / sizeof(unsigned int)));
				LOG(LogType::LOG_INFO, "Primeros %d �ndices:", numIndices);

				for (int i = 0; i < numIndices; i += 3) {
					if (i + 2 < numIndices) {
						LOG(LogType::LOG_INFO, " Tri�ngulo %d: %u, %u, %u",
							i / 3, indices[i], indices[i + 1], indices[i + 2]);
					}
				}

				free(indices);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	LOG(LogType::LOG_INFO, "=== Fin de informaci�n de GPUMesh ===");
}
#pragma endregion