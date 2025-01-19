#include "Particle.h"
#include "ParticlesEmitterComponent.h"

Particle* particle = nullptr;


void Start() {
    particle = new Particle();
	
}

void Update(ParticlesEmitterComponent* component) {
    if (particle->lifetime <= 0) {
        // Lógica para cuando la vida de la partícula se agota
		CleanUp();
    }
}

void Spawn(ParticlesEmitterComponent* component) {
    // Asumiendo que ParticleEmitterComponent tiene una función para obtener la posición
    glm::vec3 spawnPosition = component->GetPosition();

    // Crear una nueva partícula
    Particle* newParticle = new Particle();
    newParticle->position.push_back(spawnPosition);
    newParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial

    // Configurar otros parámetros de la partícula
    newParticle->lifetime = 5.0f; // Duración de la partícula en segundos
    newParticle->rotation = 0.0f; // Rotación inicial

    // Aquí podrías agregar la nueva partícula a una lista de partículas activas
    // Por ejemplo:
    // activeParticles.push_back(newParticle);

    // Lógica de OpenGL para dibujar la partícula
    glPushMatrix();
    glTranslatef(spawnPosition.x, spawnPosition.y, spawnPosition.z);
    // Aquí podrías dibujar la partícula, por ejemplo, como un punto o un sprite
    glBegin(GL_POINTS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
}
void Safe() {
    // Lógica para guardar el estado
}

void Load() {
    // Lógica para cargar el estado
}

void CleanUp() {
    delete particle;
    particle = nullptr;
}