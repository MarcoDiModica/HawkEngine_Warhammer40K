#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/LightComponent.h"

struct GPUPointLight {
	glm::vec4 position;   // xyz = posición, w = radio
	glm::vec4 color;      // rgb = color, a = intensidad
	glm::vec4 attenuation; // x = constante, y = lineal, z = cuadrática, w = no usado
	uint32_t lightType;   // 0 = point, 1 = spot (futuro)
	uint32_t castShadow;  // 0 = no, 1 = yes
	uint32_t shadowMapIndex; // índice a shadowmap (si usa sombras)
	uint32_t padding;     // alineación
};

struct ShadowSettings {
	int shadowMapSize = 2048;
	float shadowBias = 0.005f;
	float shadowNormalBias = 0.01f;
	float shadowFarPlane = 100.0f;
	int pcfKernelSize = 3;
	bool softShadows = true;
	float cascadeLambda = 0.5f;
};

struct GPUDirectionalLight {
	glm::vec4 direction;  // xyz = dirección, w = intensidad
	glm::vec4 color;      // rgb = color, a = no usado
	glm::mat4 shadowMatrix; // matriz para shadowmap (si usa sombras)
	uint32_t castShadow;  // 0 = no, 1 = yes
	uint32_t shadowMapIndex; // índice a shadowmap (si usa sombras)
	uint32_t useCascades; // 0 = no, 1 = yes (futuro)
	uint32_t numCascades; // número de cascadas (futuro)
};

class ForwardPlusLighting {
public:
	static ForwardPlusLighting& GetInstance();

	bool Initialize(int windowWidth, int windowHeight);
	void Shutdown();
	void Resize(int width, int height);

	void CollectLights(const std::vector<GameObject*>& gameObjects);
	void UpdateLights();
	void PerformLightCulling(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

	void CPUFallbackCulling(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
	void SetTileSize(int size) { tileSize = size; RecalculateTiles(); }
	void SetMaxLightsPerTile(int max) { maxLightsPerTile = max; }
	void EnableClusteredLighting(bool enable) { useClusteredLighting = enable; }

	GLuint GetPointLightBuffer() const { return pointLightBuffer; }
	GLuint GetDirectionalLightBuffer() const { return directionalLightBuffer; }
	GLuint GetLightGridBuffer() const { return lightGridBuffer; }
	GLuint GetLightIndicesBuffer() const { return lightIndicesBuffer; }

	int GetTilesX() const { return tilesX; }
	int GetTilesY() const { return tilesY; }
	int GetTileSize() const { return tileSize; }
	int GetMaxLightsPerTile() const { return maxLightsPerTile; }

	GLuint GetShadowFBO() const { return shadowMapFBO; }
	GLuint GetPointShadowMapArray() const { return pointShadowMapArray; }
	GLuint GetDirectionalShadowMapArray() const { return directionalShadowMapArray; }
	int GetShadowMapSize() const { return shadowMapSize; }
	int GetMaxPointLightShadows() const { return maxPointLightShadows; }
	int GetMaxDirectionalLightShadows() const { return maxDirectionalLightShadows; }
	int GetMaxCascades() const { return maxCascades; }
	GPUPointLight GetPointLight(int index) const { return pointLights[index]; }
	const std::vector<GPUPointLight>& GetPointLights() const { return pointLights; }

	int GetTotalLights() const { return (int)pointLights.size(); }
	int GetVisibleLights() const { return visibleLightCount; }
	int GetCulledLights() const { return GetTotalLights() - visibleLightCount; }

	const GPUDirectionalLight& GetDirectionalLight() const { return directionalLight; }

	void SetupShadowMaps();
	void UpdateShadowSettings(const ShadowSettings& settings);
	const ShadowSettings& GetShadowSettings() const { return shadowSettings; }

	// For point lights
	glm::mat4 CalculatePointLightMatrix(const glm::vec3& lightPos, int face);
	std::vector<glm::mat4> CalculatePointLightMatrices(const glm::vec3& lightPos);

	// For directional lights
	std::vector<glm::mat4> CalculateDirectionalLightMatrices(const glm::vec3& lightDir,
		const glm::vec3& cameraPos,
		const glm::mat4& cameraView);

	uint32_t AcquirePointShadowIndex();
	void ReleasePointShadowIndex(uint32_t index);

	uint32_t AcquireDirShadowIndex();
	void ReleaseDirShadowIndex(uint32_t index);

	void ResetShadowIndices();

private:
	ForwardPlusLighting() = default;
	~ForwardPlusLighting() = default;

	ForwardPlusLighting(const ForwardPlusLighting&) = delete;
	ForwardPlusLighting& operator=(const ForwardPlusLighting&) = delete;

	void RecalculateTiles();
	bool CompileLightCullingShader();
	GPUPointLight ConvertToGPULight(const LightComponent* light);
	void UpdateDirectionalLight(const LightComponent* light);

	int screenWidth = 0;
	int screenHeight = 0;

	int tileSize = 16; 
	int tilesX = 0;
	int tilesY = 0;
	int maxLightsPerTile = 64;
	bool useClusteredLighting = false;

	GLuint lightCullingShader = 0;

	GLuint pointLightBuffer = 0;
	GLuint directionalLightBuffer = 0;
	GLuint lightGridBuffer = 0;
	GLuint lightIndicesBuffer = 0;

	std::vector<GPUPointLight> pointLights;
	GPUDirectionalLight directionalLight;

	int visibleLightCount = 0;

	static constexpr int MAX_POINT_LIGHTS = 1024;



	//xadows

	uint32_t nextPointShadowIndex = 0;
	uint32_t nextDirShadowIndex = 0;
	std::vector<uint32_t> availablePointShadowIndices;
	std::vector<uint32_t> availableDirShadowIndices;

	GLuint shadowMapFBO = 0;
	GLuint pointShadowMapArray = 0;
	GLuint directionalShadowMapArray = 0;
	int shadowMapSize = 2048;
	int maxPointLightShadows = 16;
	int maxDirectionalLightShadows = 4;
	int maxCascades = 4;
	ShadowSettings shadowSettings;
};