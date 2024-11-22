#include "Scene.h"

#include "MeshRendererComponent.h"
#include "Mesh.h"

void Scene::Start()
{
	for (auto& child : children())
	{
		child->Start();
	}
}

void Scene::Update(float deltaTime)
{
	for (auto& child : children())
	{
		child->Update(deltaTime);
	}
}

void Scene::Destroy()
{
	for (auto& child : children())
	{
		child->Destroy();
	}
}

void Scene::OnEnable()
{
	for (auto& child : children())
	{
		child->OnEnable();
	}
}

void Scene::OnDisable()
{
	for (auto& child : children())
	{
		child->OnDisable();
	}
}

void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject)
{
	_children.erase(std::remove(_children.begin(), _children.end(), gameObject), _children.end());
}

void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	_children.push_back(gameObject);
}

std::string Scene::GetName() const
{
	return name;
}

void Scene::SetName(const std::string& name)
{
	this->name = name;
}