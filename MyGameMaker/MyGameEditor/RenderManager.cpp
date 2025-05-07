#include "RenderManager.h"
#include <iostream>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyGameEngine/ShaderManager.h"

RenderManager& RenderManager::GetInstance() {
	static RenderManager instance;
	return instance;
}

bool CheckExtension(const char* extension) {
	return glewIsSupported(extension) == GL_TRUE;
}

bool RenderManager::Initialize() {
	if (!GLEW_VERSION_4_3) {
		LOG(LogType::LOG_ERROR, "Error: Se requiere OpenGL 4.3 o superior");
		return false;
	}

	if (!GLEW_ARB_bindless_texture) {
		LOG(LogType::LOG_ERROR, "Bindless textures not supported!");
	}
	if (!GLEW_ARB_gpu_shader_int64) {
		LOG(LogType::LOG_ERROR, "64-bit integers not supported in shaders!");
	}

	glCreateVertexArrays(1, &defaultVAO);
	glGenQueries(3, timeQueries);

	if (!BindlessManager::GetInstance().Initialize()) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo inicializar BindlessManager");
		return false;
	}

	if (!GPUDrivenRenderer::GetInstance().Initialize()) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo inicializar GPUDrivenRenderer");
		return false;
	}

	if (!ForwardPlusLighting::GetInstance().Initialize(windowWidth, windowHeight)) {
		LOG(LogType::LOG_WARNING, "Warning: No se pudo inicializar ForwardPlusLighting, se utilizará forward rendering estándar");
		useForwardPlus = false;
	}

	if (!InitializeShaders()) {
		LOG(LogType::LOG_ERROR, "Error: No se pudieron inicializar shaders");
		return false;
	}

	return true;
}

void RenderManager::Shutdown() {
	if (defaultVAO) glDeleteVertexArrays(1, &defaultVAO);
	if (timeQueries[0]) glDeleteQueries(3, timeQueries);

	GPUDrivenRenderer::GetInstance().Shutdown();
	if (useForwardPlus) {
		ForwardPlusLighting::GetInstance().Shutdown();
	}
	BindlessManager::GetInstance().Shutdown();

	defaultVAO = 0;
	timeQueries[0] = timeQueries[1] = timeQueries[2] = 0;
	pbrShader = 0;
	unlitShader = 0;

	queuedObjects.clear();
	instanceGroups.clear();
}

void RenderManager::BeginFrame() {
	BeginGPUQuery();

	queuedObjects.clear();
	instanceGroups.clear();

	BindlessManager::GetInstance().ClearInstances();
	GPUDrivenRenderer::GetInstance().BeginFrame();

	stats = RenderStatistics();
}

void RenderManager::EndFrame() {
	EndGPUQuery();

	stats.gpuTimeMs = GetGPUTimeMs();

	queuedObjects.clear();
	instanceGroups.clear();

	GPUDrivenRenderer::GetInstance().EndFrame();
}

void RenderManager::SubmitGameObject(GameObject* gameObject) {
	if (!gameObject || !gameObject->IsActive()) return;

	stats.totalGameObjects++;

	queuedObjects.push_back(gameObject);

	for (const auto& child : gameObject->GetChildren()) {
		if (child && child->IsActive()) {
			SubmitGameObject(child.get());
		}
	}
}

void RenderManager::RenderScene(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos) {
	if (queuedObjects.empty()) return;

	for (auto* obj : queuedObjects) {
		ProcessGameObject(obj);
	}

	if (useForwardPlus) {
		ForwardPlusLighting::GetInstance().CollectLights(queuedObjects);
		ForwardPlusLighting::GetInstance().UpdateLights();
		ForwardPlusLighting::GetInstance().PerformLightCulling(viewMatrix, projMatrix);

		stats.totalLights = ForwardPlusLighting::GetInstance().GetTotalLights();
		stats.visibleLights = ForwardPlusLighting::GetInstance().GetVisibleLights();
	}

	CreateInstanceGroups();

	GPUDrivenRenderer::GetInstance().PrepareDrawCommands(viewMatrix, projMatrix, cameraPos);

	GPUDrivenRenderer::GetInstance().RenderAll(viewMatrix, projMatrix, cameraPos);

	BindlessManager::GetInstance().EndFrame();

	stats.visibleGameObjects = GPUDrivenRenderer::GetInstance().GetVisibleInstanceCount();
	stats.totalDrawCalls = GPUDrivenRenderer::GetInstance().GetTotalDrawCommands();
}

void RenderManager::RenderFromCamera(CameraComponent* camera) {
	if (!camera) return;

	glm::mat4 viewMatrix = camera->view();
	glm::mat4 projMatrix = camera->projection();
	glm::vec3 cameraPos = camera->owner->GetTransform()->GetPosition();

	RenderScene(viewMatrix, projMatrix, cameraPos);
}

void RenderManager::SetWindowSize(int width, int height) {
	windowWidth = width;
	windowHeight = height;

	if (useForwardPlus) {
		ForwardPlusLighting::GetInstance().Resize(width, height);
	}
}

void RenderManager::SetUseGPUCulling(bool enable) {
	GPUDrivenRenderer::GetInstance().SetUseGPUCulling(enable);
}

void RenderManager::SetUseOcclusionCulling(bool enable) {
	GPUDrivenRenderer::GetInstance().SetUseOcclusionCulling(enable);
}

bool RenderManager::InitializeShaders() {
	pbrShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::PBR);
	if (pbrShader == 0) {
		LOG(LogType::LOG_WARNING, "Warning: Shader PBR no encontrado en ShaderManager");
		return false;
	}

	unlitShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::UNLIT);
	if (unlitShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: UNLIT no encontrado en ShaderManager");
		return false;
	}

	return unlitShader != 0 && pbrShader != 0;
}

void RenderManager::ProcessGameObject(GameObject* gameObject) {
	if (!gameObject || !gameObject->IsActive()) return;

	if (gameObject->HasComponent<MeshRenderer>()) {
		MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>();

		auto mesh = renderer->GetMesh();
		auto material = renderer->GetMaterial();

		if (mesh && material) {
			uint32_t meshIndex = BindlessManager::GetInstance().RegisterMesh(mesh.get());
			uint32_t materialIndex = BindlessManager::GetInstance().RegisterMaterial(material.get());

			if (meshIndex != UINT32_MAX && materialIndex != UINT32_MAX) {
				GPUInstance instance;
				instance.modelMatrix = gameObject->GetTransform()->GetMatrix();
				instance.prevModelMatrix = instance.modelMatrix;
				instance.objectData = glm::vec4(1.0f);
				instance.meshIndex = meshIndex;
				instance.materialIndex = materialIndex;
				instance.objectId = gameObject->GetID().GetValue();
				instance.flags = 0;

				MeshMaterialKey key{ meshIndex, materialIndex };

				instanceGroups[key].push_back(instance);
			}
		}
	}
}

void RenderManager::CreateInstanceGroups() {
	for (const auto& group : instanceGroups) {
		const MeshMaterialKey& key = group.first;
		const auto& instances = group.second;

		if (!instances.empty()) {
			glm::vec4 boundingSphere(0.0f, 0.0f, 0.0f, 1000.0f);
			GPUDrivenRenderer::GetInstance().AddInstanceGroup(
				key.meshIndex,
				key.materialIndex,
				boundingSphere,
				instances
			);
		}
	}

	BindlessManager::GetInstance().UpdateBuffers();
}

void RenderManager::BeginGPUQuery() {
	currentQueryIndex = (currentQueryIndex + 1) % 3;
	glBeginQuery(GL_TIME_ELAPSED, timeQueries[currentQueryIndex]);
}

void RenderManager::EndGPUQuery() {
	glEndQuery(GL_TIME_ELAPSED);
}

float RenderManager::GetGPUTimeMs() {
	int previousIndex = (currentQueryIndex + 1) % 3;

	GLint available = 0;
	glGetQueryObjectiv(timeQueries[previousIndex], GL_QUERY_RESULT_AVAILABLE, &available);

	if (available) {
		GLuint64 time;
		glGetQueryObjectui64v(timeQueries[previousIndex], GL_QUERY_RESULT, &time);
		return static_cast<float>(time) / 1000000.0f;
	}

	return 0.0f;
}