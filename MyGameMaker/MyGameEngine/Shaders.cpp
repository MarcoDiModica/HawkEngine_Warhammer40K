#include "Shaders.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shaders::Shaders() : _program(0) {}

Shaders::~Shaders() {
    if (_program != 0) {
        glDeleteProgram(_program);
    }
}

bool Shaders::LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
    std::string vertexShaderSource = LoadShaderSource(vertexShaderFile);
    std::string fragmentShaderSource = LoadShaderSource(fragmentShaderFile);

    GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);

    GLint success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(_program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Shaders::Bind() const {
    glUseProgram(_program);
}

void Shaders::UnBind() const {
    glUseProgram(0);
}

GLuint Shaders::GetProgram() const {
    return _program;
}

void Shaders::SetUniform(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(_program, name.c_str()), value);
}

void Shaders::SetUniform(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(_program, name.c_str()), value);
}

void Shaders::SetUniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(value));
}
void Shaders::SetUniform(const std::string& name, const glm::vec4& value) {
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shaders::SetUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shaders::CompileShader(const std::string& shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = shaderSource.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

std::string Shaders::LoadShaderSource(const std::string& shaderFile) {
    std::ifstream file(shaderFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}