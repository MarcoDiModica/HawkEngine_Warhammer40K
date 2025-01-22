#ifndef __INPUT_BOX_COMPONENT_H__
#define __INPUT_BOX_COMPONENT_H__
#pragma once

#include "MyGameEditor/UIElement.h"
#include "MyGameEngine/Gameobject.h"
#include "CanvasComponent.h"
#include "UIComponent.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>

class InputBoxComponent : public UIComponent {
public:
    InputBoxComponent(GameObject* owner);
    ~InputBoxComponent() override;

    void Start() const override;
    void Update(float deltaTime)const override;
    void Destroy()const override;

    CanvasComponent::UIComponentType GetType() const override { return CanvasComponent::UIComponentType::INPUTBOX; }

    void AddInputBox(std::shared_ptr<UIElement> inputBox);
    void RemoveInputBox(std::shared_ptr<UIElement> inputBox);
    void Render();
    void SetOnTextChanged(std::function<void(const std::string&)> onTextChanged);

private:
    std::vector<std::shared_ptr<UIElement>> inputBoxes;
    std::function<void(const std::string&)> onTextChanged;
};

#endif // !__INPUT_BOX_COMPONENT_H__