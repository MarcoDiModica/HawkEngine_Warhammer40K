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

extern App* Application;

RenderManager& RenderManager::GetInstance() {
	static RenderManager instance;
	return instance;
}

RenderManager::RenderManager() {
	//Initialize default values
	instancedRenderingEnabled = true;
}

RenderManager::~RenderManager() {
	Cleanup();
}

bool RenderManager::Initialize() {
	// Create any necessary OpenGL objects/buffers
	ClearRenderQueues();
	return true;
}

void RenderManager::Cleanup() {
	// Clean up any OpenGL objects
	for (auto& buffer : instanceBuffers) {
		if (buffer.second != 0) {
			glDeleteBuffers(1, &buffer.second);
		}
	}
	instanceBuffers.clear();

	ClearRenderQueues();
}

void RenderManager::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
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
		return;
	}

	SaveGLState();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (instancedRenderingEnabled) {
		ProcessBatches();
	}

	for (const auto& pair : opaqueQueues) {
		TheRenderQueue(pair.second, viewMatrix, projectionMatrix);
	}

	for (const auto& pair : transparentQueues) {
		glEnable(GL_BLEND);
		TheRenderQueue(pair.second, viewMatrix, projectionMatrix);
	}

	RestoreGLState();
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

void RenderManager::TheRenderQueue(const RenderQueue& queue, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (queue.IsEmpty()) return;

	if (instancedRenderingEnabled) {
		ShaderType shaderType = ShaderType::PBR; 

		if (!queue.commands.empty() && queue.commands[0].material) {
			shaderType = queue.commands[0].material->GetShaderType();
		}

		auto batchIt = renderBatches.find(shaderType);
		if (batchIt != renderBatches.end()) {
			const auto& batches = batchIt->second;

			for (const auto& batch : batches) {
				if (batch.isTransparent != queue.commands[0].isTransparent) {
					continue;
				}

				RenderInstanced(batch, viewMatrix, projectionMatrix);
			}
		}
	}
	else {
		for (const auto& command : queue.commands) {
			RenderStandard(command, viewMatrix, projectionMatrix);
		}
	}
}

void RenderManager::RenderInstanced(const RenderBatch& batch, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (batch.commands.empty() || !batch.mesh || !batch.material) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(batch.shaderType);
	if (!shader) return;

	SetShaderState(shader, viewMatrix, projectionMatrix);

	shader->SetUniform("isInstanced", 1);

	SetMaterialState(batch.material);

	SetMeshState(batch.mesh);

	std::vector<InstanceData> instanceData;
	BuildInstanceData(batch, instanceData);

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

	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

	for (int i = 0; i < 4; i++) {
		GLuint attribLocation = 7 + i;
		glEnableVertexAttribArray(attribLocation);
		glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
			(void*)(sizeof(float) * 4 * i));
		glVertexAttribDivisor(attribLocation, 1);
	}

	GLuint colorAttribLocation = 11;
	glEnableVertexAttribArray(colorAttribLocation);
	glVertexAttribPointer(colorAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
		(void*)(sizeof(float) * 16));
	glVertexAttribDivisor(colorAttribLocation, 1);

	auto modelData = batch.mesh->getModel()->GetModelData();
	glDrawElementsInstanced(GL_TRIANGLES, modelData.indexData.size(), GL_UNSIGNED_INT, nullptr, instanceData.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader->SetUniform("isInstanced", 0);
}

void RenderManager::RenderStandard(const RenderCommand& command, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!command.mesh || !command.material) return;

	Shaders* shader = ShaderManager::GetInstance().GetShader(command.material->GetShaderType());
	if (!shader) return;

	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	shader->Bind();

	shader->SetUniform("model", command.modelMatrix);
	shader->SetUniform("view", viewMatrix);
	shader->SetUniform("projection", projectionMatrix);

	shader->SetUniform("albedoColor", command.material->GetColor());

	if (command.material->GetShaderType() == ShaderType::PBR) {
		shader->SetUniform("metallicFactor", command.material->metallic);
		shader->SetUniform("roughnessFactor", command.material->roughness);
		shader->SetUniform("aoFactor", command.material->ao);

		command.material->bind();

		if (command.gameObject) {
			command.gameObject->GetComponent<MeshRenderer>()->SetupLightProperties(shader, Application->camera->GetTransform().GetPosition());

			command.gameObject->GetComponent<MeshRenderer>()->SetUpAnimationProperties(shader);
		}
	}

	command.gameObject->GetComponent<MeshRenderer>()->BindMeshForRendering();

	command.gameObject->GetComponent<MeshRenderer>()->DrawMeshElements();

	command.gameObject->GetComponent<MeshRenderer>()->UnbindMeshAfterRendering();

	if (lastProgram > 0) {
		glUseProgram(lastProgram);
	}
}

void RenderManager::SetShaderState(Shaders* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!shader) return;

	if (shader != currentShader) {
		shader->Bind();
		currentShader = shader;

		shader->SetUniform("view", viewMatrix);
		shader->SetUniform("projection", projectionMatrix);
	}
}

void RenderManager::SetMaterialState(const std::shared_ptr<Material>& material) {
	if (!material) return;

	if (material != currentMaterial) {
		material->bind();
		currentMaterial = material;
	}
}

void RenderManager::SetMeshState(const std::shared_ptr<Mesh>& mesh) {
	if (!mesh) return;

	if (mesh != currentMesh) {
		glBindVertexArray(mesh->getModel()->GetModelData().vA);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getModel()->GetModelData().iBID);
		currentMesh = mesh;
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

void RenderManager::BuildInstanceData(const RenderBatch& batch, std::vector<InstanceData>& instanceData) {
	instanceData.clear();
	instanceData.reserve(batch.commands.size());

	for (const auto& command : batch.commands) {
		InstanceData data;
		data.modelMatrix = command.modelMatrix;

		if (command.material) {
			data.color = command.material->GetColor();
		}
		else {
			data.color = glm::vec4(1.0f);
		}

		instanceData.push_back(data);
	}
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

GLuint RenderManager::CreateInstanceBuffer(const std::vector<InstanceData>& instances) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW);
	return buffer;
}

void RenderManager::UpdateInstanceBuffer(GLuint buffer, const std::vector<InstanceData>& instances) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW);
}

void RenderManager::ProcessBatches() {
	renderBatches.clear();

	for (const auto& pair : opaqueQueues) {
		ShaderType shaderType = pair.first;
		const auto& queue = pair.second;

		std::unordered_map<unsigned int, std::unordered_map<unsigned int, RenderBatch>> batchMap;

		for (const auto& command : queue.commands) {
			unsigned int meshId = command.mesh->getModel()->GetID();
			unsigned int materialId = command.material->GetId();

			auto& batch = batchMap[meshId][materialId];
			if (batch.commands.empty()) {
				batch.mesh = command.mesh;
				batch.material = command.material;
				batch.shaderType = shaderType;
				batch.isTransparent = false;
			}

			batch.commands.push_back(command);

			if (batch.commands.size() >= maxInstancesPerBatch) {
				renderBatches[shaderType].push_back(batch);
				batch.commands.clear();
			}
		}

		for (auto& meshPair : batchMap) {
			for (auto& materialPair : meshPair.second) {
				if (!materialPair.second.commands.empty()) {
					renderBatches[shaderType].push_back(materialPair.second);
				}
			}
		}
	}

	for (const auto& pair : transparentQueues) {
		ShaderType shaderType = pair.first;
		const auto& queue = pair.second;

		std::unordered_map<unsigned int, std::unordered_map<unsigned int, RenderBatch>> batchMap;

		for (const auto& command : queue.commands) {
			unsigned int meshId = command.mesh->getModel()->GetID();
			unsigned int materialId = command.material->GetId();

			auto& batch = batchMap[meshId][materialId];
			if (batch.commands.empty()) {
				batch.mesh = command.mesh;
				batch.material = command.material;
				batch.shaderType = shaderType;
				batch.isTransparent = true;
			}

			batch.commands.push_back(command);

			if (batch.commands.size() >= maxInstancesPerBatch) {
				renderBatches[shaderType].push_back(batch);
				batch.commands.clear();
			}
		}

		for (auto& meshPair : batchMap) {
			for (auto& materialPair : meshPair.second) {
				if (!materialPair.second.commands.empty()) {
					renderBatches[shaderType].push_back(materialPair.second);
				}
			}
		}
	}
}