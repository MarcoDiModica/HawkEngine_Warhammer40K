#pragma once
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/CameraComponent.h"
#include "BindlessManager.h"
#include "GPUDrivenRenderer.h"
#include "ForwardPlus.h"

// Clave compuesta para agrupar instancias por malla y material
struct MeshMaterialKey {
	uint32_t meshIndex;
	uint32_t materialIndex;

	bool operator==(const MeshMaterialKey& other) const {
		return meshIndex == other.meshIndex && materialIndex == other.materialIndex;
	}
};

// Función hash para la clave compuesta
namespace std {
	template<>
	struct hash<MeshMaterialKey> {
		size_t operator()(const MeshMaterialKey& k) const {
			return ((hash<uint32_t>()(k.meshIndex) ^
				(hash<uint32_t>()(k.materialIndex) << 1)) >> 1);
		}
	};
}

struct RenderStatistics {
	int totalGameObjects = 0;
	int visibleGameObjects = 0;
	int totalDrawCalls = 0;
	int totalLights = 0;
	int visibleLights = 0;
	int instanceCount = 0;
	float frameTimeMs = 0.0f;
	float gpuTimeMs = 0.0f;
};

class RenderManager {
public:
	static RenderManager& GetInstance();
	bool Initialize();
	void Shutdown();
	void BeginFrame();
	void EndFrame();
	void SubmitGameObject(GameObject* gameObject);
	void RenderDebugQuad();
	void RenderScene(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos);
	void RenderFromCamera(CameraComponent* camera);
	void SetWindowSize(int width, int height);
	void SetUseForwardPlus(bool enable) { useForwardPlus = enable; }
	void SetUseGPUCulling(bool enable);
	void SetUseOcclusionCulling(bool enable);
	const RenderStatistics& GetStatistics() const { return stats; }
private:
	RenderManager() = default;
	~RenderManager() = default;
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;
	bool InitializeShaders();
	void ProcessGameObject(GameObject* gameObject);
	void CreateInstanceGroups();
	void BeginGPUQuery();
	void EndGPUQuery();
	float GetGPUTimeMs();

	GLuint bindlessPBRShader = 0;
	GLuint bindlessUnlitShader = 0; // Añadido para UNLIT
	GLuint defaultVAO = 0;
	int windowWidth = 1280;
	int windowHeight = 720;
	std::vector<GameObject*> queuedObjects;

	// Modificado para usar la clave compuesta mesh+material
	std::unordered_map<MeshMaterialKey, std::vector<GPUInstance>> instanceGroups;

	bool useForwardPlus = true;
	GLuint timeQueries[3] = { 0 };
	int currentQueryIndex = 0;
	RenderStatistics stats;
};