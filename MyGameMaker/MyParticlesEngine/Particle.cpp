#include "Particle.h"
#include "ParticlesEmitterComponent.h"

Particle* particle = nullptr;


void Start() {
    particle = new Particle();
	
}

void Update(ParticlesEmitterComponent* component) {
    if (particle->lifetime <= 0) {
        // L�gica para cuando la vida de la part�cula se agota
		CleanUp();
    }
}

void Spawn(ParticlesEmitterComponent* component) {
    // Asumiendo que ParticleEmitterComponent tiene una funci�n para obtener la posici�n
    glm::vec3 spawnPosition = component->GetPosition();

    // Crear una nueva part�cula
    Particle* newParticle = new Particle();
    newParticle->position.push_back(spawnPosition);
    newParticle->speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Velocidad inicial

    // Configurar otros par�metros de la part�cula
    newParticle->lifetime = 5.0f; // Duraci�n de la part�cula en segundos
    newParticle->rotation = 0.0f; // Rotaci�n inicial

    // Aqu� podr�as agregar la nueva part�cula a una lista de part�culas activas
    // Por ejemplo:
    // activeParticles.push_back(newParticle);

    // L�gica de OpenGL para dibujar la part�cula
    glPushMatrix();
    glTranslatef(spawnPosition.x, spawnPosition.y, spawnPosition.z);
    // Aqu� podr�as dibujar la part�cula, por ejemplo, como un punto o un sprite
    glBegin(GL_POINTS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
}
void Safe() {
    // L�gica para guardar el estado
}

void Load() {
    // L�gica para cargar el estado
}

void CleanUp() {
    delete particle;
    particle = nullptr;
}