#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"
#include "readOnlyView.h"
#include "Octree.h"

class GameObject;

class Scene
{
public:
	Scene( const std::string& name = "Scene") : name(name)  {}
	~Scene() {}

	//void Awake();
	void Start();
	//void FixedUpdate(float fixedDeltaTime);
	void Update(float deltaTime);
	//void LateUpdate(float deltaTime);
	void Destroy();

	void OnEnable();
	void OnDisable();

	void DebugDrawTree();

	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);

	std::string GetName() const;
	void SetName(const std::string& name);

	auto children()const { return readOnlyVector<std::shared_ptr<GameObject>>(_children); }

	Octree* tree = nullptr; // change to unique

private:
	friend class GameObject;

	std::string name;

	friend class SceneManager;
	friend class Root;
	friend class SceneSerializer;

	std::vector<std::shared_ptr<GameObject>> _children;
};
