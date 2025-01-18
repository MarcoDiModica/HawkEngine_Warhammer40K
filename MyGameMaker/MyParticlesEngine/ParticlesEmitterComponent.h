#pragma once
#include <glm/glm.hpp>

class ParticlesEmitterComponent {
public:
    ParticlesEmitterComponent();
    ~ParticlesEmitterComponent();

    glm::vec3 GetPosition() const;

private:
    glm::vec3 position;
};;