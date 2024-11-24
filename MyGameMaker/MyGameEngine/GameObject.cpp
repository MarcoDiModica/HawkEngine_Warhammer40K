#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "../MyGameEditor/App.h"
#include <iostream>

unsigned int GameObject::nextGid = 1;

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)), gid(nextGid++)
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
}

GameObject::GameObject(const GameObject& other) :
    name(other.name),
    gid(nextGid++),
    active(other.active),
    transform(std::make_shared<Transform_Component>(*other.transform)),
    mesh(other.mesh),
    tag(other.tag),
    cachedComponentType(typeid(Component))
{
    for (const auto& component : other.components) {
        components[component.first] = component.second->Clone();
    }
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other)
    {
        name = other.name;
        gid = nextGid++;
        active = other.active;
        transform = std::make_shared<Transform_Component>(*other.transform);
        mesh = other.mesh;
        tag = other.tag;

        components.clear();

        for (const auto& component : other.components)
        {
            components[component.first] = component.second->Clone();
        }

        for (auto& child : children)
		{
			child->Destroy();
		}
    }
    return *this;
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

    Draw();
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
}

void GameObject::Draw() const
{
    //std::cout << "Draw GameObject: " << name << std::endl;
    
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

    if (HasComponent<MeshRenderer>())
    {
        auto meshRenderer = GetComponent<MeshRenderer>();
        meshRenderer->Render();
    }

    for (const auto& child : children)
	{
		child->Draw();
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
    if (!mesh && children.size()) bbox = children.front()->boundingBox();
    for (const auto& child : children) bbox = bbox + child->boundingBox();
    return transform->GetMatrix() * bbox;
}

void GameObject::SetParent(std::shared_ptr<GameObject> parent) {
    // Si ya tiene un padre, se elimina como hijo del padre anterior
    if (auto currentParent = this->parent.lock()) {
        currentParent->RemoveChild(shared_from_this());
    }

    // Asigna el nuevo padre
    this->parent = parent;

    // Si el nuevo padre es válido, se añade como hijo
    if (parent) {
        parent->AddChild(shared_from_this());
    }
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
    // Se asegura que el hijo no sea nulo y no sea el propio objeto
    if (child && child != shared_from_this()) {
        // Se añade el hijo a la lista de hijos
        children.push_back(child);
    }
}

void GameObject::RemoveChild(std::shared_ptr<GameObject> child) {
    // Se busca al hijo en la lista de hijos
    auto it = std::find(children.begin(), children.end(), child);

    // Si se encuentra al hijo, se elimina de la lista
    if (it != children.end()) {
        children.erase(it);
    }
}

std::shared_ptr<GameObject> GameObject::GetParent() const {
    // Intenta bloquear el weak_ptr para obtener un shared_ptr al padre
    return parent.lock();
}

const std::vector<std::shared_ptr<GameObject>>& GameObject::GetChildren() const {
    // Devuelve la lista de hijos
    return children;
}