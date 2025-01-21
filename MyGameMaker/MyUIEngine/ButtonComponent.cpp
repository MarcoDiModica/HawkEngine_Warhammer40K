#include "ButtonComponent.h"
#include "GameObject.h"
#include <algorithm>

ButtonComponent::ButtonComponent(GameObject* owner) : Component(owner) {
    // Initialization code here
}

ButtonComponent::~ButtonComponent() {
    // Cleanup code here
}

void ButtonComponent::Start() {
    // Start code here
}

void ButtonComponent::Update(float deltaTime) {
    for (const auto& button : buttons) {
        button->Update(deltaTime);
    }
}

void ButtonComponent::Destroy() {
    // Destroy code here
}

void ButtonComponent::AddButton(std::shared_ptr<UIElement> button) {
    buttons.push_back(button);
}

void ButtonComponent::RemoveButton(std::shared_ptr<UIElement> button) {
    buttons.erase(std::remove(buttons.begin(), buttons.end(), button), buttons.end());
}

void ButtonComponent::Render() {
    for (const auto& button : buttons) {
        if (button->IsEnabled()) {
            button->Draw();
        }
    }
}

void ButtonComponent::SetOnClick(std::function<void()> onClick) {
    this->onClick = onClick;
}