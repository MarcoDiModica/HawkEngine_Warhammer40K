#ifndef __CHECK_BOX_COMPONENT_H__
#define __CHECK_BOX_COMPONENT_H__
#pragma once

#include "Component.h"
#include "UIElement.h"
#include <vector>
#include <memory>
#include <functional>

class CheckBoxComponent : public Component {
public:
    CheckBoxComponent(GameObject* owner);
    ~CheckBoxComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::CHECK_BOX; }

    void AddCheckBox(std::shared_ptr<UIElement> checkBox);
    void RemoveCheckBox(std::shared_ptr<UIElement> checkBox);
    void Render();
    void SetOnCheckedChanged(std::function<void(bool)> onCheckedChanged);

private:
    std::vector<std::shared_ptr<UIElement>> checkBoxes;
    std::function<void(bool)> onCheckedChanged;
};

#endif // !__CHECK_BOX_COMPONENT_H__