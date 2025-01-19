#include "Particle.h"
#include "ParticlesEmitterComponent.h"

Particle* particle = nullptr;


void Particle :: Start() {
    particle = new Particle();
	
}

void Particle::Update(float deltaTime) {
    if (lifetime > 0) {
        // Actualizar la posici�n de la part�cula usando su velocidad        
		position[0] += speed[0] * deltaTime; 

        // Disminuir el tiempo de vida de la part�cula
        lifetime -= deltaTime;
    }
    else {
        // L�gica para cuando la vida de la part�cula se agota
        CleanUp();
    }
}

void Particle:: Spawn() {
    
      // L�gica de OpenGL para dibujar la part�cula como un plano
    glPushMatrix();
    glTranslatef(position[0].x, position[0].y, position[0].z);

    // Dibujar un plano (por ejemplo, un cuadrado) en la posici�n de la part�cula
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
    // L�gica para guardar el estado
}

void Load() {
    // L�gica para cargar el estado
}

void CleanUp() {
    delete particle;
    particle = nullptr;
}