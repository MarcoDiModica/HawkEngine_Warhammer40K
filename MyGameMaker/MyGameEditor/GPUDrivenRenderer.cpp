#include "GPUDrivenRenderer.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include "MyGameEngine/ShaderManager.h"

GPUDrivenRenderer& GPUDrivenRenderer::GetInstance() {
	static GPUDrivenRenderer instance;
	return instance;
}

bool GPUDrivenRenderer::Initialize() {
	if (!GLEW_ARB_multi_draw_indirect) {
		LOG(LogType::LOG_INFO, "Multi-draw indirect no soportado por esta GPU! Usando fallback.");
	}

	if (!GLEW_ARB_compute_shader) {
		LOG(LogType::LOG_INFO, "Compute Shaders no soportados por esta GPU!");
		useGPUCulling = false;
	}

	glCreateVertexArrays(1, &defaultVAO);

	glCreateBuffers(1, &drawCommandBuffer);
	glNamedBufferStorage(drawCommandBuffer,
		MAX_DRAW_COMMANDS * sizeof(DrawElementsCommand),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &cullDataBuffer);
	glNamedBufferStorage(cullDataBuffer,
		MAX_DRAW_COMMANDS * sizeof(CullData),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &visibleCountBuffer);
	glNamedBufferStorage(visibleCountBuffer,
		sizeof(GLuint),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	if (useGPUCulling) {
		if (!CompileCullingShader()) {
			LOG(LogType::LOG_INFO, "No se pudo compilar shader de culling, usando CPU fallback");
			useGPUCulling = false;
		}
	}

	return true;
}

void GPUDrivenRenderer::Shutdown() {
	if (cullingShader) glDeleteProgram(cullingShader);
	if (drawCommandBuffer) glDeleteBuffers(1, &drawCommandBuffer);
	if (cullDataBuffer) glDeleteBuffers(1, &cullDataBuffer);
	if (visibleCountBuffer) glDeleteBuffers(1, &visibleCountBuffer);
	if (defaultVAO) glDeleteVertexArrays(1, &defaultVAO);

	cullingShader = 0;
	drawCommandBuffer = 0;
	cullDataBuffer = 0;
	visibleCountBuffer = 0;
	defaultVAO = 0;

	cullData.clear();
	drawCommands.clear();
}

void GPUDrivenRenderer::BeginFrame() {
	cullData.clear();
	drawCommands.clear();
	currentInstanceOffset = 0;
	visibleInstanceCount = 0;

	GLuint zero = 0;
	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);

	BindlessManager::GetInstance().ClearInstances();
}

void GPUDrivenRenderer::EndFrame() {
}

void GPUDrivenRenderer::AddInstanceGroup(
	uint32_t meshIndex,
	const glm::vec4& boundingSphere,
	const std::vector<GPUInstance>& instances) {

	if (instances.empty() || meshIndex >= BindlessManager::GetInstance().GetMeshCount()) {
		return;
	}

	if (cullData.size() >= MAX_DRAW_COMMANDS) {
		LOG(LogType::LOG_WARNING, "Warning: Alcanzado límite máximo de comandos de dibujo");
		return;
	}

	CullData cullItem;
	cullItem.boundingSphere = boundingSphere;
	cullItem.drawID = static_cast<uint32_t>(cullData.size());
	cullItem.meshIndex = meshIndex;
	cullItem.instanceOffset = currentInstanceOffset;
	cullItem.instanceCount = static_cast<uint32_t>(instances.size());

	for (const auto& instance : instances) {
		BindlessManager::GetInstance().AddInstance(instance);
	}

	currentInstanceOffset += static_cast<uint32_t>(instances.size());
	cullData.push_back(cullItem);
}

void GPUDrivenRenderer::PrepareDrawCommands(/*const Frustum& frustum*/) {
	if (cullData.empty()) {
		return;
	}

	glNamedBufferSubData(cullDataBuffer, 0,
		cullData.size() * sizeof(CullData),
		cullData.data());

	drawCommands.clear();
	visibleInstanceCount = 0;

	for (const auto& cullItem : cullData) {
		GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
		if (!meshData) continue;

		DrawElementsCommand command;
		command.count = meshData->indexCount;
		command.instanceCount = cullItem.instanceCount;
		command.firstIndex = 0;
		command.baseVertex = 0;
		command.baseInstance = cullItem.instanceOffset;

		drawCommands.push_back(command);
		visibleInstanceCount += cullItem.instanceCount;
	}

	if (!drawCommands.empty()) {
		glNamedBufferSubData(drawCommandBuffer, 0,
			drawCommands.size() * sizeof(DrawElementsCommand),
			drawCommands.data());
	}

	LOG(LogType::LOG_INFO, "Total de instancias visibles: %d", visibleInstanceCount);
	LOG(LogType::LOG_INFO, "Total de draw commands: %d", (int)drawCommands.size());
}

//void GPUDrivenRenderer::CPUFrustumCulling(const Frustum& frustum) {
//	drawCommands.clear();
//	visibleInstanceCount = 0;
//
//	GLuint zero = 0;
//	glNamedBufferSubData(visibleCountBuffer, 0, sizeof(GLuint), &zero);
//
//	for (const auto& cullItem : cullData) {
//		bool isVisible = true;
//
//		if (useFrustumCulling) {
//			glm::vec3 center(cullItem.boundingSphere);
//			float radius = cullItem.boundingSphere.w;
//
//			isVisible = frustum.SphereInFrustum(center, radius);
//		}
//
//		if (isVisible) {
//			GPUMesh* meshData = BindlessManager::GetInstance().GetMeshData(cullItem.meshIndex);
//			if (!meshData) continue;
//
//			DrawElementsCommand command;
//			command.count = meshData->indexCount;
//			command.instanceCount = cullItem.instanceCount;
//			command.firstIndex = 0;
//			command.baseVertex = 0;
//			command.baseInstance = cullItem.instanceOffset;
//
//			drawCommands.push_back(command);
//
//		}
//	}
//
//	if (!drawCommands.empty()) {
//		glNamedBufferSubData(drawCommandBuffer, 0,
//			drawCommands.size() * sizeof(DrawElementsCommand),
//			drawCommands.data());
//	}
//}

void GPUDrivenRenderer::RenderAll(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	//TEST DE BINDLESS Y SSBO
	// GLuint testVS = glCreateShader(GL_VERTEX_SHADER);
	//const char* vsSource = R"(
	//	#version 460 core
	//	void main() {
	//		const vec2 positions[3] = vec2[3](
	//			vec2(-0.5, -0.5),
	//			vec2(0.5, -0.5),
	//			vec2(0.0, 0.5)
	//		);
	//		gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
	//	}
	//)";
	//glShaderSource(testVS, 1, &vsSource, NULL);
	//glCompileShader(testVS);

	//// Verificar compilación del Vertex Shader
	//GLint vsSuccess;
	//glGetShaderiv(testVS, GL_COMPILE_STATUS, &vsSuccess);
	//if (!vsSuccess) {
	//	char infoLog[512];
	//	glGetShaderInfoLog(testVS, 512, NULL, infoLog);
	//	LOG(LogType::LOG_ERROR, "Error: Vertex shader compilation failed:\n%s", infoLog);
	//	return;
	//}

	//GLuint testFS = glCreateShader(GL_FRAGMENT_SHADER);
	//const char* fsSource = R"(
	//	#version 460 core
	//	#extension GL_ARB_bindless_texture : require
	//	#extension GL_ARB_gpu_shader_int64 : require

	//	out vec4 FragColor;

	//	struct TestMaterial {
	//		vec4 color;
	//		uint64_t textureHandle;
	//	};

	//	layout(std430, binding = 0) readonly buffer MaterialBuffer {
	//		TestMaterial materials[];
	//	};

	//	void main() {
	//		// Si el SSBO y bindless funcionan juntos, verás azul
	//		vec4 baseColor = materials[0].color;

	//		// Intenta usar la textura solo si el color base funciona
	//		if (baseColor.r < 0.1 && baseColor.g < 0.1 && baseColor.b > 0.9) {
	//			sampler2D tex = sampler2D(materials[0].textureHandle);
	//			vec2 fixedTexCoord = vec2(0.25, 0.25);
	//			FragColor = texture(tex, fixedTexCoord);
	//		} else {
	//			FragColor = baseColor;
	//		}
	//	}
	//)";
	//glShaderSource(testFS, 1, &fsSource, NULL);
	//glCompileShader(testFS);

	//// Verificar compilación del Fragment Shader
	//GLint fsSuccess;
	//glGetShaderiv(testFS, GL_COMPILE_STATUS, &fsSuccess);
	//if (!fsSuccess) {
	//	char infoLog[512];
	//	glGetShaderInfoLog(testFS, 512, NULL, infoLog);
	//	LOG(LogType::LOG_ERROR, "Error: Fragment shader compilation failed:\n%s", infoLog);
	//	return;
	//}

	//GLuint testProgram = glCreateProgram();
	//glAttachShader(testProgram, testVS);
	//glAttachShader(testProgram, testFS);
	//glLinkProgram(testProgram);

	//// Verificar enlazado del Programa
	//GLint programSuccess;
	//glGetProgramiv(testProgram, GL_LINK_STATUS, &programSuccess);
	//if (!programSuccess) {
	//	char infoLog[512];
	//	glGetProgramInfoLog(testProgram, 512, NULL, infoLog);
	//	LOG(LogType::LOG_ERROR, "Error: Shader program linkage failed:\n%s", infoLog);
	//}

	//// Limpiar estado de OpenGL
	//glUseProgram(testProgram);
	//glBindVertexArray(defaultVAO);

	//// Crear una textura simple
	//GLuint testTexture;
	//glCreateTextures(GL_TEXTURE_2D, 1, &testTexture);
	//unsigned char texData[] = {
	//	255, 0, 0, 255,   0, 255, 0, 255,
	//	0, 0, 255, 255,   255, 255, 0, 255
	//};
	//glTextureStorage2D(testTexture, 1, GL_RGBA8, 2, 2);
	//glTextureSubImage2D(testTexture, 0, 0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	//glTextureParameteri(testTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTextureParameteri(testTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//// Obtener handle bindless
	//GLuint64 handle = glGetTextureHandleARB(testTexture);
	//glMakeTextureHandleResidentARB(handle);

	//// Crear y llenar un SSBO con textura bindless
	//struct TestMaterial {
	//	glm::vec4 color;
	//	uint64_t textureHandle;
	//};

	//TestMaterial testMat;
	//testMat.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Azul
	//testMat.textureHandle = handle;

	//GLuint testMatBuffer;
	//glCreateBuffers(1, &testMatBuffer);
	//glNamedBufferStorage(testMatBuffer, sizeof(TestMaterial), &testMat, GL_DYNAMIC_STORAGE_BIT);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, testMatBuffer);

	//// Dibujar
	//glUseProgram(testProgram);
	//glBindVertexArray(defaultVAO);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//// Limpiar (mover la liberación de recursos al final del ciclo de vida)
	// glUseProgram(0);
	// glBindVertexArray(0);
	// glDeleteProgram(testProgram);
	// glDeleteShader(testVS);
	// glDeleteShader(testFS);
	// glDeleteBuffers(1, &testMatBuffer);
	// glMakeTextureHandleNonResidentARB(handle);
	// glDeleteTextures(1, &testTexture);

	//// Retornar tempranamente para evitar ejecutar el resto del código
	//return;
}

bool GPUDrivenRenderer::CompileCullingShader() {
	//pillar el shader de ShaderManager GetShaderPorgram
	cullingShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::CULLING_COMPUTE);
	if (cullingShader == 0) {
		LOG(LogType::LOG_ERROR, "Error: No se pudo obtener el programa de shader de culling");
		return false;
	}

	return false;
}