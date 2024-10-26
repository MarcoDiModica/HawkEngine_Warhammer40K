#include "GameObject.h"
#include "MeshRendererComponent.h"
#include <iostream>

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)) 
{
    transform = AddComponent<Transform_Component>();
}

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

    /*if (HasComponent<MeshRenderer>())
	{
		auto meshRenderer = GetComponent<MeshRenderer>();
		if (meshRenderer)
		{
			meshRenderer->Render();
		}
        else
        {
			std::cout << "MeshRenderer is nullptr" << std::endl;
		}
	}
    else
    {
        std::cout << "No MeshRenderer" << std::endl;
    }*/

    transform->Translate(glm::vec3(0.01f, 0.0f, 0.0f));
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
    child->SetParent(shared_from_this());
}

void GameObject::RemoveChild(std::shared_ptr<GameObject> child) 
{
    auto iter = std::find(children.begin(), children.end(), child);
    if (iter != children.end()) {
        children.erase(iter);
        child->RemoveParent();
    }
}

void GameObject::SetParent(std::shared_ptr<GameObject> newParent) 
{
    if (auto oldParent = parent.lock()) {
        oldParent->RemoveChild(shared_from_this());
    }

    parent = newParent;

    if (newParent) {
        newParent->children.push_back(shared_from_this());
    }

    if (transform) {
        transform->SetParent(newParent ? newParent->transform : std::weak_ptr<Transform_Component>());
    }
}

void GameObject::RemoveParent() 
{
    if (auto oldParent = parent.lock()) {
        oldParent->RemoveChild(shared_from_this());
    }

    parent.reset();

    if (transform) {
        transform->SetParent({});
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