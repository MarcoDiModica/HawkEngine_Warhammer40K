#include "TextComponent.h"
#include <iostream>
#include "MyScriptingEngine/MonoManager.h"
#include "../MyGameEngine/ShaderManager.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyUIEngine/FontManager.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TextComponent::TextComponent(GameObject* owner, const std::string& text, const glm::vec2& position, const glm::vec3& color, float fontSize)
    : Component(owner), m_text(text), m_position(position), m_color(color), m_fontSize(fontSize)
{
    name = "TextComponent";
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

glm::vec2 TextComponent::GetBoxSize() const {
	return m_boxSize;
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
    glm::vec2 renderPosition = m_position;

    if (owner->HasComponent<UITransformComponent>()) {
        UITransformComponent* rectTransform = owner->GetComponent<UITransformComponent>();
        glm::vec2 uiPosition = rectTransform->GetPosition();

        renderPosition.x = uiPosition.x * screenWidth;
        renderPosition.y = uiPosition.y * screenHeight;
    }

    const float scaleFactor = m_fontSize / 10.0f;
    if (!m_overrideBoxSize) {
        const_cast<TextComponent*>(this)->m_boxSize = FontManager::GetInstance().CalculateTextBoxSize(m_text, scaleFactor);
    }

    float centeredX = renderPosition.x - m_boxSize.x / 2.0f;
    float centeredY = renderPosition.y;

    if (m_debugDrawBox) {
        float left = centeredX;
        float right = centeredX + m_boxSize.x;
        float top = centeredY + m_boxSize.y / 2.0f;
        float bottom = centeredY - m_boxSize.y / 2.0f;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(orthoProjection));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);
        glVertex2f(left, bottom);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    customShader->Bind();
    customShader->SetUniform("projection", orthoProjection);
    customShader->SetUniform("view", viewMatrix);
    customShader->SetUniform("model", modelMatrix);
    customShader->SetUniform("modColor", glm::vec4(m_color, 1.0f));

    FontManager::GetInstance().RenderTextBoxedWithShader(customShader, m_text, renderPosition.x, renderPosition.y, scaleFactor, m_boxSize);
    customShader->UnBind();

    std::cout << "Renderizando texto: " << m_text
        << " en posición (" << renderPosition.x << ", " << renderPosition.y << ")"
        << " color (" << m_color.r << ", " << m_color.g << ", " << m_color.b << ")"
        << " tamaño fuente " << m_fontSize << std::endl;
}

void TextComponent::SetBoxSize(const glm::vec2& size) {
    m_boxSize = size;
    m_overrideBoxSize = true;
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