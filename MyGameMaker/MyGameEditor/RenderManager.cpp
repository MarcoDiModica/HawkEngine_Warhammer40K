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
		LOG(LogType::LOG_ERROR, "Error: No se pudieron inicializar shaders bindless PBR");
		return false;
	}

	LOG(LogType::LOG_INFO, "===== OpenGL System Information =====");
	LOG(LogType::LOG_INFO, "OpenGL Version: %s", glGetString(GL_VERSION));
	LOG(LogType::LOG_INFO, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOG(LogType::LOG_INFO, "Vendor: %s", glGetString(GL_VENDOR));
	LOG(LogType::LOG_INFO, "Renderer: %s", glGetString(GL_RENDERER));

	bool hasBindlessTexture = CheckExtension("GL_ARB_bindless_texture");
	bool hasShaderStorageBuffer = CheckExtension("GL_ARB_shader_storage_buffer_object");
	bool hasMultiDrawIndirect = CheckExtension("GL_ARB_multi_draw_indirect");
	bool hasComputeShader = CheckExtension("GL_ARB_compute_shader");
	bool hasGPUShaderInt64 = CheckExtension("GL_ARB_gpu_shader_int64");

	LOG(LogType::LOG_INFO, "==== Required extensions status ====");
	LOG(LogType::LOG_INFO, "GL_ARB_bindless_texture: %s", hasBindlessTexture ? "YES" : "NO");
	LOG(LogType::LOG_INFO, "GL_ARB_shader_storage_buffer_object: %s", hasShaderStorageBuffer ? "YES" : "NO");
	LOG(LogType::LOG_INFO, "GL_ARB_multi_draw_indirect: %s", hasMultiDrawIndirect ? "YES" : "NO");
	LOG(LogType::LOG_INFO, "GL_ARB_compute_shader: %s", hasComputeShader ? "YES" : "NO");
	LOG(LogType::LOG_INFO, "GL_ARB_gpu_shader_int64: %s", hasGPUShaderInt64 ? "YES" : "NO");

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
	bindlessPBRShader = 0;

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

void RenderManager::RenderDebugQuad() {
	// Usar el shader debug
	GLuint debugShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::DEBUG);
	glUseProgram(debugShader);

	// Renderizar un quad simple
	glBindVertexArray(defaultVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderManager::RenderScene(const glm::mat4& viewMatrix, const glm::mat4& projMatrix/*, const Frustum& frustum*/) {
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

	GPUDrivenRenderer::GetInstance().PrepareDrawCommands(/*frustum*/);

	GLuint bindlessPBRShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::BINDLESS_PBR);

	if (bindlessPBRShader != 0) {
		if (useForwardPlus) {
			glUseProgram(bindlessPBRShader);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
				ForwardPlusLighting::GetInstance().GetPointLightBuffer());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
				ForwardPlusLighting::GetInstance().GetDirectionalLightBuffer());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5,
				ForwardPlusLighting::GetInstance().GetLightGridBuffer());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
				ForwardPlusLighting::GetInstance().GetLightIndicesBuffer());

			glUniform1i(glGetUniformLocation(bindlessPBRShader, "useForwardPlus"), useForwardPlus ? 1 : 0);
			glUniform2i(glGetUniformLocation(bindlessPBRShader, "screenSize"), windowWidth, windowHeight);
			glUniform1i(glGetUniformLocation(bindlessPBRShader, "tileSize"),
				ForwardPlusLighting::GetInstance().GetTileSize());

			glm::vec3 viewPos = glm::vec3(glm::inverse(viewMatrix)[3]);
			glUniform3fv(glGetUniformLocation(bindlessPBRShader, "viewPos"), 1, glm::value_ptr(viewPos));

			glUseProgram(0);
		}
	}

	LOG(LogType::LOG_INFO, "Rendering %d objects, %d instances",
		queuedObjects.size(),
		GPUDrivenRenderer::GetInstance().GetVisibleInstanceCount());

	BindlessManager::GetInstance().EndFrame();

	GPUDrivenRenderer::GetInstance().RenderAll(viewMatrix, projMatrix);

	stats.visibleGameObjects = GPUDrivenRenderer::GetInstance().GetVisibleInstanceCount();
	stats.totalDrawCalls = GPUDrivenRenderer::GetInstance().GetTotalDrawCommands();

}

void RenderManager::RenderFromCamera(CameraComponent* camera) {
	if (!camera) return;

	const glm::mat4& viewMatrix = camera->view();
	const glm::mat4& projMatrix = camera->projection();
	//const Frustum& frustum = camera->frustum;

	RenderScene(viewMatrix, projMatrix/*, frustum*/);
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
	bindlessPBRShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::BINDLESS_PBR);

	if (bindlessPBRShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: Shader bindless PBR no encontrado en ShaderManager");
		return false;
	}

	return true;
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
				instance.objectData = glm::vec4(0.0f);
				instance.meshIndex = meshIndex;
				instance.materialIndex = materialIndex;
				instance.objectId = gameObject->GetID().GetValue();
				instance.flags = 0;

				instanceGroups[meshIndex].push_back(instance);
			}
		}
	}
}

void RenderManager::CreateInstanceGroups() {
	BindlessManager::GetInstance().UpdateBuffers();

	for (const auto& group : instanceGroups) {
		uint32_t meshIndex = group.first;
		const auto& instances = group.second;

		if (!instances.empty()) {
			// Calcular bounding sphere aproximada para el grupo
			// Simplificado: usamos una esfera con centro en 0,0,0 y radio suficiente
			// En una implementación real calcularías esto basado en las posiciones de los objetos
			glm::vec4 boundingSphere(0.0f, 0.0f, 0.0f, 1000.0f);

			GPUDrivenRenderer::GetInstance().AddInstanceGroup(meshIndex, boundingSphere, instances);
		}
	}
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