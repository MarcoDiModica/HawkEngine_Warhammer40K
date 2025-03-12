#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Log.h"
#include "Material.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include <iostream>
#include "LightComponent.h"
#include "Shaders.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyScriptingEngine/EngineBinds.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/object.h>
#include "MyShadersEngine/ShaderComponent.h"
#include "glm/gtx/matrix_decompose.inl"

unsigned int GameObject::nextGid = 1;

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)), gid(nextGid++)
{
    AddComponent<Transform_Component>();
    //transform = GetComponent<Transform_Component>();
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

    if (node != nullptr) {
        node->removeObject(*this);
    }
}

GameObject::GameObject(const GameObject& other) :
    name(other.name),
    gid(nextGid++),
    active(other.active),
    //transform(new Transform_Component(this)),
    mesh(other.mesh),
    tag(other.tag),
    cachedComponentType(typeid(Component)),
    parent(nullptr)
{   
    for (const auto& component : other.components) {
        components[component.first] = std::move( component.second->Clone(this) );
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
        //transform = other.transform;
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
	//transform(std::move(other.transform)),
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
		//transform = std::move(other.transform);
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

    for (auto& scriptComponent : scriptComponents)
    {
        scriptComponent->Start();
    }

    //if (GetName() != "Cube_3") {
    //    scene->tree->Insert(scene->tree->root, *this, 0);
    //}

    for (auto& child : children)
    {
        child->Start();
    }
}

void GameObject::Update(float deltaTime)
{
    //display();
    timeActive += deltaTime;

    if (!active)
    {
        return;
    }

    // Call C# update
    if (CsharpReference) {

    }


    //check the state of the components and throw an error if they are null
    for (auto& component : components)
	{

		if (!component.second)
		{
			throw std::runtime_error("Component is null");
		}
	}
    
    for (auto& component : components)
	{
		component.second->Update(deltaTime);
	}

    for (auto& scriptComponent : scriptComponents)
    {
        scriptComponent->Update(deltaTime);
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

    

    for (auto& child : children)
    {
        child->Destroy();
    }

    return;

    for (auto& component : components)
    {
        component.second->Destroy();
    }

    for (auto& scriptComponent : scriptComponents) {
        scriptComponent->Destroy();
    }
    scriptComponents.clear();
}

void GameObject::Draw() const
{
    //std::cout << "Draw GameObject: " << name << std::endl;
    if (!active) { return; }


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
    glMultMatrixd(GetTransform()->GetData());

    if (HasComponent<MeshRenderer>())
    {
        auto meshRenderer = GetComponent<MeshRenderer>();
        meshRenderer->Render();
    }

    //glMultMatrixd(&glm::dmat4(1.0)[0][0]);

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
    BoundingBox combinedBoundingBox{};

    if (HasComponent<MeshRenderer>()) {
        auto meshRenderer = GetComponent<MeshRenderer>();
        combinedBoundingBox = meshRenderer->GetMesh()->boundingBox();
    }
    else if (!children.empty()) {
        combinedBoundingBox = children.front()->boundingBox();
    }

    for (auto it = children.begin() + (HasComponent<MeshRenderer>() || children.empty() ? 0 : 1); it != children.end(); ++it) {
        combinedBoundingBox = combinedBoundingBox + (*it)->boundingBox();
    }

    return GetTransform()->GetMatrix() * combinedBoundingBox;
}


BoundingBox GameObject::localBoundingBox() const {
    BoundingBox combinedBoundingBox{};

    if (HasComponent<MeshRenderer>()) {
        auto meshRenderer = GetComponent<MeshRenderer>();
        combinedBoundingBox = meshRenderer->GetMesh()->boundingBox();
    }
    else if (!children.empty()) {
        combinedBoundingBox = children.front()->boundingBox();
    }

    for (auto it = children.begin() + (HasComponent<MeshRenderer>() || children.empty() ? 0 : 1); it != children.end(); ++it) {
        combinedBoundingBox = combinedBoundingBox + (*it)->boundingBox();
    }

    return combinedBoundingBox;
}

void GameObject::SetParent(GameObject* parent) {
	if (this->parent == parent) {
		return;
	}

	if (parent == this) {
		return;
	}

	GameObject* p = parent;
	while (p) {
		if (p == this) return; 
		p = p->GetParent();
	}

	if (this->parent) {
		this->parent->RemoveChild(this);
	}

	this->parent = parent;
	if (parent) {
		parent->AddChild(this);
	}
	else if (scene) {
		scene->AddGameObject(shared_from_this());
	}
}

void GameObject::ApplyWorldToLocalTransform(GameObject* child, const glm::dmat4& childWorldMatrix) {
	glm::dmat4 parentWorldMatrix = GetTransform()->GetMatrix();

	glm::dmat4 newLocalMatrix = glm::inverse(parentWorldMatrix) * childWorldMatrix;

	glm::dvec3 skew;
	glm::dvec4 perspective;
	glm::dvec3 localPosition;
	glm::dquat localRotation;
	glm::dvec3 localScale;

	glm::decompose(newLocalMatrix, localScale, localRotation, localPosition, skew, perspective);

	Transform_Component* transform = child->GetTransform();
	transform->SetLocalPosition(localPosition);
	transform->SetRotationQuat(localRotation);
	transform->SetScale(localScale);
}

void GameObject::AddChild(GameObject* child) {
	if (child == this) { return; }

	GameObject* parent = GetParent();
	while (parent) {
		if (parent == child) return; 
		parent = parent->GetParent();
	}

	glm::dmat4 childWorldMatrix = child->GetTransform()->GetMatrix();

	if (child->GetParent() == nullptr) {
		if (child->scene) {
			for (size_t i = 0; i < child->scene->_children.size(); ++i) {
				if (*child->scene->_children[i] == *child) {
					children.push_back(child->shared_from_this());

					if (scene && scene->tree) {
						scene->tree->Insert(scene->tree->root, *children.back(), 0);
					}

					children[children.size() - 1]->parent = this;

					child->scene->_children.erase(child->scene->_children.begin() + i);

					ApplyWorldToLocalTransform(child, childWorldMatrix);
					return;
				}
			}
		}
	}
	else {
		GameObject* previousParent = child->GetParent();

		if (GetParent() == child) {
			for (size_t i = 0; i < child->children.size(); ++i) {
				auto& uncle = child->children[i];
				previousParent->AddChild(uncle.get());
				i--;
			}
		}

		children.push_back(child->shared_from_this());

		if (scene && scene->tree) {
			scene->tree->Insert(scene->tree->root, *children.back(), 0);
		}

		children[children.size() - 1]->parent = this;

		previousParent->RemoveChild(child);

		ApplyWorldToLocalTransform(child, childWorldMatrix);
		return;
	}
}

void GameObject::RemoveChild(GameObject* child)
{
    if (child == nullptr) { return; }
    if (children.empty()) { return; }
    for (auto it = children.begin(); it != children.end(); ++it)
	{
		if ((*it).get() == child)
		{
			children.erase(it);
			break;
		}
	}
}


MonoObject* GameObject::GetSharp() {
    if (CsharpReference) {
        return CsharpReference;
    }

    //Obtenemos el nombre del GO, creamos el string en mono y llamamos a la funcion que crea el GO
    MonoString* monoString = mono_string_new(MonoManager::GetInstance().GetDomain(), name.c_str());
    CsharpReference = EngineBinds::CreateGameObjectSharp(monoString, this);

    return CsharpReference;
}