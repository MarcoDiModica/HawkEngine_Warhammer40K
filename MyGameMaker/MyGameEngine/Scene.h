#pragma once
#include <memory>
#include <vector>
#include "TreeExt.h"
#include "GameObject.h"

class Scene : public TreeExt<GameObject>
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

	std::shared_ptr<GameObject> FindGameObjectbyName(const std::string& name);
	std::shared_ptr<GameObject> FindGameObjectbyTag(const std::string& tag);

	std::string GetName() const;
	void SetName(const std::string& name);

private:
	std::string name;
};
