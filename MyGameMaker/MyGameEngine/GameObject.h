#pragma once
#include "Component.h"

class GameObject
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
	void Render();
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

	//Componentes
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

	//Jerarquia
	std::weak_ptr<GameObject> parent;
	std::vector<std::shared_ptr<GameObject>> children;
};

