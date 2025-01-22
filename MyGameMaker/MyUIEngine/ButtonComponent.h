#ifndef __BUTTON_COMPONENT_H__
#define __BUTTON_COMPONENT_H__
#pragma once

#include "UIComponent.h"
#include "MyGameEditor/UIElement.h"
#include "MyGameEngine/Gameobject.h"
#include "CanvasComponent.h"
#include <vector>
#include <memory>
#include <functional>


class ButtonComponent : public UIComponent {
public:
    ButtonComponent(GameObject* owner);
    ~ButtonComponent() override;

    void Start()const override;
    void Update(float deltaTime)const override;
    void Destroy()const override;

    CanvasComponent::UIComponentType GetType() const override { return CanvasComponent::UIComponentType::BUTTON; }

    void AddButton(std::shared_ptr<UIElement> button);
    void RemoveButton(std::shared_ptr<UIElement> button);
    void Render();
    void SetOnClick(std::function<void()> onClick);

private:
    std::vector<std::shared_ptr<UIElement>> buttons;
    std::function<void()> onClick;
};

#endif // !__BUTTON_COMPONENT_H__