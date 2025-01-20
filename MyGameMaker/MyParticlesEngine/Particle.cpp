#include "Particle.h"
#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/Log.h"
#include "../MyGameEngine/Image.h"

Particle* particle = nullptr;


void Particle :: Start() {
    particle = new Particle();
	
}

void Particle::Update(float deltaTime) {
    if (lifetime > 0) {
        // Actualizar la posici�n de la part�cula usando su velocidad        
		position[0] += speed[0] * deltaTime; 

		Draw();
        // Disminuir el tiempo de vida de la part�cula
        lifetime -= deltaTime;
    }
    else {
        // L�gica para cuando la vida de la part�cula se agota
        CleanUp();
    }
}

void Particle::Spawn() {

    position.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    LOG(LogType::LOG_INFO, "Part�cula generada en la posici�n: %f, %f, %f", position[0].x, position[0].y, position[0].z);
}

void Particle::Draw() {

    texture->LoadTexture("../MyGameEditor/Assets/Textures/SmokeParticleTexture.png");
    textureID = texture->id();

    if (textureID == 0) {
        std::cout << "Textura no cargada" << std::endl;
        return; // Asegurarse de que la textura est� cargada
    }

    glPushMatrix();
    glTranslatef(position[0].x, position[0].y, position[0].z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    // Especificar las coordenadas de textura y las cuatro esquinas del plano
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f); // Esquina inferior izquierda
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.0f);  // Esquina inferior derecha
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.0f);   // Esquina superior derecha
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.0f);  // Esquina superior izquierda
    glEnd();

    glDisable(GL_TEXTURE_2D);

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