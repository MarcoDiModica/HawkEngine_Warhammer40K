#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Shaders.h"
#include "types.h"

class ShaderManager {
public:
	static ShaderManager& GetInstance();

	bool Initialize();

	Shaders* GetShader(ShaderType type);

	template<typename T>
	bool RegisterShader() {
		static_assert(std::is_base_of<Shaders, T>::value, "T must derive from Shaders");

		auto shader = std::make_unique<T>();
		ShaderType type = shader->GetShaderType();

		if (!shader->Initialize()) {
			return false;
		}

		shaders[type] = std::move(shader);
		return true;
	}

	void Cleanup();

private:
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	std::unordered_map<ShaderType, std::unique_ptr<Shaders>> shaders{};
};