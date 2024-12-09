#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Log.h"
#include <iostream>

unsigned int GameObject::nextGid = 1;

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)), gid(nextGid++)
{
    AddComponent<Transform_Component>();
    transform = GetComponent<Transform_Component>();
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
    transform(new Transform_Component(this)),
    mesh(other.mesh),
    tag(other.tag),
    cachedComponentType(typeid(Component)),
    parent(nullptr)
{   
    for (const auto& component : other.components) {
        components[component.first] = component.second->Clone(this);
        components[component.first]->owner = this;
    }

    for (const auto& child : other.children) {
        auto newChild = std::make_shared<GameObject>(*child);
        newChild->parent = this;
        children.emplace_back(std::move(newChild));
	}
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other)
    {
        name = other.name;
        gid = nextGid++;
        active = other.active;
        transform = other.transform;
        mesh = other.mesh;
        tag = other.tag;
        parent = nullptr;

        components.clear();
        for (const auto& component : other.components)
        {
            components[component.first] = component.second->Clone(this);
        }

        for (auto& child : children)
        {
            child->parent = nullptr;
            child->Destroy();
        }
        children.clear();

        for (const auto& child : other.children)
		{
			auto newChild = std::make_shared<GameObject>(*child);
			newChild->parent = this;
			children.emplace_back(std::move(newChild));
		}
    }
    return *this;
}

GameObject::GameObject(GameObject&& other) noexcept :
	name(std::move(other.name)),
	gid(nextGid++),
	active(other.active),
	transform(std::move(other.transform)),
	mesh(std::move(other.mesh)),
	tag(std::move(other.tag)),
	components(std::move(other.components)),
    children(std::move(other.children)),
    parent(other.parent),
	cachedComponentType(typeid(Component))
{
    for (auto& child : children) {
        child->parent = this;
    }
    
    for (auto& component : components)
	{
		component.second->owner = this;
	}

    other.parent = nullptr;
    other.children.clear();
}

GameObject& GameObject::operator=(GameObject&& other) noexcept
{
	if (this != &other)
	{
		name = std::move(other.name);
		gid = nextGid++;
		active = other.active;
		transform = std::move(other.transform);
		mesh = std::move(other.mesh);
		tag = std::move(other.tag);
		components = std::move(other.components);
        children = std::move(other.children);
        parent = other.parent;
		cachedComponentType = typeid(Component);

        for (auto& child : children) {
            child->parent = this;
        }
        
        for (auto& component : components)
		{
			component.second->owner = this;
		}

        other.parent = nullptr;
        other.children.clear();
	}
	return *this;
}

void GameObject::Start()
{
    for (auto& component : components)
    {
        component.second->Start();
    }

    if (GetName() != "Cube_3") {
        scene->tree->Insert(scene->tree->root, *this, 0);
    }

    for (auto& child : children)
    {
        child->Start();
    }
}

void GameObject::Update(float deltaTime)
{
    //display();
    if (!active)
    {
        return;
    }

    //LOG(LogType::LOG_INFO, "GameObject::Update", "GameObject: " + name + " - Update");

    //std::cout << std::endl << GetName() << "has " << children().size() << " children";

    //check the state of the components and throw an error if they are null
    for (auto& component : components)
	{
		if (!component.second)
		{
			throw std::runtime_error("Component is null");
		}
	}

    //log size of components
    
    for (auto& component : components)
	{
		component.second->Update(deltaTime);
        //LOG(LogType::LOG_INFO, "GameObject::Update", "GameObject: " + name + " - Update");
	}

    for (auto& child : children)
    {
        child->Update(deltaTime);
    }

    Draw();
}

void GameObject::Destroy()
{
    if (!this) {
        return;
    }

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

    for (const auto& child : children)
    {
        child->Draw();
    }

    //glLoadIdentity(); // Resets the current matrix to identity
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

  //  glMultMatrixd(&glm::dmat4(1.0)[0][0]);

    glPopMatrix();
    //glLoadIdentity();
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

void GameObject::SetParent(GameObject* parent)
{
    if (this->parent == parent)
	{
		return;
	}

    if (this->parent)
	{
		this->parent->RemoveChild(this);
	}

	this->parent = parent;
    if (parent)
    {
        parent->AddChild(this);
	}
}

void GameObject::AddChild(GameObject* child)
{
    if (child->GetParent() == nullptr) {
        if (child->scene) {
            
            for (size_t i = 0; i < child->scene->_children.size(); ++i) {

                if (*child->scene->_children[i] == *child) {

                    children.push_back(child->shared_from_this());
                    children[children.size() - 1]->parent = this;
                    children[children.size() - 1]->GetTransform()->UpdateLocalMatrix(this->GetTransform()->GetMatrix());

                    child->scene->_children.erase(child->scene->_children.begin() + i);
                    return;
                }
            }
        }
    }
    else {

        GameObject* prev_father = child->GetParent();
        children.push_back(child->shared_from_this());
        children[children.size() -1]->parent = this;
        children[children.size() - 1]->GetTransform()->UpdateLocalMatrix(this->GetTransform()->GetMatrix());
        prev_father->RemoveChild(child);

        return;
    }
}

void GameObject::RemoveChild(GameObject* child)
{
    for (auto it = children.begin(); it != children.end(); ++it)
	{
		if ((*it).get() == child)
		{
			children.erase(it);
			break;
		}
	}
}