#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/LightComponent.h"

struct GPUPointLight {
	glm::vec4 position;   // xyz = posici�n, w = radio
	glm::vec4 color;      // rgb = color, a = intensidad
	glm::vec4 attenuation; // x = constante, y = lineal, z = cuadr�tica, w = no usado
	uint32_t lightType;   // 0 = point, 1 = spot (futuro)
	uint32_t castShadow;  // 0 = no, 1 = yes
	uint32_t shadowMapIndex; // �ndice a shadowmap (si usa sombras)
	uint32_t padding;     // alineaci�n
};

struct GPUDirectionalLight {
	glm::vec4 direction;  // xyz = direcci�n, w = intensidad
	glm::vec4 color;      // rgb = color, a = no usado
	glm::mat4 shadowMatrix; // matriz para shadowmap (si usa sombras)
	uint32_t castShadow;  // 0 = no, 1 = yes
	uint32_t shadowMapIndex; // �ndice a shadowmap (si usa sombras)
	uint32_t useCascades; // 0 = no, 1 = yes (futuro)
	uint32_t numCascades; // n�mero de cascadas (futuro)
	float darknessFallback;
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

	int GetTotalLights() const { return (int)pointLights.size(); }
	int GetVisibleLights() const { return visibleLightCount; }
	int GetCulledLights() const { return GetTotalLights() - visibleLightCount; }

	const GPUDirectionalLight& GetDirectionalLight() const { return directionalLight; }

	glm::vec3 GetDirLightPosition() const { return dirLightPosition; }
	glm::vec4 GetDirLightBounds() const { return dirOrthographicBounds; }
	float GetDirLightDistance() const { return dirLightDistance; }
	float GetDirLightNearPlane() const { return dirNearPlane; }
	float GetDirLightFarPlane() const { return dirFarPlane; }

	void SetDirLightPosition(const glm::vec3& position) { dirLightPosition = position; }

	void SetDirLightDistance(float distance) { dirLightDistance = distance; }
	void SetDirLightNearPlane(float nearPlane) { dirNearPlane = nearPlane; }
	void SetDirLightFarPlane(float farPlane) { dirFarPlane = farPlane; }
	void SetDirLightOrthographicBounds(const glm::vec4& bounds) { dirOrthographicBounds = bounds; }



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

	glm::vec3 dirLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	float dirLightDistance = 100.0f;
	float dirNearPlane = 1.0f;
	float dirFarPlane = 100.0f;
	glm::vec4 dirOrthographicBounds = glm::vec4(-10.0f, 10.0f, -10.0f, 10.0f); // left, right, bottom, top

	std::vector<GPUPointLight> pointLights;
	GPUDirectionalLight directionalLight;

	int visibleLightCount = 0;

	static constexpr int MAX_POINT_LIGHTS = 1024; 
};