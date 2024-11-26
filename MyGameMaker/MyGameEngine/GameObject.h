#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "TreeExt.h"
#include "Mesh.h"
#include "BoundingBox.h"

class SceneSerializer;

enum class DrawMode
{
    AccumultedMatrix,
    InstancedMatrix,
    PushPopMatrix
};

class GameObject : public std::enable_shared_from_this<GameObject>, public TreeExt<GameObject>
{
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other);

    template <IsComponent T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);

    template <IsComponent T>
    std::shared_ptr<T> GetComponent() const;

    template <IsComponent T>
    void RemoveComponent();

    template <IsComponent T>
    bool HasComponent() const;

    //void Awake();
    void Start();
    //void FixedUpdate(float fixedDeltaTime);
    void Update(float deltaTime);
    //void LateUpdate(float deltaTime);
    void Draw() const;
    void Destroy();

    void OnEnable();
    void OnDisable();

    void SetActive(bool active);
    bool IsActive() const;

    std::string GetName() const;
    void SetName(const std::string& name);

    bool CompareTag(const std::string& tag) const;

    BoundingBox boundingBox() const;
    BoundingBox localBoundingBox() const { return mesh ? mesh->boundingBox() : BoundingBox(); }

    std::shared_ptr<Transform_Component> GetTransform() const { return transform; }

    DrawMode drawMode = DrawMode::PushPopMatrix;

    unsigned int GetId() const { return gid; }

    bool operator==(const GameObject& other) const {
        return gid == other.gid;
    }

    bool operator!=(const GameObject& other) const {
        return !(*this == other);
    }

    bool isSelected = false;
    bool isStatic = false;

private:
    friend class SceneSerializer;

    void DrawAccumultedMatrix() const;
    void DrawInstancedMatrix() const;
    void DrawPushPopMatrix() const;

    std::string name;
    unsigned int gid;
    static unsigned int nextGid;
    std::string tag = "Untagged";
    bool active = true;
    bool destroyed = false;

    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    mutable std::type_index cachedComponentType;
    mutable std::shared_ptr<Component> cachedComponent;

    std::shared_ptr<Transform_Component> transform;
    std::shared_ptr<Mesh> mesh;
};


template <IsComponent T, typename... Args>
std::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
    std::shared_ptr<T> newComponent = std::make_shared<T>(this, std::forward<Args>(args)...);
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
