#ifndef __BUTTON_COMPONENT_H__
#define __BUTTON_COMPONENT_H__
#pragma once

#include "Component.h"
#include "UIElement.h"
#include <vector>
#include <memory>
#include <functional>

class ButtonComponent : public Component {
public:
    ButtonComponent(GameObject* owner);
    ~ButtonComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::BUTTON; }

    void AddButton(std::shared_ptr<UIElement> button);
    void RemoveButton(std::shared_ptr<UIElement> button);
    void Render();
    void SetOnClick(std::function<void()> onClick);

private:
    std::vector<std::shared_ptr<UIElement>> buttons;
    std::function<void()> onClick;
};

#endif // !__BUTTON_COMPONENT_H__