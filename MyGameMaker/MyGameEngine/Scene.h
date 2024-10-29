#pragma once

#include "GameObject.h"

class Scene
{
public:

	Scene();
	~Scene();

	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);
	void Start();
	void Update(float deltaTime);
	void Draw() const;
private:
	std::vector<std::shared_ptr<GameObject>> gameObjectsInScene;
};

