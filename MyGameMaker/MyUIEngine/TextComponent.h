#pragma once  
#include <string>  
#include <glm/glm.hpp>  
#include "../MyGameEngine/GameObject.h"


class TextComponent : public Component
{
public:
    TextComponent(GameObject* owner, const std::string& text = "", const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec3& color = { 1.0f, 1.0f, 1.0f }, float fontSize = 16.0f);
    TextComponent(const TextComponent&) = delete;
    TextComponent& operator=(const TextComponent&) = delete;

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

    ComponentType GetType() const override { return ComponentType::UI; }
	void Awake() override {}
	void Start() override {}
	void Update(float deltaTime) override {}
	void Destroy() override {}
	
	std::unique_ptr<Component> Clone(GameObject* owner) override
	{
		return std::make_unique<TextComponent>(owner, m_text, m_position, m_color, m_fontSize);
	}
    // New method to access the owner  
    GameObject* GetOwner() const { return m_owner; }

    // Agregar instancia de clase abstracta
    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

private:
    GameObject* m_owner; // Pointer to the owning GameObject  
    std::string m_text;
    glm::vec2 m_position;
    glm::vec3 m_color;
    float m_fontSize;
    glm::mat4 m_projection;
};
