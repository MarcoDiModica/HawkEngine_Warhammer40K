#include "FontManager.h"
#include <iostream>

FontManager& FontManager::GetInstance() {
    static FontManager instance;
    return instance;
}

FontManager::FontManager() {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR: No se pudo inicializar FreeType Library" << std::endl;
    }
}

FontManager::~FontManager() {
    FT_Done_FreeType(ft);
}

bool FontManager::LoadFont(const std::string& fontPath, int fontSize) {
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR: No se pudo cargar la fuente: " << fontPath << std::endl;
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Characters.clear();
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR: No se pudo cargar el carácter: " << c << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void FontManager::RenderText(Shaders* shader, const std::string& text, float x, float y, float scale) {
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0); 

    for (const char& c : text) {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;


        shader->SetUniform("uText", 0);
        shader->SetUniform("uTextColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Renderiza el carácter (aquí deberías usar un VAO/VBO configurado previamente)
        // Por simplicidad, este ejemplo no incluye la configuración del VAO/VBO.

        x += (ch.Advance >> 6) * scale;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
