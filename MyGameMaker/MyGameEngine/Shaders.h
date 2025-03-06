#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "types.h"

class Shaders
{
public:
    Shaders();
    ~Shaders();

    bool LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    void Bind() const;
	void UnBind() const;
    GLuint GetProgram() const;

    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::mat4& value);

private:
    GLuint CompileShader(const std::string& shaderSource, GLenum shaderType);
    std::string LoadShaderSource(const std::string& shaderFile);

    GLuint _program;
};