#include "CheckBoxComponent.h"

CheckBoxComponent::CheckBoxComponent(GameObject* owner)
    : Component(owner), isChecked(false) {
}

CheckBoxComponent::~CheckBoxComponent() {}

void CheckBoxComponent::Start() {
    // Inicializar recursos o estado de la checkbox
}

void CheckBoxComponent::Update(float deltaTime) {
    // Actualizar lógica, como detectar clics
}

void CheckBoxComponent::Destroy() {
    // Liberar recursos si es necesario
}

std::unique_ptr<Component> CheckBoxComponent::Clone(GameObject* owner) {
    auto clone = std::make_unique<CheckBoxComponent>(owner);
    clone->SetChecked(isChecked);
    clone->SetOnCheckedChanged(onCheckedChanged);
    return clone;
}

void CheckBoxComponent::SetChecked(bool checked) {
    if (isChecked != checked) {
        isChecked = checked;
        if (onCheckedChanged) {
            onCheckedChanged(isChecked);
        }
    }
}

bool CheckBoxComponent::IsChecked() const {
    return isChecked;
}

void CheckBoxComponent::SetOnCheckedChanged(std::function<void(bool)> callback) {
    onCheckedChanged = std::move(callback);
}

void CheckBoxComponent::Render() {
    // Aquí podrías implementar el renderizado de la checkbox
    // usando la lógica que se adapte a tu motor de juego.
}
