#include "Shaders.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "MyGameEditor/Log.h"

Shaders::Shaders() : _program(0), isComputeShader(false) {}

Shaders::~Shaders() {
	if (_program != 0) {
		glDeleteProgram(_program);
		_program = 0;
	}
}

bool Shaders::LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
	vertexShaderPath = vertexShaderFile;
	fragmentShaderPath = fragmentShaderFile;

	std::string vertexShaderSource = LoadShaderSource(vertexShaderFile);
	std::string fragmentShaderSource = LoadShaderSource(fragmentShaderFile);

	if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
		LOG(LogType::LOG_ERROR, "Failed to load shader source files: %s, %s", vertexShaderFile.c_str(), fragmentShaderFile.c_str());
		return false;
	}

	vertexShaderSource = PreprocessShader(vertexShaderSource);
	fragmentShaderSource = PreprocessShader(fragmentShaderSource);

	GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	if (vertexShader == 0 || fragmentShader == 0) {
		return false;
	}

	if (_program != 0) {
		glDeleteProgram(_program);
	}

	_program = glCreateProgram();
	glAttachShader(_program, vertexShader);
	glAttachShader(_program, fragmentShader);
	glLinkProgram(_program);

	GLint success;
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(_program, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	isComputeShader = false;

	return true;
}

bool Shaders::LoadShadersFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
	vertexShaderPath.clear();
	fragmentShaderPath.clear();

	std::string processedVertexSource = PreprocessShader(vertexSource);
	std::string processedFragmentSource = PreprocessShader(fragmentSource);

	GLuint vertexShader = CompileShader(processedVertexSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(processedFragmentSource, GL_FRAGMENT_SHADER);

	if (vertexShader == 0 || fragmentShader == 0) {
		return false;
	}

	if (_program != 0) {
		glDeleteProgram(_program);
	}

	_program = glCreateProgram();
	glAttachShader(_program, vertexShader);
	glAttachShader(_program, fragmentShader);
	glLinkProgram(_program);

	GLint success;
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(_program, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	isComputeShader = false;

	return true;
}

bool Shaders::LoadComputeShader(const std::string& computeShaderFile) {
	computeShaderPath = computeShaderFile;

	std::string computeShaderSource = LoadShaderSource(computeShaderFile);

	if (computeShaderSource.empty()) {
		LOG(LogType::LOG_ERROR, "Failed to load compute shader source file: %s", computeShaderFile.c_str());
		return false;
	}

	computeShaderSource = PreprocessShader(computeShaderSource);

	GLuint computeShader = CompileShader(computeShaderSource, GL_COMPUTE_SHADER);

	if (computeShader == 0) {
		return false;
	}

	if (_program != 0) {
		glDeleteProgram(_program);
	}

	_program = glCreateProgram();
	glAttachShader(_program, computeShader);
	glLinkProgram(_program);

	GLint success;
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(_program, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		return false;
	}

	glDeleteShader(computeShader);

	isComputeShader = true;

	return true;
}

bool Shaders::LoadComputeShaderFromSource(const std::string& computeSource) {
	computeShaderPath.clear();

	std::string processedComputeSource = PreprocessShader(computeSource);

	GLuint computeShader = CompileShader(processedComputeSource, GL_COMPUTE_SHADER);

	if (computeShader == 0) {
		return false;
	}

	if (_program != 0) {
		glDeleteProgram(_program);
	}

	_program = glCreateProgram();
	glAttachShader(_program, computeShader);
	glLinkProgram(_program);

	GLint success;
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(_program, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		return false;
	}

	glDeleteShader(computeShader);

	isComputeShader = true;

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

bool Shaders::Reload() {
	uniformLocationsCache.clear();
	uniformBlockCache.clear();
	storageBlockCache.clear();

	if (isComputeShader) {
		if (!computeShaderPath.empty()) {
			return LoadComputeShader(computeShaderPath);
		}
	}
	else {
		if (!vertexShaderPath.empty() && !fragmentShaderPath.empty()) {
			return LoadShaders(vertexShaderPath, fragmentShaderPath);
		}
	}

	return false;
}

void Shaders::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) {
	if (!isComputeShader) {
		LOG(LogType::LOG_ERROR, "Error: Trying to dispatch a non-compute shader");
		return;
	}

	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void Shaders::DispatchBarrier(GLbitfield barriers) {
	glMemoryBarrier(barriers);
}

GLint Shaders::GetUniformLocation(const std::string& name) {
	auto it = uniformLocationsCache.find(name);
	if (it != uniformLocationsCache.end()) {
		return it->second;
	}

	GLint location = glGetUniformLocation(_program, name.c_str());
	uniformLocationsCache[name] = location;

	if (location == -1 && name[0] != '_') {
		LOG(LogType::LOG_WARNING, "Warning: Uniform '%s' not found in shader.", name.c_str());
		//log the name of the shader
		std::string shaderName = vertexShaderPath.empty() ? fragmentShaderPath : vertexShaderPath;
		LOG(LogType::LOG_WARNING, "Shader: %s", shaderName.c_str());
	}

	return location;
}

GLuint Shaders::GetUniformBlockIndex(const std::string& name) {
	auto it = uniformBlockCache.find(name);
	if (it != uniformBlockCache.end()) {
		return it->second;
	}

	GLuint index = glGetUniformBlockIndex(_program, name.c_str());
	uniformBlockCache[name] = index;

	return index;
}

GLuint Shaders::GetStorageBlockIndex(const std::string& name) {
	auto it = storageBlockCache.find(name);
	if (it != storageBlockCache.end()) {
		return it->second;
	}

	GLuint index = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, name.c_str());
	storageBlockCache[name] = index;

	return index;
}

GLuint Shaders::CompileShader(const std::string& shaderSource, GLenum shaderType) {
	GLuint shader = glCreateShader(shaderType);
	const char* sourceCStr = shaderSource.c_str();
	glShaderSource(shader, 1, &sourceCStr, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024]; 
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

		std::string typeName;
		switch (shaderType) {
		case GL_VERTEX_SHADER: typeName = "Vertex"; break;
		case GL_FRAGMENT_SHADER: typeName = "Fragment"; break;
		case GL_COMPUTE_SHADER: typeName = "Compute"; break;
		default: typeName = "Unknown"; break;
		}

		LOG(LogType::LOG_ERROR, "ERROR::%s::SHADER::COMPILATION_FAILED\n%s", typeName.c_str(), infoLog);

		std::istringstream sourceStream(shaderSource);
		std::string line;
		int lineNumber = 1;
		std::cerr << "Shader source excerpt:" << std::endl;
		while (std::getline(sourceStream, line) && lineNumber <= 10) {
			std::cerr << lineNumber << ": " << line << std::endl;
			lineNumber++;
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

std::string Shaders::LoadShaderSource(const std::string& shaderFile) {
	std::ifstream file(shaderFile);
	if (!file.is_open()) {
		LOG(LogType::LOG_ERROR, "Failed to open shader file: %s", shaderFile.c_str());
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string Shaders::PreprocessShader(const std::string& source) {
	// This implementation can be extended to handle:
	// - #include directives
	// - #define constants
	// - Conditional compilation
	// - Shader variants
	return source;
}

void Shaders::SetUniform(const std::string& name, int value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shaders::SetUniform(const std::string& name, uint32_t value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1ui(location, value);
	}
}

void Shaders::SetUniform(const std::string& name, float value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1f(location, value);
	}
}

void Shaders::SetUniform(const std::string& name, bool value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, value ? 1 : 0);
	}
}

void Shaders::SetUniformVec2(const std::string& name, const glm::vec2& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform2fv(location, 1, glm::value_ptr(value));
	}
}

void Shaders::SetUniformVec3(const std::string& name, const glm::vec3& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
}

void Shaders::SetUniformVec4(const std::string& name, const glm::vec4& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}

void Shaders::SetUniformMat2(const std::string& name, const glm::mat2& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shaders::SetUniformMat3(const std::string& name, const glm::mat3& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shaders::SetUniformMat4(const std::string& name, const glm::mat4& value) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shaders::SetUniformArray(const std::string& name, const int* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1iv(location, count, values);
	}
}

void Shaders::SetUniformArray(const std::string& name, const float* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform1fv(location, count, values);
	}
}

void Shaders::SetUniformArray(const std::string& name, const glm::vec2* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform2fv(location, count, (const float*)values);
	}
}

void Shaders::SetUniformArray(const std::string& name, const glm::vec3* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform3fv(location, count, (const float*)values);
	}
}

void Shaders::SetUniformArray(const std::string& name, const glm::vec4* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniform4fv(location, count, (const float*)values);
	}
}

void Shaders::SetUniformArray(const std::string& name, const glm::mat4* values, int count) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformMatrix4fv(location, count, GL_FALSE, (const float*)values);
	}
}

void Shaders::SetUniform(const std::string& name, GLuint64 bindlessHandle) {
	GLint location = GetUniformLocation(name);
	if (location != -1) {
		glUniformHandleui64ARB(location, bindlessHandle);
	}
}

void Shaders::SetUniformBlockBinding(const std::string& name, GLuint bindingPoint) {
	GLuint blockIndex = GetUniformBlockIndex(name);
	if (blockIndex != GL_INVALID_INDEX) {
		glUniformBlockBinding(_program, blockIndex, bindingPoint);
	}
}

void Shaders::SetStorageBlockBinding(const std::string& name, GLuint bindingPoint) {
	GLuint blockIndex = GetStorageBlockIndex(name);
	if (blockIndex != GL_INVALID_INDEX) {
		glShaderStorageBlockBinding(_program, blockIndex, bindingPoint);
	}
}

UnlitShader::UnlitShader() {
}

bool UnlitShader::Initialize() {
	return LoadShaders("Assets/Shaders/unlit_vertex.glsl", "Assets/Shaders/unlit_fragment.glsl");
}

PBRShader::PBRShader() {
}

bool PBRShader::Initialize() {
	return LoadShaders("Assets/Shaders/pbr_vertex.glsl", "Assets/Shaders/pbr_fragment.glsl");
}

ForwardPlusComputeShader::ForwardPlusComputeShader() {
}

bool ForwardPlusComputeShader::Initialize() {
	return LoadComputeShader("Assets/Shaders/light_culling.glsl");
}

CullingComputeShader::CullingComputeShader() {
}

bool CullingComputeShader::Initialize() {
	return LoadComputeShader("Assets/Shaders/compute_culling.glsl");
}

ComputeShader::ComputeShader(const std::string& filename, ShaderType type)
	: filename(filename), shaderType(type) {
}

bool ComputeShader::Initialize() {
	return LoadComputeShader(filename);
}

CustomShader::CustomShader(const std::string& vertexFile, const std::string& fragmentFile, ShaderType type)
	: vertexFile(vertexFile), fragmentFile(fragmentFile), shaderType(type) {
}

bool CustomShader::Initialize() {
	return LoadShaders(vertexFile, fragmentFile);
}