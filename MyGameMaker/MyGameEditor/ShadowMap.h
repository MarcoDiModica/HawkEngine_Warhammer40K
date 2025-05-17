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

private:
    GLuint fbo;
    GLuint shadowMap;
    int width, height;
    glm::mat4 lightSpaceMatrix;
};