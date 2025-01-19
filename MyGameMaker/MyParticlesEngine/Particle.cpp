#include "Particle.h"
#include "ParticlesEmitterComponent.h"

Particle* particle = nullptr;


void Particle :: Start() {
    particle = new Particle();
	
}

void Particle::Update(float deltaTime) {
    if (lifetime > 0) {
        // Actualizar la posición de la partícula usando su velocidad        
		position[0] += speed[0] * deltaTime; 

        // Disminuir el tiempo de vida de la partícula
        lifetime -= deltaTime;
    }
    else {
        // Lógica para cuando la vida de la partícula se agota
        CleanUp();
    }
}

void Particle:: Spawn() {
    
      // Lógica de OpenGL para dibujar la partícula como un plano
    glPushMatrix();
    glTranslatef(position[0].x, position[0].y, position[0].z);

    // Dibujar un plano (por ejemplo, un cuadrado) en la posición de la partícula
    glBegin(GL_QUADS);
    // Especificar las cuatro esquinas del plano
    glVertex3f(-0.5f, -0.5f, 0.0f); // Esquina inferior izquierda
    glVertex3f(0.5f, -0.5f, 0.0f);  // Esquina inferior derecha
    glVertex3f(0.5f, 0.5f, 0.0f);   // Esquina superior derecha
    glVertex3f(-0.5f, 0.5f, 0.0f);  // Esquina superior izquierda
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