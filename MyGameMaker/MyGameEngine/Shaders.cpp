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
	try {
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

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception while loading shaders: " << e.what() << std::endl;
		return false;
	}
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
    glUniform4fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(value));
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

		// Incluir información sobre el tipo de shader
		std::string shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "VERTEX" :
			((shaderType == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN");
		std::cerr << "SHADER TYPE: " << shaderTypeStr << std::endl;

		// Mostrar las primeras líneas del código del shader para depuración
		std::istringstream sourceStream(shaderSource);
		std::string line;
		int lineNum = 1;
		std::cerr << "SHADER SOURCE:\n";
		while (std::getline(sourceStream, line) && lineNum <= 10) {
			std::cerr << lineNum++ << ": " << line << std::endl;
		}

		// Limpiar recursos y lanzar un error para detener el proceso
		glDeleteShader(shader);
		throw std::runtime_error("Shader compilation failed");
	}

	return shader;
}

std::string Shaders::LoadShaderSource(const std::string& shaderFile) {
	std::ifstream file(shaderFile);
	if (!file.is_open()) {
		std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << shaderFile << std::endl;
		throw std::runtime_error("Failed to open shader file: " + shaderFile);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}