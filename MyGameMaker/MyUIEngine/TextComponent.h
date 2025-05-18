#pragma once
#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>

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
    void SetBoxSize(float x, float y);
	void SetSpaceWidth(float spaceWidth) { m_spaceWidth = spaceWidth; }
    void SetDebugDrawBox(bool enabled) { m_debugDrawBox = enabled; }

    // M?todos para obtener propiedades  
    const std::string& GetText() const;
    const glm::vec2& GetPosition() const;
    const glm::vec3& GetColor() const;
    glm::vec2 GetBoxSize() const;
    bool GetDebugDrawBox() const { return m_debugDrawBox; }
    float GetFontSize() const;
	float GetSpaceWidth() const { return m_spaceWidth; }
    void SetProjection(const glm::mat4& proj) { m_projection = proj; }
    void Render() const;

    ComponentType GetType() const override { return ComponentType::TEXT; }
    void Awake() override {}
    void Start() override {}
    void Update(float deltaTime) override;
    void Destroy() override {}

    std::unique_ptr<Component> Clone(GameObject* owner) override
    {
        return std::make_unique<TextComponent>(owner, m_text, m_position, m_color, m_fontSize);
    }
    // New method to access the owner  
    GameObject* GetOwner() const { return owner; }

    // Agregar instancia de clase abstracta
    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

private:
    bool m_debugDrawBox = false;
	bool m_overrideBoxSize = false;
    std::string m_text;
    glm::vec2 m_position;
    glm::vec3 m_color;
    float m_fontSize;
	float m_spaceWidth = 0.0f;
    glm::vec2 m_boxSize = glm::vec2(200.0f, 100.0f);
    glm::mat4 m_projection;
    glm::vec2 spriteSize = glm::vec2(0.0f, 0.0f);
    glm::vec2 sheetSize = glm::vec2(0.0f, 0.0f);
    glm::vec2 spriteOffset = glm::vec2(0.0f, 0.0f);

protected:

    friend class SceneSerializer;

    YAML::Node encode() override {

        YAML::Node node = Component::encode();

        node["text"] = m_text;
        node["position"] = std::vector<float>{ m_position.x, m_position.y };
        node["color"] = std::vector<float>{ m_color.r, m_color.g, m_color.b };
        node["fontSize"] = m_fontSize;
        node["boxSize"] = std::vector<float>{ m_boxSize.x, m_boxSize.y };
        node["spriteSize"] = std::vector<float>{ spriteSize.x, spriteSize.y };
        node["sheetSize"] = std::vector<float>{ sheetSize.x, sheetSize.y };
        node["spriteOffset"] = std::vector<float>{ spriteOffset.x, spriteOffset.y };

        return node;
    }

    bool decode(const YAML::Node& node) override {

        if (!Component::decode(node)) {
            return false;
        }

        m_text = node["text"].as<std::string>();
        m_position.x = node["position"][0].as<float>();
        m_position.y = node["position"][1].as<float>();
        m_color.r = node["color"][0].as<float>();
        m_color.g = node["color"][1].as<float>();
        m_color.b = node["color"][2].as<float>();
        m_fontSize = node["fontSize"].as<float>();
        m_boxSize.x = node["boxSize"][0].as<float>();
        m_boxSize.y = node["boxSize"][1].as<float>();
        spriteSize.x = node["spriteSize"][0].as<float>();
        spriteSize.y = node["spriteSize"][1].as<float>();
        sheetSize.x = node["sheetSize"][0].as<float>();
        sheetSize.y = node["sheetSize"][1].as<float>();
        spriteOffset.x = node["spriteOffset"][0].as<float>();
        spriteOffset.y = node["spriteOffset"][1].as<float>();

        return true;
    }
};