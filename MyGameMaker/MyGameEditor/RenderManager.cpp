#include "RenderManager.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/ShaderManager.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEditor/App.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "RenderCommand.h"
#include "RenderStats.h"

extern App* Application;

RenderManager& RenderManager::GetInstance() {
	static RenderManager instance;
	return instance;
}

RenderManager::RenderManager() {
	instancedRenderingEnabled = false;
	maxInstancesPerBatch = 100;

	RenderStats::GetInstance().SetVerbosityLevel(RenderStats::VerbosityLevel::FULL);
}

RenderManager::~RenderManager() {
	Cleanup();
}

bool RenderManager::Initialize() {
	// Create any necessary OpenGL objects/buffers
	ClearRenderQueues();

#ifdef _DEBUG
	RenderStats::GetInstance().SetupOpenGLDebugCallback();
#endif

	return true;
}

void RenderManager::Cleanup() {
	for (auto& buffer : instanceBuffers) {
		if (buffer.second != 0) {
			glDeleteBuffers(1, &buffer.second);
		}
	}
	instanceBuffers.clear();

	ClearRenderQueues();
}

void RenderManager::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	START_TIMING("FullRenderPass");

	ResetStateTracking();

	bool hasOpaque = false;
	bool hasTransparent = false;

	for (const auto& pair : opaqueQueues) {
		if (!pair.second.IsEmpty()) {
			hasOpaque = true;
			break;
		}
	}

	for (const auto& pair : transparentQueues) {
		if (!pair.second.IsEmpty()) {
			hasTransparent = true;
			break;
		}
	}

	if (!hasOpaque && !hasTransparent) {
		END_TIMING("FullRenderPass");
		return;
	}

	SaveGLState();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	START_TIMING("ProcessBatches");
	ProcessBatches();
	END_TIMING("ProcessBatches");

	START_TIMING("RenderOpaqueObjects");
	RenderBatches(false, viewMatrix, projectionMatrix);
	END_TIMING("RenderOpaqueObjects");

	START_TIMING("RenderTransparentObjects");
	glEnable(GL_BLEND);
	RenderBatches(true, viewMatrix, projectionMatrix);
	END_TIMING("RenderTransparentObjects");

	RestoreGLState();
	END_TIMING("FullRenderPass");
}

void RenderManager::RenderFromCamera(CameraComponent* camera) {
	if (!camera) return;

	glm::mat4 viewMatrix = camera->view();
	glm::mat4 projectionMatrix = camera->projection();

	Render(viewMatrix, projectionMatrix);
}

void RenderManager::RenderEditor(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	// Special handling for editor rendering
	// This would include rendering grid, gizmos, etc.
	Render(viewMatrix, projectionMatrix);
}

void RenderManager::RenderGame() {
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	if (!Application->root->mainCamera) return;

	CameraComponent* gameCamera = Application->root->mainCamera->GetComponent<CameraComponent>();
	if (!gameCamera) return;

	RenderFromCamera(gameCamera);
}

void RenderManager::SubmitMesh(GameObject* gameObject, const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material) {
	if (!gameObject || !mesh || !material) return;

	RenderCommand command;
	command.gameObject = gameObject;
	command.mesh = mesh;
	command.material = material;
	command.modelMatrix = gameObject->GetTransform()->GetMatrix();

	glm::vec3 cameraPos;
	if (Application->root->mainCamera) {
		cameraPos = Application->root->mainCamera->GetTransform()->GetPosition();
	}
	else if (Application->camera) {
		cameraPos = Application->camera->GetTransform().GetPosition();
	}

	glm::vec3 objectPos = gameObject->GetTransform()->GetPosition();
	command.distanceToCamera = glm::length(cameraPos - objectPos);

	command.isTransparent = material->GetColor().a < 0.99f;

	ShaderType shaderType = material->GetShaderType();

	if (command.isTransparent) {
		transparentQueues[shaderType].AddCommand(command);
	}
	else {
		opaqueQueues[shaderType].AddCommand(command);
	}
}

void RenderManager::SubmitGameObject(GameObject* gameObject) {
	if (!gameObject || !gameObject->IsActive()) return;

	if (gameObject->HasComponent<MeshRenderer>()) {
		auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
		auto mesh = meshRenderer->GetMesh();
		auto material = meshRenderer->GetMaterial();

		if (mesh && material) {
			SubmitMesh(gameObject, mesh, material);
		}
	}

	for (const auto& child : gameObject->GetChildren()) {
		if (child && child->IsActive()) {
			SubmitGameObject(child.get());
		}
	}
}

void RenderManager::SortRenderCommands() {
	for (auto& pair : opaqueQueues) {
		pair.second.Sort();
	}

	for (auto& pair : transparentQueues) {
		pair.second.Sort();
	}
}

void RenderManager::ClearRenderQueues() {
	for (auto& pair : opaqueQueues) {
		pair.second.Clear();
	}

	for (auto& pair : transparentQueues) {
		pair.second.Clear();
	}

	renderBatches.clear();
}

bool RenderManager::CanBatchCommands(const RenderCommand& a, const RenderCommand& b) const {
	if (!a.mesh || !b.mesh || !a.material || !b.material)
		return false;

	if (a.isTransparent != b.isTransparent)
		return false;

	if (a.material->GetShaderType() != b.material->GetShaderType())
		return false;

	if (a.mesh->getModel()->GetID() != b.mesh->getModel()->GetID())
		return false;

	if (a.material->GetId() != b.material->GetId())
		return false;

	if (a.specialData.isAnimated != b.specialData.isAnimated)
		return false;

	return true;
}

void RenderManager::ProcessBatches() {
	renderBatches.clear();

	struct BatchKey {
		unsigned int meshId;
		unsigned int materialId;
		ShaderType shaderType;
		bool isTransparent;
		bool isAnimated;

		bool operator==(const BatchKey& other) const {
			return meshId == other.meshId &&
				materialId == other.materialId &&
				shaderType == other.shaderType &&
				isTransparent == other.isTransparent &&
				isAnimated == other.isAnimated;
		}
	};

	struct KeyHasher {
		std::size_t operator()(const BatchKey& k) const {
			std::size_t h1 = std::hash<unsigned int>{}(k.meshId);
			std::size_t h2 = std::hash<unsigned int>{}(k.materialId);
			std::size_t h3 = std::hash<int>{}(static_cast<int>(k.shaderType));
			std::size_t h4 = std::hash<bool>{}(k.isTransparent);
			std::size_t h5 = std::hash<bool>{}(k.isAnimated);
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
		}
	};

	for (const auto& pair : opaqueQueues) {
		const ShaderType shaderType = pair.first;
		const auto& queue = pair.second;

		if (queue.commands.empty()) continue;

		std::unordered_map<BatchKey, RenderBatch, KeyHasher> batchMap;

		for (const auto& command : queue.commands) {
			if (!command.mesh || !command.material) continue;

			BatchKey key{
				command.mesh->getModel()->GetID(),
				command.material->GetId(),
				shaderType,
				false,
				command.specialData.isAnimated
			};

			auto& batch = batchMap[key];

			if (batch.commands.empty()) {
				batch.mesh = command.mesh;
				batch.material = command.material;
				batch.shaderType = shaderType;
				batch.isTransparent = false;
			}

			batch.commands.push_back(command);

			if (batch.commands.size() >= maxInstancesPerBatch) {
				if (renderBatches.find(shaderType) == renderBatches.end()) {
					renderBatches[shaderType] = std::vector<RenderBatch>();
				}
				renderBatches[shaderType].push_back(batch);
				batch.commands.clear();
			}
		}

		for (auto& pair : batchMap) {
			if (!pair.second.commands.empty()) {
				if (renderBatches.find(shaderType) == renderBatches.end()) {
					renderBatches[shaderType] = std::vector<RenderBatch>();
				}
				renderBatches[shaderType].push_back(pair.second);
			}
		}
	}

	for (const auto& pair : transparentQueues) {
		const ShaderType shaderType = pair.first;
		const auto& queue = pair.second;

		if (queue.commands.empty()) continue;

		std::unordered_map<BatchKey, RenderBatch, KeyHasher> batchMap;

		for (const auto& command : queue.commands) {
			if (!command.mesh || !command.material) continue;

			BatchKey key{
				command.mesh->getModel()->GetID(),
				command.material->GetId(),
				shaderType,
				true, 
				command.specialData.isAnimated
			};

			auto& batch = batchMap[key];

			if (batch.commands.empty()) {
				batch.mesh = command.mesh;
				batch.material = command.material;
				batch.shaderType = shaderType;
				batch.isTransparent = true;
			}

			batch.commands.push_back(command);

			if (batch.commands.size() >= maxInstancesPerBatch) {
				if (renderBatches.find(shaderType) == renderBatches.end()) {
					renderBatches[shaderType] = std::vector<RenderBatch>();
				}
				renderBatches[shaderType].push_back(batch);
				batch.commands.clear();
			}
		}

		for (auto& pair : batchMap) {
			if (!pair.second.commands.empty()) {
				if (renderBatches.find(shaderType) == renderBatches.end()) {
					renderBatches[shaderType] = std::vector<RenderBatch>();
				}

				if (pair.second.isTransparent) {
					std::sort(pair.second.commands.begin(), pair.second.commands.end(),
						[](const RenderCommand& a, const RenderCommand& b) {
							return a.distanceToCamera > b.distanceToCamera;
						});
				}

				renderBatches[shaderType].push_back(pair.second);
			}
		}
	}
}

void RenderManager::RenderBatches(bool transparent, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	for (const auto& pair : renderBatches) {
		ShaderType shaderType = pair.first;
		const auto& batches = pair.second;

		for (const auto& batch : batches) {
			if (batch.isTransparent != transparent)
				continue;

			if (instancedRenderingEnabled && batch.commands.size() > 1 && !batch.commands[0].specialData.isAnimated) {
				RenderInstanced(batch, viewMatrix, projectionMatrix);
			}
			else {
				RenderBatchStandard(batch, viewMatrix, projectionMatrix);
			}
		}
	}
}

void RenderManager::RenderBatchStandard(const RenderBatch& batch, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (batch.commands.empty() || !batch.mesh || !batch.material)
		return;

	START_TIMING("RenderBatchStandard");
	RENDER_STATS.BeginBatch("Standard_" + std::to_string(batch.commands.size()));

	Shaders* shader = ShaderManager::GetInstance().GetShader(batch.shaderType);
	if (!shader) {
		END_TIMING("RenderBatchStandard");
		RENDER_STATS.EndBatch();
		return;
	}

	SetShaderState(shader, viewMatrix, projectionMatrix);

	SetMaterialState(batch.material, shader);

	SetMeshState(batch.mesh);

	if (batch.shaderType == ShaderType::PBR) {
#ifndef _BUILD
		shader->SetUniform("viewPos", Application->camera->GetTransform().GetPosition());
#else
		shader->SetUniform("viewPos", Application->root->mainCamera->GetTransform()->GetPosition());
#endif
	}

	for (const auto& command : batch.commands) {
		shader->SetUniform("model", command.modelMatrix);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(command.modelMatrix)));
		shader->SetUniform("normalMatrix", normalMatrix);

		if (command.specialData.isAnimated && command.gameObject->HasComponent<MeshRenderer>()) {
			command.gameObject->GetComponent<MeshRenderer>()->SetUpAnimationProperties(shader);
		}

		if (batch.shaderType == ShaderType::PBR && command.gameObject->HasComponent<MeshRenderer>()) {
#ifndef _BUILD
			command.gameObject->GetComponent<MeshRenderer>()->SetupLightProperties(
				shader, Application->camera->GetTransform().GetPosition());
#else
			command.gameObject->GetComponent<MeshRenderer>()->SetupLightProperties(
				shader, Application->root->mainCamera->GetTransform()->GetPosition());
#endif
		}

		int triangleCount = command.mesh->getModel()->GetModelData().indexData.size() / 3;
		glDrawElements(GL_TRIANGLES, command.mesh->getModel()->GetModelData().indexData.size(),
			GL_UNSIGNED_INT, nullptr);
		RECORD_DRAWCALL(false, 1, triangleCount);
	}

	RENDER_STATS.EndBatch();
	END_TIMING("RenderBatchStandard");
}

void RenderManager::RenderInstanced(const RenderBatch& batch, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (batch.commands.empty() || !batch.mesh || !batch.material) return;

	START_TIMING("RenderInstanced");
	RENDER_STATS.BeginBatch("Instanced_" + std::to_string(batch.commands.size()));

	Shaders* shader = ShaderManager::GetInstance().GetShader(batch.shaderType);
	if (!shader) {
		END_TIMING("RenderInstanced");
		RENDER_STATS.EndBatch();
		return;
	}

	SetShaderState(shader, viewMatrix, projectionMatrix);

	shader->SetUniform("isInstanced", 1);

	SetMaterialState(batch.material, shader);

	SetMeshState(batch.mesh);

	if (batch.shaderType == ShaderType::PBR) {
		shader->SetUniform("viewPos", Application->camera->GetTransform().GetPosition());

		if (!batch.commands.empty() && batch.commands[0].gameObject) {
			batch.commands[0].gameObject->GetComponent<MeshRenderer>()->SetupLightProperties(
				shader, Application->camera->GetTransform().GetPosition());
		}
	}

	START_TIMING("PrepareInstanceData");
	std::vector<InstanceData> instanceData;
	batch.GenerateInstanceData(instanceData);
	END_TIMING("PrepareInstanceData");

	START_TIMING("UpdateInstanceBuffer");
	GLuint instanceBuffer = 0;
	unsigned int meshId = batch.mesh->getModel()->GetID();

	auto bufferIt = instanceBuffers.find(meshId);
	if (bufferIt != instanceBuffers.end()) {
		instanceBuffer = bufferIt->second;
		UpdateInstanceBuffer(instanceBuffer, instanceData);
	}
	else {
		instanceBuffer = CreateInstanceBuffer(instanceData);
		instanceBuffers[meshId] = instanceBuffer;
	}
	END_TIMING("UpdateInstanceBuffer");

	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

	const GLint matrixAttrStart = 7;
	const GLint colorAttrLoc = 11;
	const GLint normalMatrixAttrStart = 12; 

	for (int i = 0; i < 4; i++) {
		GLuint attrLoc = matrixAttrStart + i;
		glEnableVertexAttribArray(attrLoc);
		glVertexAttribPointer(attrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
			(void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(attrLoc, 1);
	}

	glEnableVertexAttribArray(colorAttrLoc);
	glVertexAttribPointer(colorAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
		(void*)(sizeof(float) * 16));
	glVertexAttribDivisor(colorAttrLoc, 1);

	size_t normalMatrixOffset = sizeof(float) * 20; 
	for (int i = 0; i < 3; i++) {
		GLuint attrLoc = normalMatrixAttrStart + i;
		glEnableVertexAttribArray(attrLoc);
		glVertexAttribPointer(attrLoc, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
			(void*)(normalMatrixOffset + sizeof(float) * 3 * i));
		glVertexAttribDivisor(attrLoc, 1);
	}

	auto modelData = batch.mesh->getModel()->GetModelData();
	int triangleCount = modelData.indexData.size() / 3;

	START_TIMING("DrawElementsInstanced");
	glDrawElementsInstanced(GL_TRIANGLES, modelData.indexData.size(), GL_UNSIGNED_INT, nullptr,
		instanceData.size());
	END_TIMING("DrawElementsInstanced");

	RECORD_DRAWCALL(true, instanceData.size(), triangleCount);

	for (int i = 0; i < 4; i++) {
		glVertexAttribDivisor(matrixAttrStart + i, 0);
		glDisableVertexAttribArray(matrixAttrStart + i);
	}

	glVertexAttribDivisor(colorAttrLoc, 0);
	glDisableVertexAttribArray(colorAttrLoc);

	for (int i = 0; i < 3; i++) {
		glVertexAttribDivisor(normalMatrixAttrStart + i, 0);
		glDisableVertexAttribArray(normalMatrixAttrStart + i);
	}

	shader->SetUniform("isInstanced", 0);

	RENDER_STATS.EndBatch();
	END_TIMING("RenderInstanced");
}

void RenderManager::PrintFrameStats() {
	if (RenderStats::GetInstance().GetVerbosityLevel() >= RenderStats::VerbosityLevel::BASIC) {
		std::cout << RenderStats::GetInstance().GetStatsReport() << std::endl;
	}
}

void RenderManager::SetShaderState(Shaders* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!shader) return;

	if (shader != currentBoundShader) {
		shader->Bind();
		RENDER_STATS.RecordShaderChange();

		shader->SetUniform("view", viewMatrix);
		shader->SetUniform("projection", projectionMatrix);

		currentBoundMaterialId = 0;
		currentBoundMeshId = 0;
		currentBoundShader = shader;
	}
}

void RenderManager::SetMaterialState(const std::shared_ptr<Material>& material, Shaders* shader) {
	if (!material || !shader) return;

	unsigned int materialId = material->GetId();
	if (materialId != currentBoundMaterialId) {
		material->bind();
		RENDER_STATS.RecordMaterialChange();

		shader->SetUniform("albedoColor", material->GetColor());

		if (material->GetShaderType() == ShaderType::PBR) {
			shader->SetUniform("metallicFactor", material->metallic);
			shader->SetUniform("roughnessFactor", material->roughness);
			shader->SetUniform("aoFactor", material->ao);
			shader->SetUniform("tonemapStrength", material->tonemapStrength);

			auto imagePtr = material->getImage();
			if (imagePtr && imagePtr->id() != 0) {
				shader->SetUniform("u_HasAlbedoMap", 1);
				shader->SetUniform("albedoMap", 0);
			}
			else {
				shader->SetUniform("u_HasAlbedoMap", 0);
			}

			auto normalMapPtr = material->getNormalMap();
			if (normalMapPtr && normalMapPtr->id() != 0) {
				shader->SetUniform("u_HasNormalMap", 1);
				shader->SetUniform("normalMap", 1);
			}
			else {
				shader->SetUniform("u_HasNormalMap", 0);
			}

			auto metallicMapPtr = material->getMetallicMap();
			if (metallicMapPtr && metallicMapPtr->id() != 0) {
				shader->SetUniform("u_HasMetallicMap", 1);
				shader->SetUniform("metallicMap", 2);
			}
			else {
				shader->SetUniform("u_HasMetallicMap", 0);
			}

			auto roughnessMapPtr = material->getRoughnessMap();
			if (roughnessMapPtr && roughnessMapPtr->id() != 0) {
				shader->SetUniform("u_HasRoughnessMap", 1);
				shader->SetUniform("roughnessMap", 3);
			}
			else {
				shader->SetUniform("u_HasRoughnessMap", 0);
			}

			auto aoMapPtr = material->getAoMap();
			if (aoMapPtr && aoMapPtr->id() != 0) {
				shader->SetUniform("u_HasAoMap", 1);
				shader->SetUniform("aoMap", 4);
			}
			else {
				shader->SetUniform("u_HasAoMap", 0);
			}
		}
		else if (material->GetShaderType() == ShaderType::UNLIT) {
			auto imagePtr = material->getImage();
			if (imagePtr && imagePtr->id() != 0) {
				shader->SetUniform("u_HasTexture", 1);
				shader->SetUniform("texture1", 0);
			}
			else {
				shader->SetUniform("u_HasTexture", 0);
			}
		}

		currentBoundMaterialId = materialId;
	}
}

void RenderManager::SetMeshState(const std::shared_ptr<Mesh>& mesh) {
	if (!mesh) return;

	unsigned int meshId = mesh->getModel()->GetID();
	if (meshId != currentBoundMeshId) {
		glBindVertexArray(mesh->getModel()->GetModelData().vA);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getModel()->GetModelData().iBID);
		RENDER_STATS.RecordVAOBinding();
		currentBoundMeshId = meshId;
	}
}

void RenderManager::RestoreState() {
	if (currentShader) {
		currentShader->UnBind();
		currentShader = nullptr;
	}

	if (currentMaterial) {
		currentMaterial->unbind();
		currentMaterial = nullptr;
	}

	if (currentMesh) {
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		currentMesh = nullptr;
	}

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}

GLuint RenderManager::CreateInstanceBuffer(const std::vector<InstanceData>& instances) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW);
	return buffer;
}

void RenderManager::UpdateInstanceBuffer(GLuint buffer, const std::vector<InstanceData>& instances) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	size_t requiredSize = instances.size() * sizeof(InstanceData);

	if (instanceBufferSizes.find(buffer) == instanceBufferSizes.end() ||
		requiredSize > instanceBufferSizes[buffer]) {
		glBufferData(GL_ARRAY_BUFFER, requiredSize, instances.data(), GL_DYNAMIC_DRAW);
		instanceBufferSizes[buffer] = requiredSize;
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, requiredSize, instances.data());
	}
}

void RenderManager::ResetStateTracking() {
	currentBoundShader = nullptr;
	currentBoundMaterialId = 0;
	currentBoundMeshId = 0;
}

void RenderManager::SaveGLState() {
	glGetIntegerv(GL_CURRENT_PROGRAM, &stateCache.lastProgram);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &stateCache.lastVAO);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &stateCache.lastElementArrayBuffer);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &stateCache.lastActiveTexture);
	glGetBooleanv(GL_BLEND, (GLboolean*)&stateCache.blendEnabled);
	glGetIntegerv(GL_BLEND_SRC, &stateCache.blendSrc);
	glGetIntegerv(GL_BLEND_DST, &stateCache.blendDst);
}

void RenderManager::RestoreGLState() {
	if (stateCache.blendEnabled) {
		glEnable(GL_BLEND);
		glBlendFunc(stateCache.blendSrc, stateCache.blendDst);
	}
	else {
		glDisable(GL_BLEND);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stateCache.lastElementArrayBuffer);
	glBindVertexArray(stateCache.lastVAO);
	glActiveTexture(stateCache.lastActiveTexture);

	if (stateCache.lastProgram > 0) {
		glUseProgram(stateCache.lastProgram);
	}
	else {
		glUseProgram(0);
	}
}