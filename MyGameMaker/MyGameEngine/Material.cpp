#include "Material.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <iostream> 

bool Material::LoadTexture(const std::string& filePath)
{
    std::wstring wFilePath(filePath.begin(), filePath.end());

    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);

    if (!ilLoadImage(wFilePath.c_str())) {
        std::cerr << "Error al cargar la imagen: " << filePath << std::endl;
        ilDeleteImages(1, &image);
        CreateDefaultTexture();
        return false;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    ilDeleteImages(1, &image);

    return true;
}


void Material::CreateDefaultTexture() 
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char white[] = { 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);

    // Configurar los parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    // Desactivar la textura
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}