#include <random>

#include "Particle.h"
#include "ParticlesEmitterComponent.h"

#include "../MyGameEditor/Log.h"

Particle::Particle()
    : lifetime(1.0f)
    , rotation(0.0f)
    , textureID(0)
    , texture(std::make_unique<Image>())
    , texture2(std::make_unique<Image>())
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
            texture = std::move(texture2);
            Draw2();
        }
        else {
            Draw();
        }
        lifetime -= deltaTime;
    }
}

void Particle::Draw() {
    if (!texture) {
        std::cerr << "Texture not initialized" << std::endl;
        return;
    }

    textureID = texture->id();
    if (textureID == 0) {
        std::cerr << "Texture not loaded" << std::endl;
        return;
    }

    std::cout << "Drawing particle at position: " << position[0].x << ", " << position[0].y << ", " << position[0].z << std::endl;
    std::cout << "Using texture ID: " << textureID << std::endl;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    glTranslatef(position[0].x, position[0].y, position[0].z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    float size = 1.0f;
    float alpha = std::min(1.0f, lifetime);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, 0.0f);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();
}

void Particle::Draw2() {
    if (!texture) {
        std::cerr << "Texture not initialized" << std::endl;
        return;
    }

    textureID = texture->id();
    if (textureID == 0) {
        std::cerr << "Texture not loaded" << std::endl;
        return;
    }

    std::cout << "Drawing particle type 2 at position: " << position[0].x << ", " << position[0].y << ", " << position[0].z << std::endl;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    glTranslatef(position[0].x, position[0].y, position[0].z);

    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    float randomSize = ((float)rand() / RAND_MAX) * 2.5f + 0.5f;
    float alpha = std::min(1.0f, lifetime);

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