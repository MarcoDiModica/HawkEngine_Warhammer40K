#include "TextComponent.h"
#include <iostream>
#include "MyScriptingEngine/MonoManager.h"
#include "../MyGameEngine/ShaderManager.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyUIEngine/FontManager.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEngine/CameraComponent.h"

TextComponent::TextComponent(GameObject* owner, const std::string& text, const glm::vec2& position, const glm::vec3& color, float fontSize)
    : Component(owner), m_owner(owner), m_text(text), m_position(position), m_color(color), m_fontSize(fontSize)
{
}

MonoObject* TextComponent::GetSharp()
{
    return CsharpReference;
}

void TextComponent::SetText(const std::string& text)
{
    m_text = text;
}

void TextComponent::SetPosition(const glm::vec2& position)
{
    m_position = position;
}

void TextComponent::SetColor(const glm::vec3& color)
{
    m_color = color;
}

void TextComponent::SetFontSize(float fontSize)
{
    m_fontSize = fontSize;
}

const std::string& TextComponent::GetText() const
{
    return m_text;
}

const glm::vec2& TextComponent::GetPosition() const
{
    return m_position;
}

const glm::vec3& TextComponent::GetColor() const
{
    return m_color;
}

float TextComponent::GetFontSize() const
{
    return m_fontSize;
}

void TextComponent::Render() const
{
    // Obtener el shader
    Shaders* unlitShader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);
    if (!unlitShader) {
        std::cerr << "ERROR: Shader UNLIT no encontrado." << std::endl;
        return;
    }

    CameraComponent* mainCamera = Application->root->mainCamera->GetComponent<CameraComponent>();
    if (!mainCamera) {
        std::cerr << "ERROR: No se encontró la cámara principal." << std::endl;
        return;
    }

    glm::mat4 projectionMatrix = mainCamera->projection();
    glm::mat4 viewMatrix = mainCamera->view();

    unlitShader->Bind();
    unlitShader->SetUniform("uProjection", projectionMatrix);
    unlitShader->SetUniform("uView", viewMatrix);

    glm::vec2 renderPosition = m_position;
    if (m_owner->HasComponent<UITransformComponent>()) {
        UITransformComponent* rectTransform = m_owner->GetComponent<UITransformComponent>();
        renderPosition = glm::vec2(rectTransform->GetPosition().x, rectTransform->GetPosition().y);
    }

    FontManager::GetInstance().LoadFont("assets/arial_narrow_7.ttf", m_fontSize);
    FontManager::GetInstance().RenderText(unlitShader, m_text, renderPosition.x, renderPosition.y, m_fontSize, m_color);

    unlitShader->UnBind();

    std::cout << "Renderizando texto: " << m_text
        << " en posición (" << renderPosition.x << ", " << renderPosition.y << ")"
        << " con color (" << m_color.r << ", " << m_color.g << ", " << m_color.b << ")"
        << " y tamaño de fuente " << m_fontSize << std::endl;
}