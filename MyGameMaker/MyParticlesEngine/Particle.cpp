#include <random>

#include "Particle.h"
#include "ParticlesEmitterComponent.h"

#include "../MyGameEditor/Log.h"
#include <glm/gtc/type_ptr.hpp>

Particle::Particle()
    : lifetime(1.0f)
    , rotation(0.0f)
    , textureID(0)
    , texture(std::make_unique<Image>())
    , texture2(std::make_unique<Image>())
    , modelMatrix(glm::mat4(1.0f))
    , i(0.0f)
{
    if (!texture || !texture2) {
        throw std::runtime_error("Failed to initialize textures");
    }
}

void Particle :: Start() {}

void Particle::Update(float deltaTime) {
    if (lifetime > 0) {
        if (!position.empty() && !speed.empty()) {
            position[0] += speed[0] * deltaTime;
        }

        if (texture2 && lifetime <= 1.0f) {
            GLuint oldId = texture2->id();
            texture = std::move(texture2);

            if (texture->id() != oldId) {
                textureID = oldId;
            }
        }

        lifetime -= deltaTime;
    }
}

void Particle::Draw(const glm::mat4& billboardMatrix) {
    if (!texture || lifetime <= 0.0f) {
        return;
    }

    if (textureID == 0) {
        textureID = texture->id();
        if (textureID == 0) {
            return;
        }
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    float modelMatrixArray[16]{};
    const float* billboardMatrixArray = glm::value_ptr(billboardMatrix);
    for (int i = 0; i < 16; i++) {
        modelMatrixArray[i] = billboardMatrixArray[i];
    }
    glMultMatrixf(modelMatrixArray);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    float size = 1.0f;
    float alpha = std::min(1.0f, lifetime);
    if (lifetime < 0.1f) {
        alpha *= (lifetime / 0.1f);
    }

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, 0.0f);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();
}

void Particle::Draw2(const glm::mat4& billboardMatrix) {
    if (!texture || lifetime <= 0.0f) {
        return;
    }

    if (textureID == 0) {
        textureID = texture->id();
        if (textureID == 0) {
            return;
        }
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    float modelMatrixArray[16]{};
    const float* billboardMatrixArray = glm::value_ptr(billboardMatrix);
    for (int i = 0; i < 16; i++) {
        modelMatrixArray[i] = billboardMatrixArray[i];
    }
    glMultMatrixf(modelMatrixArray);

    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    float randomSize = ((float)rand() / RAND_MAX) * 2.5f + 0.5f;
    float alpha = std::min(1.0f, lifetime);
    if (lifetime < 0.1f) {
        alpha *= (lifetime / 0.1f);
    }

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-randomSize / 2, -randomSize / 2, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(randomSize / 2, -randomSize / 2, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(randomSize / 2, randomSize / 2, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-randomSize / 2, randomSize / 2, 0.0f);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();
}

void Particle::SetParticleSpeed(const glm::vec3& newSpeed) {
    if (!speed.empty()) {
        speed[0] = newSpeed;
    }
    else {
        speed.push_back(newSpeed);
    }
}

void Particle::Spawn() 
{
    position.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    LOG(LogType::LOG_INFO, "Particle spawned at position: %f, %f, %f",
        position[0].x, position[0].y, position[0].z);
}