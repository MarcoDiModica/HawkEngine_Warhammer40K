#include "RenderManager.h"
#include <iostream>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyGameEngine/ShaderManager.h"
#include "MyUIEngine/UICanvasComponent.h"
#include "MyUIEngine/UIImageComponent.h"
#include "MyUIEngine/UITransformComponent.h"

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
	BindlessManager::GetInstance().EndFrame();
}

void RenderManager::SubmitGameObject(GameObject* gameObject, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, CameraBase::Plane* frustumPlanes) {
	if (!gameObject || !gameObject->IsActive()) return;

	stats.totalGameObjects++;

	bool objectVisible = true;

	if (frustumPlanes && gameObject->HasComponent<MeshRenderer>()) {
		BoundingBox bbox = gameObject->boundingBox();

		CameraBase::FrustumIntersection result = TestFrustumAABB(bbox.min, bbox.max, frustumPlanes);

		if (result == CameraBase::FrustumIntersection::OUTSIDE) {
			objectVisible = false;
		}
	}

	if (objectVisible) {
		queuedObjects.push_back(gameObject);
	}

	for (const auto& child : gameObject->GetChildren()) {
		if (child && child->IsActive()) {
			SubmitGameObject(child.get(), viewMatrix, projMatrix, frustumPlanes);
		}
	}
}

CameraBase::FrustumIntersection RenderManager::TestFrustumAABB(const glm::vec3& bboxMin, const glm::vec3& bboxMax, CameraBase::Plane* frustumPlanes) {
	CameraBase::FrustumIntersection result = CameraBase::FrustumIntersection::INSIDE;

	for (int i = 0; i < 6; i++) {
		const CameraBase::Plane& plane = frustumPlanes[i];

		glm::vec3 p(bboxMin);
		if (plane.normal.x >= 0) p.x = bboxMax.x;
		if (plane.normal.y >= 0) p.y = bboxMax.y;
		if (plane.normal.z >= 0) p.z = bboxMax.z;

		glm::vec3 n(bboxMax);
		if (plane.normal.x >= 0) n.x = bboxMin.x;
		if (plane.normal.y >= 0) n.y = bboxMin.y;
		if (plane.normal.z >= 0) n.z = bboxMin.z;

		if (plane.distanceToPoint(p) < 0)
			return CameraBase::FrustumIntersection::OUTSIDE;

		if (plane.distanceToPoint(n) < 0)
			result = CameraBase::FrustumIntersection::INTERSECT;
	}

	return result;
}

void RenderManager::RenderScene(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos, CameraBase::Plane* frustrumPlanes) {
	if (queuedObjects.empty()) return;

	for (auto* obj : queuedObjects) {
		ProcessGameObject(obj);
	}

	if (useForwardPlus) {
		ForwardPlusLighting::GetInstance().CollectLights(queuedObjects);
		ForwardPlusLighting::GetInstance().UpdateLights();
		ForwardPlusLighting::GetInstance().PerformLightCulling(viewMatrix, projMatrix);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ForwardPlusLighting::GetInstance().GetPointLightBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ForwardPlusLighting::GetInstance().GetDirectionalLightBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ForwardPlusLighting::GetInstance().GetLightGridBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ForwardPlusLighting::GetInstance().GetLightIndicesBuffer());

		stats.totalLights = ForwardPlusLighting::GetInstance().GetTotalLights();
		stats.visibleLights = ForwardPlusLighting::GetInstance().GetVisibleLights();
	}

	CreateInstanceGroups();

	GPUDrivenRenderer::GetInstance().PrepareDrawCommands();

	GPUDrivenRenderer::GetInstance().RenderAll(viewMatrix, projMatrix, cameraPos);

	stats.visibleGameObjects = GPUDrivenRenderer::GetInstance().GetVisibleInstanceCount();
	stats.totalDrawCalls = GPUDrivenRenderer::GetInstance().GetTotalDrawCommands();
}

void RenderManager::RenderFromCamera(CameraComponent* camera) {
	if (!camera) return;

	glm::mat4 viewMatrix = camera->view();
	glm::mat4 projMatrix = camera->projection();
	glm::vec3 cameraPos = camera->owner->GetTransform()->GetPosition();
	CameraBase::Plane* frstrumPlanes = camera->GetPlanes();

	RenderScene(viewMatrix, projMatrix, cameraPos, frstrumPlanes);
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

	if (gameObject->HasComponent<UIImageComponent>()) {
		auto image = gameObject->GetComponent<UIImageComponent>();
		if (image) {
			glm::mat4 modelMatrix = image->GetModelMatrix();

			auto mesh = image->GetMesh();
			auto material = image->GetMaterial();

			if (mesh && material) {
				uint32_t meshIndex = BindlessManager::GetInstance().RegisterMesh(mesh.get());
				uint32_t materialIndex = BindlessManager::GetInstance().RegisterMaterial(material.get());

				if (meshIndex != UINT32_MAX && materialIndex != UINT32_MAX) {
					GPUInstance instance;
					instance.modelMatrix = modelMatrix;
					instance.prevModelMatrix = modelMatrix;
					instance.objectData = glm::vec4(1.0f);
					instance.meshIndex = 0;
					instance.materialIndex = materialIndex;
					instance.objectId = gameObject->GetID().GetValue();
					instance.flags = 0;

					MeshMaterialKey key{ 0, materialIndex };

					instanceGroups[key].push_back(instance);
				}
			}
		}
	}
}

void RenderManager::CreateInstanceGroups() {
	for (const auto& group : instanceGroups) {
		const MeshMaterialKey& key = group.first;
		const auto& instances = group.second;

		if (!instances.empty()) {

			GPUDrivenRenderer::GetInstance().AddInstanceGroup(
				key.meshIndex,
				key.materialIndex,
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