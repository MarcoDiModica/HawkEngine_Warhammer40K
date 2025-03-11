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

	if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
		std::cerr << "Failed to load shader source files" << std::endl;
		return false;
	}

	// Preprocess shader source for extensions/includes
	vertexShaderSource = PreprocessShader(vertexShaderSource);
	fragmentShaderSource = PreprocessShader(fragmentShaderSource);

	GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	if (vertexShader == 0 || fragmentShader == 0) {
		return false;
	}

	// Create and link program
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

GLint Shaders::GetUniformLocation(const std::string& name) {
	// Cache uniform locations for better performance
	auto it = uniformLocationsCache.find(name);
	if (it != uniformLocationsCache.end()) {
		return it->second;
	}

	GLint location = glGetUniformLocation(_program, name.c_str());
	uniformLocationsCache[name] = location;
	return location;
}

void Shaders::SetUniform(const std::string& name, int value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shaders::SetUniform(const std::string& name, float value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1f(location, value);
	}
}

void Shaders::SetUniform(const std::string& name, const glm::vec3& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
}

void Shaders::SetUniform(const std::string& name, const glm::vec4& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}

void Shaders::SetUniform(const std::string& name, const glm::mat4& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
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
		return 0;
	}

	return shader;
}

std::string Shaders::LoadShaderSource(const std::string& shaderFile) {
	std::ifstream file(shaderFile);
	if (!file.is_open()) {
		std::cerr << "Failed to open shader file: " << shaderFile << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string Shaders::PreprocessShader(const std::string& source) {
	// This can be extended to handle #include, #define for shadows, etc.
	return source;
}

//UnlitShader
UnlitShader::UnlitShader() {
}

bool UnlitShader::Initialize() {
	return LoadShaders("Assets/Shaders/unlit_vertex.glsl", "Assets/Shaders/unlit_fragment.glsl");
}

// PBRShader
PBRShader::PBRShader() {
}

bool PBRShader::Initialize() {
	return LoadShaders("Assets/Shaders/pbr_vertex.glsl", "Assets/Shaders/pbr_fragment.glsl");
}