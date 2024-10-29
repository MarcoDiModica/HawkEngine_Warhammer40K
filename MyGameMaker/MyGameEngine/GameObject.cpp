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

    for (auto& child : children()) {
        child.Destroy();
    }
}

void GameObject::Start()
{
    for (auto& component : components)
    {
        component.second->Start();
    }

    for (auto& child : children())
    {
        child.Start();
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
    
    for (auto& child : children())
	{
		child.Update(deltaTime);
	}

    Draw();
}

void GameObject::Destroy()
{
    destroyed = true;
    
    for (auto& component : components)
	{
		component.second->Destroy();
	}

    for (auto& child : children())
    {
        child.Destroy();
    }
}

void GameObject::Draw() const
{
    switch (drawMode)
    {
    case DrawMode::AccumultedMatrix:
        DrawAccumultedMatrix();
        break;
    case DrawMode::InstancedMatrix:
        DrawInstancedMatrix();
        break;
    case DrawMode::PushPopMatrix:
        DrawPushPopMatrix();
        break;
    }
}

void GameObject::DrawAccumultedMatrix() const
{
    //De momento nada ya lo hare en un futuro :)
}

void GameObject::DrawInstancedMatrix() const
{
	//De momento nada ya lo hare en un futuro :)
}

void GameObject::DrawPushPopMatrix() const
{
    glPushMatrix();
    glMultMatrixd(transform->GetData());

    if (auto meshRenderer = GetComponent<MeshRenderer>())
    {
        meshRenderer->Render();
    }

    for (const auto& child : children())
    {
        child.Draw();
    }

    glPopMatrix();
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

bool GameObject::CompareTag(const std::string& tag) const
{
	return this->tag == tag;
}

BoundingBox GameObject::boundingBox() const 
{
    BoundingBox bbox = localBoundingBox();
    if (!mesh && children().size()) bbox = children().front().boundingBox();
    for (const auto& child : children()) bbox = bbox + child.boundingBox();
    return transform->GetMatrix() * bbox;
}