#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "types.h"

class Shaders
{
public:
	Shaders();
	virtual ~Shaders();

	bool LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	void Bind() const;
	void UnBind() const;
	GLuint GetProgram() const;

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const glm::vec3& value);
	void SetUniform(const std::string& name, const glm::vec4& value);
	void SetUniform(const std::string& name, const glm::mat4& value);

	virtual ShaderType GetShaderType() const = 0;

protected:
	GLuint CompileShader(const std::string& shaderSource, GLenum shaderType);
	std::string LoadShaderSource(const std::string& shaderFile);

	GLint GetUniformLocation(const std::string& name);

	// Support for shader preprocessing (for future extensions)
	virtual std::string PreprocessShader(const std::string& source);

	GLuint _program;
	std::unordered_map<std::string, GLint> uniformLocationsCache;
};

class UnlitShader : public Shaders {
public:
	UnlitShader();
	ShaderType GetShaderType() const override { return ShaderType::UNLIT; }

	bool Initialize();
};

class PBRShader : public Shaders {
public:
	PBRShader();
	ShaderType GetShaderType() const override { return ShaderType::PBR; }

	bool Initialize();

	// Shadow-mapping related methods should be added here in the future or maybe in a separate file
};