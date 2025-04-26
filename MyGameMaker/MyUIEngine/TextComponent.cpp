#include "TextComponent.h"
#include <iostream>
#include "MyScriptingEngine/MonoManager.h"
#include "../MyGameEngine/ShaderManager.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyUIEngine/FontManager.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>

TextComponent::TextComponent(GameObject* owner, const std::string& text, const glm::vec2& position, const glm::vec3& color, float fontSize)
    : Component(owner), m_owner(owner), m_text(text), m_position(position), m_color(color), m_fontSize(fontSize)
{
}

MonoObject* TextComponent::GetSharp() {
    return CsharpReference;
}

void TextComponent::SetText(const std::string& text) {
    m_text = text;
}

void TextComponent::SetPosition(const glm::vec2& position) {
    m_position = position;
}

void TextComponent::SetColor(const glm::vec3& color) {
    m_color = color;
}

void TextComponent::SetFontSize(float fontSize) {
    m_fontSize = fontSize;
}

const std::string& TextComponent::GetText() const {
    return m_text;
}

const glm::vec2& TextComponent::GetPosition() const {
    return m_position;
}

const glm::vec3& TextComponent::GetColor() const {
    return m_color;
}

float TextComponent::GetFontSize() const {
    return m_fontSize;
}

void TextComponent::Render() const {
    Shaders* customShader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);
    if (!customShader) {
        std::cerr << "ERROR: Shader UNLIT no encontrado." << std::endl;
        return;
    }

    float screenWidth = Application->window->width();
    float screenHeight = Application->window->height();

    glm::mat4 orthoProjection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    customShader->Bind();
    customShader->SetUniform("projection", orthoProjection);
    customShader->SetUniform("view", viewMatrix);
    customShader->SetUniform("model", modelMatrix);
    customShader->SetUniform("modColor", glm::vec4(m_color, 1.0f));

    glm::vec2 renderPosition = m_position;
    if (m_owner->HasComponent<UITransformComponent>()) {
        UITransformComponent* rectTransform = m_owner->GetComponent<UITransformComponent>();
        renderPosition = rectTransform->GetPosition();
    }

    // renderPosition ya es el correcto para RenderTextWithShader
    FontManager::GetInstance().RenderTextWithShader(customShader, m_text, renderPosition.x, renderPosition.y, m_fontSize);

    customShader->UnBind();

    std::cout << "Renderizando texto: " << m_text
        << " en posición (" << renderPosition.x << ", " << renderPosition.y << ")"
        << " color (" << m_color.r << ", " << m_color.g << ", " << m_color.b << ")"
        << " tamaño fuente " << m_fontSize << std::endl;
}

void TextComponent::Update(float deltaTime) {
    if (CsharpReference) {
        MonoClass* textClass = mono_object_get_class(CsharpReference);
        MonoMethod* updateMethod = mono_class_get_method_from_name(textClass, "Update", 0);
        if (updateMethod) {
            MonoObject* exception = nullptr;
            mono_runtime_invoke(updateMethod, CsharpReference, nullptr, &exception);
            if (exception) {
                MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
                const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
                LOG(LogType::LOG_ERROR, "UpdateError: %s", exceptionStr);
                mono_free((void*)exceptionStr);
            }
        }
    }
    Render();
}