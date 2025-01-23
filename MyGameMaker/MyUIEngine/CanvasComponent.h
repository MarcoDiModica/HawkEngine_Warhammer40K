#ifndef __CANVAS_COMPONENT_H__
#define __CANVAS_COMPONENT_H__
#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEditor/UIElement.h"
#include <vector>
#include <memory>

class CanvasComponent : public Component {
public:
    enum class UIComponentType {
        BUTTON,
        CHECKBOX,
        INPUTBOX,
        IMAGE,
        UNKNOWN
    };

    CanvasComponent(GameObject* owner);
    ~CanvasComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    std::unique_ptr<Component> Clone(GameObject* owner) override;

    ComponentType GetType() const override { return ComponentType::CANVAS; }

    void AddElement(std::shared_ptr<UIElement> element);
    void RemoveElement(std::shared_ptr<UIElement> element);
    void Render();

private:
    std::vector<std::shared_ptr<UIElement>> elements;
};

#endif // !__CANVAS_COMPONENT_H__
