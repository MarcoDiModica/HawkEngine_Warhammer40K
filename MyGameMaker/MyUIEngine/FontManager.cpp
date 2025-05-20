#include "FontManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

FontManager& FontManager::GetInstance() {
    static FontManager instance;
    return instance;
}

FontManager::FontManager() {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR: No se pudo inicializar FreeType Library" << std::endl;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

FontManager::~FontManager() {
    FT_Done_FreeType(ft);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void FontManager::Start() {
    const std::string defaultFontPath = "Assets/Halvert-Solid.oft";
    int defaultFontSize = 4;

    if (!LoadFont(defaultFontPath, defaultFontSize)) {
        std::cerr << "ERROR: No se pudo cargar la fuente predeterminada en FontManager::Start" << std::endl;
    }
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

        if (!face->glyph->bitmap.buffer || face->glyph->bitmap.width == 0 || face->glyph->bitmap.rows == 0) {
            std::cerr << "[WARNING] Glyph vacío para char '" << c << "' en " << fontPath << ". Usando carácter de reemplazo." << std::endl;

            GLuint emptyTexture;
            glGenTextures(1, &emptyTexture);
            glBindTexture(GL_TEXTURE_2D, emptyTexture);

            unsigned char emptyBitmap[4] = { 0, 0, 0, 0 };
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emptyBitmap);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character replacementCharacter = {
                emptyTexture,
                glm::ivec2(1, 1),
                glm::ivec2(0, 0),
                static_cast<GLuint>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, replacementCharacter));
            continue;
        }

        int width = face->glyph->bitmap.width;
        int height = face->glyph->bitmap.rows;
        std::cout << "[LoadFont] Glyph '" << c << "' - Size: (" << width << "x" << height << ") Pitch: " << face->glyph->bitmap.pitch << "\n";

        GLuint texture;
        glGenTextures(1, &texture);
        if (texture == 0) {
            std::cerr << "ERROR: No se pudo generar la textura para el carácter: " << c << std::endl;
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, texture);

        const unsigned char* buffer = face->glyph->bitmap.buffer;
        std::vector<unsigned char> rgbaBuffer(width * height * 4, 255);
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                int gray = buffer[j * face->glyph->bitmap.pitch + i];
                int idx = (j * width + i) * 4;
                rgbaBuffer[idx + 0] = 255;
                rgbaBuffer[idx + 1] = 255;
                rgbaBuffer[idx + 2] = 255;
                rgbaBuffer[idx + 3] = gray;
            }
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgbaBuffer.data()
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(width, height),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Fuente cargada correctamente: " << fontPath << " con tamaño " << fontSize << std::endl;
    return true;
}

void FontManager::RenderTextBoxedWithShader(Shaders* shader, const std::string& text, float x, float y, float scale, float spacing, const glm::vec2& boxSize) {
    if (!isFontLoaded) {
        LoadFont("Assets/Halvert_Solid.ttf", 16);
        isFontLoaded = true;
    }
    if (Characters.empty()) return;

    shader->Bind();
    shader->SetUniform("u_HasTexture", true);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float maxWidth = boxSize.x;
    float cursorX = x - maxWidth / 2.0f;
    float cursorY = y;
    float lineHeight = 0.0f;

    spacing = 2.5f; 

    std::istringstream stream(text);
    std::string word;
    float spaceWidth = (Characters[' '].Advance >> 6) * scale;

    while (stream >> word) {
        float wordWidth = 0.0f;
        float wordMaxHeight = 0.0f;

        for (char wc : word) {
            if (Characters.find(wc) == Characters.end()) continue;
            Character ch = Characters[wc];
            wordWidth += (ch.Advance >> 6) * scale;
            wordMaxHeight = std::max(wordMaxHeight, ch.Size.y * scale);
        }
        wordWidth += spaceWidth;

        if (cursorX + wordWidth > x + maxWidth / 2.0f) {
            cursorX = x - maxWidth / 2.0f;
            cursorY -= lineHeight * spacing;
            lineHeight = 0.0f;
        }

        for (char c : word) {
            if (Characters.find(c) == Characters.end()) continue;
            Character ch = Characters[c];

            float xpos = cursorX + ch.Bearing.x * scale;
            float ypos = cursorY - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            lineHeight = std::max(lineHeight, h);

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            shader->SetUniformVec2("SpriteOffset", glm::vec2(0.0f, 0.0f));
            shader->SetUniformVec2("SpriteSize", glm::vec2(ch.Size));
            shader->SetUniformVec2("SheetSize", glm::vec2(ch.Size));

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            cursorX += (ch.Advance >> 6) * scale;
        }

        cursorX += spaceWidth;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->UnBind();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}


glm::vec2 FontManager::CalculateTextBoxSize(const std::string& text, float scale) {
    float maxLineWidth = 0.0f;
    float currentLineWidth = 0.0f;
    float totalHeight = 0.0f;
    float lineHeight = 0.0f;

    std::istringstream stream(text);
    std::string word;

    float spaceWidth = (Characters[' '].Advance >> 6) * scale;

    while (stream >> word) {
        float wordWidth = 0.0f;
        float wordMaxHeight = 0.0f;

        for (char wc : word) {
            if (Characters.find(wc) == Characters.end()) continue;
            Character ch = Characters[wc];
            wordWidth += (ch.Advance >> 6) * scale;
            wordMaxHeight = std::max(wordMaxHeight, ch.Size.y * scale);
        }

        if (currentLineWidth + wordWidth > 500.0f) {
            totalHeight += lineHeight;
            maxLineWidth = std::max(maxLineWidth, currentLineWidth);
            currentLineWidth = 0.0f;
            lineHeight = 0.0f;
        }

        currentLineWidth += wordWidth + spaceWidth;
        lineHeight = std::max(lineHeight, wordMaxHeight);
    }

    totalHeight += lineHeight;
    maxLineWidth = std::max(maxLineWidth, currentLineWidth);

    return glm::vec2(maxLineWidth, totalHeight);
}
