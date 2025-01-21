#ifndef __INPUT_BOX_COMPONENT_H__
#define __INPUT_BOX_COMPONENT_H__
#pragma once

#include "Component.h"
#include "UIElement.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>

class InputBoxComponent : public Component {
public:
    InputBoxComponent(GameObject* owner);
    ~InputBoxComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::INPUT_BOX; }

    void AddInputBox(std::shared_ptr<UIElement> inputBox);
    void RemoveInputBox(std::shared_ptr<UIElement> inputBox);
    void Render();
    void SetOnTextChanged(std::function<void(const std::string&)> onTextChanged);

private:
    std::vector<std::shared_ptr<UIElement>> inputBoxes;
    std::function<void(const std::string&)> onTextChanged;
};

#endif // !__INPUT_BOX_COMPONENT_H__