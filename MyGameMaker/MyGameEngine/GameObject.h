#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "TreeExt.h"
#include "Mesh.h"
#include "BoundingBox.h"
#include "../MyScriptingEngine/EngineBinds.h"
#include <mono/metadata/object.h>
#include "../MyScriptingEngine/ScriptComponent.h"



class SceneSerializer;
class Scene;
struct OctreeNode;
class Shaders;

enum class DrawMode
{
    AccumultedMatrix,
    InstancedMatrix,
    PushPopMatrix
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other);

    GameObject(GameObject&& other) noexcept;
    GameObject& operator=(GameObject&& other) noexcept;

    template <IsComponent T, typename... Args>
    T* AddComponent(Args&&... args);

    template <typename T>
    T* GetComponent() const;

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
    
	void ShaderUniforms(glm::dmat4 view, glm::dmat4 projection, glm::dvec3 cameraPosition, std::list<GameObject*> lights, Shaders useShader);

    std::string GetName() const;
    void SetName(const std::string& name);

    bool CompareTag(const std::string& tag) const;

    BoundingBox boundingBox() const;
    BoundingBox localBoundingBox() const { return mesh ? mesh->boundingBox() : BoundingBox(); }

    Transform_Component* GetTransform() const { return GetComponent<Transform_Component>(); }

    DrawMode drawMode = DrawMode::PushPopMatrix;

    unsigned int GetId() const { return gid; }

    void SetParent(GameObject* parent);
    GameObject* GetParent() const { return parent; }
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return children; }

    bool operator==(const GameObject& other) const {
        return gid == other.gid;
    }

    bool operator!=(const GameObject& other) const {
        return !(*this == other);
    }

    Scene* GetScene() const { return scene; }

    bool isSelected = false;
    bool isStatic = false;

    OctreeNode* node = nullptr;

    MonoObject* GetSharp();

    MonoObject* CsharpReference = nullptr;

    std::vector<std::unique_ptr<ScriptComponent>> scriptComponents;

    void setBoundingBox(const BoundingBox& bbox) { _boundingBox = bbox; }

private:
    friend class SceneSerializer;
    friend class GameObject;

    void DrawAccumultedMatrix() const;
    void DrawInstancedMatrix() const;
    void DrawPushPopMatrix() const;

    std::string name;
    unsigned int gid;
    BoundingBox _boundingBox;
private:
    
    static unsigned int nextGid;
    std::string tag = "Untagged";
    bool active = true;
    bool destroyed = false;

    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

    mutable std::type_index cachedComponentType;
    mutable std::shared_ptr<Component> cachedComponent;

    //Transform_Component* transform;
    std::shared_ptr<Mesh> mesh;

    float timeActive = 0.0f;

protected:
    friend class Scene;

    Scene* scene = nullptr;
    GameObject* parent = nullptr;
    std::vector<std::shared_ptr<GameObject>> children;
};


template <IsComponent T, typename... Args>
T* GameObject::AddComponent(Args&&... args) {
    if constexpr (std::is_same_v<T, ScriptComponent>) {
        std::unique_ptr<T> newComponent = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* result = newComponent.get();
        scriptComponents.push_back(std::move(newComponent));
        return result;
    }

    std::unique_ptr<T> newComponent = std::make_unique<T>(this, std::forward<Args>(args)...);

    T* result = newComponent.get();

    components[typeid(T)] = std::move(newComponent);

    if (result != nullptr) {
        return result;
    }
    else {
        std::string errorMsg = "Error on AddComponent(): " + this->GetName() + " - Failed to create component of type " + typeid(T).name();
        throw std::runtime_error(errorMsg);
    }
}

template <typename T>
T* GameObject::GetComponent() const {
    for (const auto& scriptComponent : scriptComponents) {
        if (dynamic_cast<T*>(scriptComponent.get()) != nullptr) {
            return dynamic_cast<T*>(scriptComponent.get());
        }
    }

    auto it = components.find(typeid(T));
    if (it != components.end()) {
        return dynamic_cast<T*>(it->second.get());
    }

    return nullptr;
}

template <IsComponent T>
void GameObject::RemoveComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        components.erase(it);
    }
    else {
        throw std::runtime_error("Error on RemoveComponent(): " + this->GetName());
    }
}

template <IsComponent T>
bool GameObject::HasComponent() const {
    return components.find(typeid(T)) != components.end();
}