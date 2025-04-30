#include "ShaderManager.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include "../MyParticlesEngine/ParticleShader.h"

ShaderManager& ShaderManager::GetInstance() {
	static ShaderManager instance;
	return instance;
}

bool ShaderManager::Initialize() {
	LOG(LogType::LOG_INFO, "Initializing ShaderManager...");
	bool success = true;

	LOG(LogType::LOG_INFO, "Registering UnlitShader...");
	success &= RegisterShader<UnlitShader>();

	LOG(LogType::LOG_INFO, "Registering PBRShader...");
	success &= RegisterShader<PBRShader>();

	LOG(LogType::LOG_INFO, "Registering ParticleShader...");
	success &= RegisterShader<ParticleShader>();

	LOG(LogType::LOG_INFO, "Registering ForwardPlusComputeShader...");
	success &= RegisterShader<ForwardPlusComputeShader>();

	LOG(LogType::LOG_INFO, "Registering CullingComputeShader...");
	success &= RegisterShader<CullingComputeShader>();

	LOG(LogType::LOG_INFO, "Registering Debug Shader...");
	success &= RegisterCustomShader("debug", "Assets/Shaders/debug_vertex.glsl",
		"Assets/Shaders/debug_fragment.glsl", ShaderType::DEBUG);

	if (!success) {
		LOG(LogType::LOG_ERROR, "ShaderManager: Failed to initialize one or more shaders");
	}
	else {
		LOG(LogType::LOG_INFO, "ShaderManager initialized successfully");
	}

	return success;
}

void ShaderManager::Cleanup() {
	typeToShaderMap.clear();
	nameToShaderMap.clear();
	shaders.clear();
}

Shaders* ShaderManager::GetShader(ShaderType type) {
	auto it = typeToShaderMap.find(type);
	if (it != typeToShaderMap.end()) {
		return it->second;
	}

	LOG(LogType::LOG_ERROR, "ShaderManager: Tipo de shader no encontrado: %d", (int)type);
	return nullptr;
}

Shaders* ShaderManager::GetShader(const std::string& name) {
	auto it = nameToShaderMap.find(name);
	if (it != nameToShaderMap.end()) {
		return it->second;
	}

	LOG(LogType::LOG_ERROR, "ShaderManager: Shader no encontrado por nombre: %s", name.c_str());
	return nullptr;
}

GLuint ShaderManager::GetShaderProgram(ShaderType type) {
	Shaders* shader = GetShader(type);
	if (shader) {
		return shader->GetProgram();
	}
	return 0;
}

GLuint ShaderManager::GetShaderProgram(const std::string& name) {
	Shaders* shader = GetShader(name);
	if (shader) {
		return shader->GetProgram();
	}
	return 0;
}

bool ShaderManager::RegisterCustomShader(
	const std::string& name,
	const std::string& vertexShaderFile,
	const std::string& fragmentShaderFile,
	ShaderType type
) {
	auto shader = std::make_unique<CustomShader>(vertexShaderFile, fragmentShaderFile, type);

	if (!shader->Initialize()) {
		LOG(LogType::LOG_ERROR, "ShaderManager: Error al inicializar shader personalizado: %s", name.c_str());
		return false;
	}

	typeToShaderMap[type] = shader.get();
	nameToShaderMap[name] = shader.get();

	shaders.push_back(std::move(shader));

	return true;
}

bool ShaderManager::RegisterComputeShader(
	const std::string& name,
	const std::string& computeShaderFile,
	ShaderType type
) {
	auto shader = std::make_unique<ComputeShader>(computeShaderFile, type);

	if (!shader->Initialize()) {
		LOG(LogType::LOG_ERROR, "ShaderManager: Error al inicializar compute shader: %s", name.c_str());
		return false;
	}

	typeToShaderMap[type] = shader.get();
	nameToShaderMap[name] = shader.get();

	shaders.push_back(std::move(shader));

	return true;
}

bool ShaderManager::RegisterShaderFromSource(
	const std::string& name,
	const std::string& vertexSource,
	const std::string& fragmentSource,
	ShaderType type
) {
	auto shader = std::make_unique<DynamicShader>(type);

	if (!shader->LoadShadersFromSource(vertexSource, fragmentSource)) {
		std::cerr << "ShaderManager: Failed to compile shader from source: " << name << std::endl;
		return false;
	}

	typeToShaderMap[type] = shader.get();
	nameToShaderMap[name] = shader.get();

	shaders.push_back(std::move(shader));

	return true;
}

bool ShaderManager::RegisterComputeShaderFromSource(
	const std::string& name,
	const std::string& computeSource,
	ShaderType type
) {
	auto shader = std::make_unique<DynamicShader>(type);

	if (!shader->LoadComputeShaderFromSource(computeSource)) {
		std::cerr << "ShaderManager: Failed to compile compute shader from source: " << name << std::endl;
		return false;
	}

	typeToShaderMap[type] = shader.get();
	nameToShaderMap[name] = shader.get();

	shaders.push_back(std::move(shader));

	return true;
}

bool ShaderManager::ReloadShader(const std::string& name) {
	auto it = nameToShaderMap.find(name);
	if (it == nameToShaderMap.end()) {
		LOG(LogType::LOG_ERROR, "ShaderManager: No se puede recargar, shader no encontrado: %s", name.c_str());
		return false;
	}

	Shaders* shader = it->second;
	return shader->Reload();
}

bool ShaderManager::ReloadShader(ShaderType type) {
	auto it = typeToShaderMap.find(type);
	if (it == typeToShaderMap.end()) {
		LOG(LogType::LOG_ERROR, "ShaderManager: No se puede recargar, tipo de shader no encontrado: %d", (int)type);
		return false;
	}

	Shaders* shader = it->second;
	return shader->Reload();
}

bool ShaderManager::ReloadAllShaders() {
	bool allSuccess = true;

	for (const auto& shader : shaders) {
		if (!shader->Reload()) {
			allSuccess = false;
			// Continuar con los demás shaders aunque uno falle
		}
	}

	return allSuccess;
}

int ShaderManager::CheckForModifiedShaders(bool forceCheck) {
	if (forceCheck) {
		if (ReloadAllShaders()) {
			return static_cast<int>(shaders.size());
		}
	}

	return 0;
}

void ShaderManager::Update() {
	if (!hotReloadEnabled) return;

	auto currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
	double now = std::chrono::duration<double>(currentTime).count();

	if (now - lastCheckTime > checkInterval) {
		CheckForModifiedShaders();
		lastCheckTime = now;
	}
}

std::string ShaderManager::GetNameForShaderType(ShaderType type) const {
	switch (type) {
	case ShaderType::UNLIT:
		return "unlit";
	case ShaderType::PBR:
		return "pbr";
	case ShaderType::PARTICLE:
		return "particle";
	case ShaderType::BINDLESS_PBR:
		return "bindless_pbr";
	case ShaderType::BINDLESS_UNLIT:
		return "bindless_unlit";
	case ShaderType::FORWARD_PLUS_COMPUTE:
		return "forward_plus_compute";
	case ShaderType::CULLING_COMPUTE:
		return "culling_compute";
	case ShaderType::DEBUG:
		return "debug";
	case ShaderType::UTILITY:
		return "utility";
	default:
		return "custom_" + std::to_string(static_cast<int>(type));
	}
}