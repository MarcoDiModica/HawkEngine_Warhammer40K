#include "CanvasComponent.h"
#include "GameObject.h"
#include <algorithm>

CanvasComponent::CanvasComponent(GameObject* owner) : Component(owner) {
    // Initialization code here
}

CanvasComponent::~CanvasComponent() {
    // Cleanup code here
}

void CanvasComponent::Start() {
    // Start code here
}

void CanvasComponent::Update(float deltaTime) {
    for (const auto& element : elements) {
        element->Update(deltaTime);
    }
}

void CanvasComponent::Destroy() {
    // Destroy code here
}

void CanvasComponent::AddElement(std::shared_ptr<UIElement> element) {
    elements.push_back(element);
}

void CanvasComponent::RemoveElement(std::shared_ptr<UIElement> element) {
    elements.erase(std::remove(elements.begin(), elements.end(), element), elements.end());
}

void CanvasComponent::Render() {
    for (const auto& element : elements) {
        if (element->IsEnabled()) {
            element->Draw();
        }
    }
}
