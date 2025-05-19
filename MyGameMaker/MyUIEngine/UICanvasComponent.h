#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/GameObject.h"
#include <yaml-cpp/yaml.h>

class UICanvasComponent : public Component
{
public:
	explicit UICanvasComponent(GameObject* owner);
	~UICanvasComponent() override = default;

	UICanvasComponent(const UICanvasComponent&) = delete;
	UICanvasComponent& operator=(const UICanvasComponent&) = delete;

	UICanvasComponent(UICanvasComponent&&) noexcept = default;
	UICanvasComponent& operator=(UICanvasComponent&&) noexcept = default;

	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::CANVAS; }

	void RenderCanvas(int viewportWidth, int viewportHeight);

	float GetMonitorWidth() const { return monitorWidth; }
	float GetMonitorHeight() const { return monitorHeight; }

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

private:
	float monitorWidth;
	float monitorHeight;

	void SetupUIRendering(int viewportWidth, int viewportHeight);
	void RestoreRenderingState();
	void RenderUIElement(GameObject* element, const glm::mat4& projection, const glm::mat4& view);

	GLint lastProgram;
	GLboolean lastDepthTest;
	GLboolean lastCullFace;
	GLboolean lastBlend;
	GLint lastBlendSrcRGB;
	GLint lastBlendDstRGB;
	GLint lastBlendSrcAlpha;
	GLint lastBlendDstAlpha;

protected:
	friend class SceneSerializer;

	YAML::Node encode() override {
		YAML::Node node = Component::encode();
		return node;
	}

	bool decode(const YAML::Node& node) override {
		Component::decode(node);
		return true;
	}
};
