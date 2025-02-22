#pragma once
#include <iostream>
#include <math.h>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "../MyGameEngine/Image.h"

class Particle {
public:
    Particle();
    ~Particle() = default;

    Particle(const Particle&) = default;
    Particle& operator=(const Particle&) = default;

    Particle(Particle&&) noexcept = default;
    Particle& operator=(Particle&&) noexcept = default;

    void Update(float deltaTime);
    void Spawn();
    void Draw();
    void Draw2();
    void Start();
    void SetParticleSpeed(const glm::vec3& newSpeed);

    std::string name;
    float lifetime;
    float rotation;
    float i;
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> speed;
    GLuint textureID;

    std::unique_ptr<Image> texture;
    std::unique_ptr<Image> texture2;

private:
    std::string goodmorning = "Good Morning";
};


