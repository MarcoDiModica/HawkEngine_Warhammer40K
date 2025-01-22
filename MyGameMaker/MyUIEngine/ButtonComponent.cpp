#include "ButtonComponent.h"

ButtonComponent::ButtonComponent(GameObject* owner)
    : UIComponent("ButtonComponent"), onClick(nullptr) {
    // Constructor implementation
}

ButtonComponent::~ButtonComponent() {
    // Destructor implementation
}

void ButtonComponent::Start()const {
    // Start implementation
}

void ButtonComponent::Update(float deltaTime)const {
    // Update implementation
}

void ButtonComponent::Destroy()const {
    // Destroy implementation
}

void ButtonComponent::AddButton(std::shared_ptr<UIElement> button) {
    buttons.push_back(button);
}

void ButtonComponent::RemoveButton(std::shared_ptr<UIElement> button) {
    buttons.erase(std::remove(buttons.begin(), buttons.end(), button), buttons.end());
}

void ButtonComponent::Render() {
    for (const auto& button : buttons) {
        button->Draw();
    }
}

void ButtonComponent::SetOnClick(std::function<void()> onClick) {
    this->onClick = onClick;
}