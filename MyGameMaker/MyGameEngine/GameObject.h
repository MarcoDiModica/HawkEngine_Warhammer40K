#pragma once
#include "Component.h"
#include "TransformComponent.h"

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    template <IsComponent T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);

    template <IsComponent T>
    std::shared_ptr<T> GetComponent() const;

    template <IsComponent T>
    void RemoveComponent();

    template <IsComponent T>
    bool HasComponent() const;

    void AddChild(std::shared_ptr<GameObject> child);
    void RemoveChild(std::shared_ptr<GameObject> child);
    std::shared_ptr<GameObject> GetParent() const;
    void SetParent(std::shared_ptr<GameObject> newParent);
    void RemoveParent();
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const;

    void Start();
    void Update(float deltaTime);
    void Destroy();

    void OnEnable();
    void OnDisable();

    void SetActive(bool active);
    bool IsActive() const;

    std::string GetName() const;
    void SetName(const std::string& name);

    std::shared_ptr<Transform_Component> GetTransform() const { return transform; }

private:
    std::string name;
    std::string tag = "Untagged";
    bool active = true;
    bool destroyed = false;

    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    mutable std::type_index cachedComponentType;
    mutable std::shared_ptr<Component> cachedComponent;

    std::weak_ptr<GameObject> parent;
public:
    // marco no me mates hago esto publico de momento
    std::vector<std::shared_ptr<GameObject>> children;
    std::shared_ptr<Transform_Component> transform;
};


//las funciones plantilla se definen donde se declaran porque el compilador lo necesita para generar el codigo especifico para que cada type con el que se usa

template <IsComponent T, typename... Args>
std::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
    std::shared_ptr<T> newComponent = std::make_shared<T>(weak_from_this(), std::forward<Args>(args)...);
    components[typeid(T)] = newComponent;
    return newComponent;
}

template <IsComponent T>
std::shared_ptr<T> GameObject::GetComponent() const {
    if (cachedComponentType == typeid(T) && cachedComponent) {
        return std::static_pointer_cast<T>(cachedComponent);
    }

    auto it = components.find(typeid(T));
    if (it != components.end()) {
        cachedComponentType = typeid(T);
        cachedComponent = it->second;
        return std::static_pointer_cast<T>(cachedComponent);
    }
    else {
        throw std::runtime_error("Component not found on GameObject: " + this->GetName());
    }
}

//en un futuro si el componente esta siendo usado no dejar que se elimine
template <IsComponent T>
void GameObject::RemoveComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        components.erase(it);
    }
    else {
        //Log a warning or something idk asdsa
    }
}

template <IsComponent T>
bool GameObject::HasComponent() const {
    return components.find(typeid(T)) != components.end();
}