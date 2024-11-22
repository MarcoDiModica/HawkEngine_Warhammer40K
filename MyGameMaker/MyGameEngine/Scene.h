#pragma once
#include <memory>
#include <vector>
#include "TreeExt.h"
#include "GameObject.h"

class Scene
{
public:
	Scene(const std::string& name = "Scene") : name(name) {}
	~Scene() {}

	//void Awake();
	void Start();
	//void FixedUpdate(float fixedDeltaTime);
	void Update(float deltaTime);
	//void LateUpdate(float deltaTime);
	void Destroy();

	void OnEnable();
	void OnDisable();

	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);

	std::string GetName() const;
	void SetName(const std::string& name);

	auto children()const { return readOnlyVector<std::shared_ptr<GameObject>>(_children); }

private:
	std::string name;

	friend class Root;
	friend class SceneSerializer;

	std::vector<std::shared_ptr<GameObject>> _children;
};
