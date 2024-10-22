#include "GameObject.h"
#include "Component.h"

GameObject::GameObject(const std::string& name)
	: name(name), tag("Untagged"), active(true), destroyed(false)
{
}

GameObject::~GameObject()
{
	//logica de destroy, por ejemplo, llamar a la funcion destroy de todos los componentes
	// y llamar a la funcion destroy de todos los hijos que a la vez estos hijos
}

void GameObject::Start()
{
	for (auto& component : components)
	{
		component.second->Start();
	}

	for (auto& child : children)
	{
		child->Start();
	}
}

void GameObject::Update(float deltaTime)
{
	if (!active)
	{
		return;
	}
	
	for (auto& component : components)
	{
		component.second->Update(deltaTime);
	}

	for (auto& child : children)
	{
		child->Update(deltaTime);
	}
}

void GameObject::Destroy()
{
	destroyed = true;
	//logica para destruir

	for (auto& child : children)
	{
		child->Destroy();
	}
	children.clear();
}

void GameObject::OnEnable() {}

void GameObject::OnDisable() {}

void GameObject::SetActive(bool active)
{
	this->active = active;

	if (active)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}

bool GameObject::IsActive() const
{
	return active;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& name)
{
	this->name = name;
}

void GameObject::AddChild(std::shared_ptr<GameObject> child)
{
	child->parent = weak_from_this();
	children.push_back(child);
}

void GameObject::RemoveChild(std::shared_ptr<GameObject> child)
{
	auto iter = std::find(children.begin(), children.end(), child);
	if (iter != children.end())
	{
		children.erase(iter);
		child->parent.reset();
	}
}

std::shared_ptr<GameObject> GameObject::GetParent() const
{
	return parent.lock();
}

const std::vector<std::shared_ptr<GameObject>>& GameObject::GetChildren() const
{
	return children;
}