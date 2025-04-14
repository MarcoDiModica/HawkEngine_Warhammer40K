#pragma once

#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "BindlessManager.h"
//#include "../MyGameEngine/Frustum.h" //que?

struct DrawElementsCommand {
	GLuint count;         // Número de índices
	GLuint instanceCount; // Número de instancias
	GLuint firstIndex;    // Primer índice
	GLint baseVertex;     // Base para indexación de vértices
	GLuint baseInstance;  // Base para indexación de instancias
};

struct CullData {
	glm::vec4 boundingSphere;  // xyz = centro, w = radio
	uint32_t drawID;           // ID del comando de dibujado
	uint32_t meshIndex;        // Índice de la malla
	uint32_t instanceOffset;   // Offset en el buffer de instancias
	uint32_t instanceCount;    // Número de instancias
};

class GPUDrivenRenderer {
public:
	static GPUDrivenRenderer& GetInstance();

	bool Initialize();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void AddInstanceGroup(
		uint32_t meshIndex,
		const glm::vec4& boundingSphere,
		const std::vector<GPUInstance>& instances
	);

	void PrepareDrawCommands(/*const Frustum& frustum*/);

	void RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

	void SetUseGPUCulling(bool enabled) { useGPUCulling = enabled; }
	void SetUseOcclusionCulling(bool enabled) { useOcclusionCulling = enabled; }
	void SetUseFrustumCulling(bool enabled) { useFrustumCulling = enabled; }

	int GetVisibleInstanceCount() const { return visibleInstanceCount; }
	int GetTotalDrawCommands() const { return (int)cullData.size(); }

private:
	GPUDrivenRenderer() = default;
	~GPUDrivenRenderer() = default;

	GPUDrivenRenderer(const GPUDrivenRenderer&) = delete;
	GPUDrivenRenderer& operator=(const GPUDrivenRenderer&) = delete;

	//void CPUFrustumCulling(const Frustum& frustum);
	bool CompileCullingShader();

	GLuint cullingShader = 0;

	GLuint drawCommandBuffer = 0;
	GLuint cullDataBuffer = 0;
	GLuint visibleCountBuffer = 0;

	GLuint defaultVAO = 0;

	std::vector<CullData> cullData;
	std::vector<DrawElementsCommand> drawCommands;

	uint32_t currentInstanceOffset = 0;

	int visibleInstanceCount = 0;

	bool useGPUCulling = true;
	bool useOcclusionCulling = false;
	bool useFrustumCulling = true;

	static constexpr int MAX_DRAW_COMMANDS = 10000;
};