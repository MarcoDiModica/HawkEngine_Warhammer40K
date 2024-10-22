#pragma once
#include "Component.h"

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

private:
	std::string name;
	std::string tag = "Untagged";
	bool active = true;
	bool destroyed;

	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

	std::weak_ptr<GameObject> parent;
	std::vector<std::shared_ptr<GameObject>> children;
};


template <IsComponent T, typename... Args>
std::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
	std::shared_ptr<T> newComponent = std::make_shared<T>(weak_from_this(), std::forward<Args>(args)...);
	components[typeid(T)] = newComponent;
	return newComponent;
}

template <IsComponent T>
std::shared_ptr<T> GameObject::GetComponent() const {
	auto it = components.find(typeid(T));
	if (it != components.end()) {
		return std::static_pointer_cast<T>(it->second);
	}
	else {
		return nullptr;
	}
}

template <IsComponent T>
void GameObject::RemoveComponent() {
	auto it = components.find(typeid(T));
	if (it != components.end()) {
		components.erase(it);
	}
}

template <IsComponent T>
bool GameObject::HasComponent() const {
	return components.find(typeid(T)) != components.end();
}