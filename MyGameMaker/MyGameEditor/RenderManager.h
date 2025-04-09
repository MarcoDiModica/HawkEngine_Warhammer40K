#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../MyGameEngine/Shaders.h"
#include "../MyGameEngine/Material.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/GameObject.h"
#include "RenderCommand.h"

class Camera;
class GameObject;
class CameraComponent;
class Transform_Component;

class RenderManager
{
public:
	static RenderManager& GetInstance();

	bool Initialize();

	void Cleanup();

	void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderFromCamera(CameraComponent* camera);
	void RenderEditor(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderGame();

	void SubmitMesh(GameObject* gameObject, const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material);
	void SubmitGameObject(GameObject* gameObject);

	void SortRenderCommands();

	void ClearRenderQueues();

	void SetInstancedRenderingEnabled(bool enabled) { instancedRenderingEnabled = enabled; }
	bool IsInstancedRenderingEnabled() const { return instancedRenderingEnabled; }

	void SetMaxInstancesPerBatch(int count) { maxInstancesPerBatch = count; }

	void SetFrustumCullingEnabled(bool enabled) { frustumCullingEnabled = enabled; }
	bool IsFrustumCullingEnabled() const { return frustumCullingEnabled; }

private:
	RenderManager();
	~RenderManager();

	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

	void TheRenderQueue(const RenderQueue& queue, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderInstanced(const RenderBatch& batch, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderStandard(const RenderCommand& command, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void SetShaderState(Shaders* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void SetMaterialState(const std::shared_ptr<Material>& material);
	void SetMeshState(const std::shared_ptr<Mesh>& mesh);
	void RestoreState();

	void BuildInstanceData(const RenderBatch& batch, std::vector<InstanceData>& instances);

	GLuint CreateInstanceBuffer(const std::vector<InstanceData>& instances);
	void UpdateInstanceBuffer(GLuint buffer, const std::vector<InstanceData>& instances);

	void ProcessBatches();

	struct GLStateCache {
		GLint lastProgram = 0;
		GLint lastVAO = 0;
		GLint lastElementArrayBuffer = 0;
		GLint lastActiveTexture = 0;
		bool blendEnabled = false;
		GLint blendSrc = 0;
		GLint blendDst = 0;
	} stateCache;

	void SaveGLState();

	void RestoreGLState();

	std::unordered_map<ShaderType, RenderQueue> opaqueQueues;
	std::unordered_map<ShaderType, RenderQueue> transparentQueues;

	std::unordered_map<ShaderType, std::vector<RenderBatch>> renderBatches;

	std::unordered_map<unsigned int, GLuint> instanceBuffers;

	Shaders* currentShader = nullptr;
	std::shared_ptr<Material> currentMaterial = nullptr;
	std::shared_ptr<Mesh> currentMesh = nullptr;

	bool instancedRenderingEnabled = true;
	int maxInstancesPerBatch = 100;
	bool frustumCullingEnabled = true;
};