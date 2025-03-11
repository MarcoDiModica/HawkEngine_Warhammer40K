#include "ShaderManager.h"
#include <iostream>

ShaderManager& ShaderManager::GetInstance() {
	static ShaderManager instance;
	return instance;
}

bool ShaderManager::Initialize() {
	bool success = true;

	success &= RegisterShader<UnlitShader>();
	success &= RegisterShader<PBRShader>();

	if (!success) {
		std::cerr << "Failed to initialize one or more shaders" << std::endl;
	}

	return success;
}

Shaders* ShaderManager::GetShader(ShaderType type) {
	auto it = shaders.find(type);
	if (it != shaders.end()) {
		return it->second.get();
	}

	std::cerr << "Shader type not found: " << static_cast<int>(type) << std::endl;
	return nullptr;
}

void ShaderManager::Cleanup() {
	shaders.clear();
}