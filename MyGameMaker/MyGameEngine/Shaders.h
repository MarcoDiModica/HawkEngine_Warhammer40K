#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "types.h"

class Shaders {
public:
    Shaders();

    virtual ~Shaders();

    bool LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

    bool LoadShadersFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    bool LoadComputeShader(const std::string& computeShaderFile);

    bool LoadComputeShaderFromSource(const std::string& computeSource);

    void Bind() const;

    void UnBind() const;

    GLuint GetProgram() const;

    virtual ShaderType GetShaderType() const = 0;

    virtual bool Initialize() = 0;

    virtual bool Reload();

    bool IsComputeShader() const { return isComputeShader; }

    void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1);

    void DispatchBarrier(GLbitfield barriers = GL_SHADER_STORAGE_BARRIER_BIT);

    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, uint32_t value);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, bool value);
    void SetUniformVec2(const std::string& name, const glm::vec2& value);
    void SetUniformVec3(const std::string& name, const glm::vec3& value);
    void SetUniformVec4(const std::string& name, const glm::vec4& value);
    void SetUniformMat2(const std::string& name, const glm::mat2& value);
    void SetUniformMat3(const std::string& name, const glm::mat3& value);
    void SetUniformMat4(const std::string& name, const glm::mat4& value);

    void SetUniformArray(const std::string& name, const int* values, int count);
    void SetUniformArray(const std::string& name, const float* values, int count);
    void SetUniformArray(const std::string& name, const glm::vec2* values, int count);
    void SetUniformArray(const std::string& name, const glm::vec3* values, int count);
    void SetUniformArray(const std::string& name, const glm::vec4* values, int count);
    void SetUniformArray(const std::string& name, const glm::mat4* values, int count);

    void SetUniform(const std::string& name, GLuint64 bindlessHandle);

    void SetUniformBlockBinding(const std::string& name, GLuint bindingPoint);
    void SetStorageBlockBinding(const std::string& name, GLuint bindingPoint);

    GLint GetUniformLocation(const std::string& name);

    GLuint GetUniformBlockIndex(const std::string& name);
    GLuint GetStorageBlockIndex(const std::string& name);

protected:
    GLuint CompileShader(const std::string& shaderSource, GLenum shaderType);
    std::string LoadShaderSource(const std::string& shaderFile);
    virtual std::string PreprocessShader(const std::string& source);

    GLuint _program;

    bool isComputeShader;

    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    std::string computeShaderPath;

    std::unordered_map<std::string, GLint> uniformLocationsCache;
    std::unordered_map<std::string, GLuint> uniformBlockCache;
    std::unordered_map<std::string, GLuint> storageBlockCache;
};

class DynamicShader : public Shaders {
public:
	DynamicShader(ShaderType type) : shaderType(type) {}

	ShaderType GetShaderType() const override {
		return shaderType;
	}

	bool Initialize() override {
		return true;
	}

private:
	ShaderType shaderType;
};

class UnlitShader : public Shaders {
public:
    UnlitShader();
    ShaderType GetShaderType() const override { return ShaderType::UNLIT; }
    bool Initialize() override;
};

class PBRShader : public Shaders {
public:
    PBRShader();
    ShaderType GetShaderType() const override { return ShaderType::PBR; }
    bool Initialize() override;
};

class DepthShader : public Shaders {
public:
    DepthShader();
    ShaderType GetShaderType() const override { return ShaderType::DEPTH; }
    bool Initialize() override;
};

class ForwardPlusComputeShader : public Shaders {
public:
    ForwardPlusComputeShader();
    ShaderType GetShaderType() const override { return ShaderType::FORWARD_PLUS_COMPUTE; }
    bool Initialize() override;
};

class CullingComputeShader : public Shaders {
public:
    CullingComputeShader();
    ShaderType GetShaderType() const override { return ShaderType::CULLING_COMPUTE; }
    bool Initialize() override;
};

class ComputeShader : public Shaders {
public:
    ComputeShader(const std::string& filename, ShaderType type = ShaderType::CUSTOM);
    ShaderType GetShaderType() const override { return shaderType; }
    bool Initialize() override;

private:
    std::string filename;
    ShaderType shaderType;
};

class CustomShader : public Shaders {
public:
    CustomShader(const std::string& vertexFile, const std::string& fragmentFile,
        ShaderType type = ShaderType::CUSTOM);
    ShaderType GetShaderType() const override { return shaderType; }
    bool Initialize() override;

private:
    std::string vertexFile;
    std::string fragmentFile;
    ShaderType shaderType;
};