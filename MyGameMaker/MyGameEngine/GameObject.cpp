#include "GameObject.h"
#include "MeshRendererComponent.h"

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)) {}

GameObject::~GameObject()
{
    for (auto& component : components) {
        component.second->Destroy();
    }
    components.clear();

    for (auto& child : children) {
        child->Destroy();
    }
    children.clear();
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
    
    for (auto& component : components)
	{
		component.second->Destroy();
	}

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