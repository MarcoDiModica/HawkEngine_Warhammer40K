#pragma once
#include <string>
#include <glm/glm.hpp>

class TextComponent
{
public:
    TextComponent(const std::string& text = "", const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec3& color = { 1.0f, 1.0f, 1.0f }, float fontSize = 16.0f);

    // Métodos para configurar el texto
    void SetText(const std::string& text);
    void SetPosition(const glm::vec2& position);
    void SetColor(const glm::vec3& color);
    void SetFontSize(float fontSize);

    // Métodos para obtener propiedades
    const std::string& GetText() const;
    const glm::vec2& GetPosition() const;
    const glm::vec3& GetColor() const;
    float GetFontSize() const;

    // Método para renderizar el texto
    void Render() const;

private:
    std::string m_text;
    glm::vec2 m_position;
    glm::vec3 m_color;
    float m_fontSize;
	glm::mat4 m_projection;
};


