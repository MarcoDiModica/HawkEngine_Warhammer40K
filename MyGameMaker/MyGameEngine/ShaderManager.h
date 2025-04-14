#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "Shaders.h"
#include "types.h"
#include "MyGameEditor/Log.h"

class ShaderManager {
public:
    static ShaderManager& GetInstance();

    bool Initialize();

    void Cleanup();

    Shaders* GetShader(ShaderType type);

    Shaders* GetShader(const std::string& name);

    GLuint GetShaderProgram(ShaderType type);

    GLuint GetShaderProgram(const std::string& name);

    template<typename T>
    bool RegisterShader() {
        static_assert(std::is_base_of<Shaders, T>::value, "T must derive from Shaders");

        auto shader = std::make_unique<T>();
        std::string name = GetNameForShaderType(shader->GetShaderType());
        ShaderType type = shader->GetShaderType();

        if (!shader->Initialize()) {
            LOG(LogType::LOG_ERROR, "ShaderManager Failed to initialize shader: %s", name);
            return false;
        }

        typeToShaderMap[type] = shader.get();
        nameToShaderMap[name] = shader.get();

        shaders.push_back(std::move(shader));

        return true;
    }

    bool RegisterCustomShader(const std::string& name,
        const std::string& vertexShaderFile,
        const std::string& fragmentShaderFile,
        ShaderType type = ShaderType::CUSTOM);

    bool RegisterComputeShader(const std::string& name,
        const std::string& computeShaderFile,
        ShaderType type = ShaderType::CUSTOM);

    bool RegisterShaderFromSource(const std::string& name,
        const std::string& vertexSource,
        const std::string& fragmentSource,
        ShaderType type = ShaderType::CUSTOM);

    bool RegisterComputeShaderFromSource(const std::string& name,
        const std::string& computeSource,
        ShaderType type = ShaderType::CUSTOM);

    bool ReloadShader(const std::string& name);

    
    bool ReloadShader(ShaderType type);

    bool ReloadAllShaders();

    int CheckForModifiedShaders(bool forceCheck = false);

    void Update();

    void SetHotReloadEnabled(bool enable) { hotReloadEnabled = enable; }

    bool IsHotReloadEnabled() const { return hotReloadEnabled; }

private:
    ShaderManager() = default;
    ~ShaderManager() = default;
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::string GetNameForShaderType(ShaderType type) const;

    std::vector<std::unique_ptr<Shaders>> shaders;

    std::unordered_map<ShaderType, Shaders*> typeToShaderMap;
    std::unordered_map<std::string, Shaders*> nameToShaderMap;

    bool hotReloadEnabled = false;
    double lastCheckTime = 0.0;
    double checkInterval = 2.0;

    static constexpr int MAX_SHADER_HISTORY = 10;
};