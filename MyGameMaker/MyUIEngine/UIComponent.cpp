#include "UIComponent.h"

UIComponent::UIComponent() : name("Unnamed"), checked(false) {}

UIComponent::UIComponent(const std::string& name) : name(name), checked(false) {}

UIComponent::~UIComponent() {}

std::string UIComponent::getName() const {
    return name;
}

void UIComponent::setName(const std::string& name) {
    this->name = name;
}

void UIComponent::Start() const {
    
}

void UIComponent::Update(float deltaTime) const {
    
}

void UIComponent::Destroy() const {
    
}

// Implementación de los métodos específicos para CheckBoxComponent
bool UIComponent::isChecked() const {
    return checked;
}

void UIComponent::setChecked(bool checked) {
    this->checked = checked;
}

void UIComponent::toggle() {
    checked = !checked;
}

// Implementación de los métodos específicos para ButtonComponent
void UIComponent::onClick() const {
    if (onClickHandler) {
        onClickHandler();
    }
}

void UIComponent::setOnClickHandler(const std::function<void()>& handler) {
    onClickHandler = handler;
}


// Implementación de los métodos específicos para InputBoxComponent
std::string UIComponent::getText() const {
    return text;
}

void UIComponent::setText(const std::string& text) {
    this->text = text;
    if (onTextChangedHandler) {
        onTextChangedHandler(text);
    }
}

void UIComponent::onTextChanged() const {
    if (onTextChangedHandler) {
        onTextChangedHandler(text);
    }
}

void UIComponent::setOnTextChangedHandler(const std::function<void(const std::string&)>& handler) {
    onTextChangedHandler = handler;
}

// Implementación de los métodos específicos para ImageComponent
std::string UIComponent::getImagePath() const {
    return imagePath;
}

void UIComponent::setImagePath(const std::string& path) {
    this->imagePath = path;
    if (onImageChangedHandler) {
        onImageChangedHandler(path);
    }
}

void UIComponent::onImageChanged() const {
    if (onImageChangedHandler) {
        onImageChangedHandler(imagePath);
    }
}

void UIComponent::setOnImageChangedHandler(const std::function<void(const std::string&)>& handler) {
    onImageChangedHandler = handler;
}