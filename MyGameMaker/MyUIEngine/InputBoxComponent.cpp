#include "InputBoxComponent.h"
#include "GameObject.h"
#include <algorithm>

InputBoxComponent::InputBoxComponent(GameObject* owner) : Component(owner) {
    // Initialization code here
}

InputBoxComponent::~InputBoxComponent() {
    // Cleanup code here
}

void InputBoxComponent::Start() {
    // Start code here
}

void InputBoxComponent::Update(float deltaTime) {
    for (const auto& inputBox : inputBoxes) {
        inputBox->Update(deltaTime);
    }
}

void InputBoxComponent::Destroy() {
    // Destroy code here
}

void InputBoxComponent::AddInputBox(std::shared_ptr<UIElement> inputBox) {
    inputBoxes.push_back(inputBox);
}

void InputBoxComponent::RemoveInputBox(std::shared_ptr<UIElement> inputBox) {
    inputBoxes.erase(std::remove(inputBoxes.begin(), inputBoxes.end(), inputBox), inputBoxes.end());
}

void InputBoxComponent::Render() {
    for (const auto& inputBox : inputBoxes) {
        if (inputBox->IsEnabled()) {
            inputBox->Draw();
        }
    }
}

void InputBoxComponent::SetOnTextChanged(std::function<void(const std::string&)> onTextChanged) {
    this->onTextChanged = onTextChanged;
}