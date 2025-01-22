#ifndef __CHECK_BOX_COMPONENT_H__
#define __CHECK_BOX_COMPONENT_H__
#pragma once

#include "UIComponent.h"
#include "MyGameEditor/UIElement.h"
#include "MyGameEngine/Gameobject.h"
#include "CanvasComponent.h"
#include <vector>
#include <memory>
#include <functional>

class CheckBoxComponent : public UIComponent {
public:
    CheckBoxComponent(GameObject* owner);
    ~CheckBoxComponent() override;

    void Start()const override;
    void Update(float deltaTime) const override;
    void Destroy() const override;

    CanvasComponent::UIComponentType GetType() const override { return CanvasComponent::UIComponentType::CHECKBOX; }

    void AddCheckBox(std::shared_ptr<UIElement> checkBox);
    void RemoveCheckBox(std::shared_ptr<UIElement> checkBox);
    void Render();
    void SetOnCheckedChanged(std::function<void(bool)> onCheckedChanged);

private:
    std::vector<std::shared_ptr<UIElement>> checkBoxes;
    std::function<void(bool)> onCheckedChanged;
};

#endif // !__CHECK_BOX_COMPONENT_H__