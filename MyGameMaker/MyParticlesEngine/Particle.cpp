#include "Particle.h"
#include "ParticlesEmitterComponent.h"
#include "../MyGameEditor/Log.h"
#include "../MyGameEngine/Image.h"

Particle* particle = nullptr;

Particle::Particle() {
    lifetime = 5.0f;
    rotation = 0.0f;
   /* position.push_back(glm::vec3(-14, 1, -10));*/
    speed.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texture = new Image();
    if (texture == nullptr) {
        std::cerr << "Error al inicializar la textura" << std::endl;
    }
}

Particle::~Particle() {
    
}

void Particle :: Start() {
    if (particle == nullptr) {
        particle = new Particle();
    }	
}

void Particle::Update(float deltaTime) {
    if (lifetime > 0) {
        // Actualizar la posición de la partícula usando su velocidad
        position[0] += speed[0] * deltaTime;

        Draw();
        // Disminuir el tiempo de vida de la partícula
        lifetime -= deltaTime;
    }
    else {
        // Lógica para cuando la vida de la partícula se agota
        CleanUp();
    }
}

void Particle::Spawn() {

    position.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    LOG(LogType::LOG_INFO, "Partícula generada en la posición: %f, %f, %f", position[0].x, position[0].y, position[0].z);
}

void Particle::Draw() {

    if (texture == nullptr) {
        std::cout << "Textura no inicializada" << std::endl;
        return;
    }

    //texture->LoadTexture("../MyGameEditor/Assets/Textures/SmokeParticleTexture.png");
    textureID = texture->id();

    if (textureID == 0) {
        std::cout << "Textura no cargada" << std::endl;
        return; // Asegurarse de que la textura está cargada
    }
 
    glPushMatrix();
    glTranslatef(position[0].x, position[0].y, position[0].z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    // Especificar las coordenadas de textura y las cuatro esquinas del plano
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f); // Esquina inferior izquierda
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(0.5f, -0.5f, 0.0f);  // Esquina inferior derecha
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(0.5f, 0.5f, 0.0f);   // Esquina superior derecha
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-0.5f, 0.5f, 0.0f);  // Esquina superior izquierda
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}
void Safe() {
    // Lógica para guardar el estado
}

void Load() {
    // Lógica para cargar el estado
}

void Particle::CleanUp() {
	// Lógica para liberar recursos
    if (particle != nullptr) {
        delete particle;
        particle = nullptr;
    }

    if (texture != nullptr) {
        delete texture;
        texture = nullptr;
    }
}