#include "TextComponent.h"
#include <iostream>
#include "../MyGameEditor/UIGameView.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UISceneWindow.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "../MyGameEngine/ShaderManager.h"
#include "../MyUIEngine/FontManager.h"

TextComponent::TextComponent(const std::string& text, const glm::vec2& position, const glm::vec3& color, float fontSize)
    : m_text(text), m_position(position), m_color(color), m_fontSize(fontSize)
{
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
   Shaders* unlitShader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);  

   if (!unlitShader)  
   {  
       std::cerr << "ERROR: Shader UNLIT no encontrado." << std::endl;  
       return;  
   }  

   unlitShader->Bind();  

   unlitShader->SetUniform("uTextColor", m_color);  
   unlitShader->SetUniform("uProjection", m_projection);  

   FontManager::GetInstance().RenderText(unlitShader, m_text, m_position.x, m_position.y, m_fontSize);  

   unlitShader->UnBind();  

   std::cout << "Renderizando texto: " << m_text  
       << " en posición (" << m_position.x << ", " << m_position.y << ")"  
       << " con color (" << m_color.r << ", " << m_color.g << ", " << m_color.b << ")"  
       << " y tamaño de fuente " << m_fontSize << std::endl;  
}