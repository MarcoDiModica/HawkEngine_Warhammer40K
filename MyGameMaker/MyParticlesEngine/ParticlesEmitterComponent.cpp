#include "ParticlesEmitterComponent.h"

ParticlesEmitterComponent::ParticlesEmitterComponent()
    : position(0.0f, 0.0f, 0.0f) // Inicializar la posición a (0, 0, 0)
{
}

ParticlesEmitterComponent::~ParticlesEmitterComponent()
{
}

glm::vec3 ParticlesEmitterComponent::GetPosition() const {
    return position;
}