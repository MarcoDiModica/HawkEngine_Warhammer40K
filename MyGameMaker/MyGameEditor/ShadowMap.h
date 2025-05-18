#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();

    bool Initialize(int width, int height);
    void BindForWriting();
    void BindForReading(GLenum textureUnit);

    glm::mat4 GetLightSpaceMatrix() const { return lightSpaceMatrix; }
    void SetLightSpaceMatrix(const glm::mat4& matrix) { lightSpaceMatrix = matrix; }

    GLuint GetFBO() const { return fbo; }
    GLuint GetTexture() const { return shadowMap; }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	void SetSize(int width, int height) {
		this->width = width;
		this->height = height;
	}

private:
    GLuint fbo;
    GLuint shadowMap;
    int width, height;
    glm::mat4 lightSpaceMatrix;
};