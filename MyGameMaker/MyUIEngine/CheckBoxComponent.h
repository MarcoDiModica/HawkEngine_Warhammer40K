#ifndef __CHECK_BOX_COMPONENT_H__
#define __CHECK_BOX_COMPONENT_H__
#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEditor/UIElement.h"
#include "MyGameEngine/GameObject.h"
#include <vector>
#include <memory>
#include <functional>

class CheckBoxComponent : public Component {
public:
    // Constructor y destructor
    CheckBoxComponent(GameObject* owner);
    ~CheckBoxComponent() override;

    // Métodos del ciclo de vida del componente
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    // Método para clonar el componente
    std::unique_ptr<Component> Clone(GameObject* owner) override;

    // Devuelve el tipo del componente
    ComponentType GetType() const override { return ComponentType::CHECKBOX; }

    // Gestión de la checkbox
    void SetChecked(bool checked);
    bool IsChecked() const;

    // Eventos
    void SetOnCheckedChanged(std::function<void(bool)> onCheckedChanged);

    // Renderizado
    void Render();

private:
    bool isChecked;  // Estado de la checkbox
    std::function<void(bool)> onCheckedChanged;  // Callback al cambiar el estado
};

#endif // !__CHECK_BOX_COMPONENT_H__
