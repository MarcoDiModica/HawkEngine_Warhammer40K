#pragma once
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <map>
#include "BindlessManager.h"
#include "../MyGameEngine/Shaders.h"
#include "MyGameEngine/CameraBase.h"
#include "ShadowMap.h"

struct DrawElementsCommand {
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLint baseVertex;
	GLuint baseInstance;
};

struct CullData {
	uint32_t drawID;
	uint32_t meshIndex;
	uint32_t instanceOffset;
	uint32_t instanceCount;
	uint32_t materialIndex;
};

enum class RenderPassType {
	NORMAL,   
	UI,       
	PARTICLES 
};

struct ShaderBatch {
	ShaderType shaderType;                    
	RenderPassType passType;                  
	std::vector<DrawElementsCommand> commands;
	std::vector<uint32_t> meshIndices;        
	std::vector<uint32_t> materialIndices;    
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
		uint32_t materialIndex,
		const std::vector<GPUInstance>& instances
	);

	void PrepareDrawCommands();	
	void ForceIncludeAllObjects();
	

	void RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec3& cameraPos);

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

	void DebugMeshInfo(uint32_t meshIndex);
	void BatchCommandsByRenderPass();

	void RenderUnlitBatch(const ShaderBatch& batch,
		const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		bool isUI);

	void RenderPBRBatch(
		const ShaderBatch& batch,
		const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		const glm::vec3& cameraPos);

	void RenderDepthPass(const ShaderBatch& batch);

	void HandleTextureBindings(Shaders* shader, const char* textureName, const char* hasTextureName, GLuint64 textureHandle);
	void BindRegularTextures(Shaders* shader, GPUMaterial* materialData);

	GLuint cullingShader = 0;
	GLuint drawCommandBuffer = 0;
	GLuint cullDataBuffer = 0;
	GLuint visibleCountBuffer = 0;
	GLuint defaultVAO = 0;

	std::vector<CullData> cullData;
	std::vector<DrawElementsCommand> drawCommands;

	std::map<RenderPassType, std::map<ShaderType, ShaderBatch>> batchesByPass;

	uint32_t currentInstanceOffset = 0;
	int visibleInstanceCount = 0;

	bool useGPUCulling = false;
	bool useOcclusionCulling = false;
	bool useFrustumCulling = true;
	bool enableCulling = true;

	static constexpr int MAX_DRAW_COMMANDS = 10000;
	bool bindlessErrorDetected;

	ShadowMap shadowMap;
	Shaders* depthShader = nullptr;

	

};