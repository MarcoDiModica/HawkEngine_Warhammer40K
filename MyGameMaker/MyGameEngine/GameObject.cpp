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

    //if (GetName() != "Cube_3") {
    //    scene->tree->Insert(scene->tree->root, *this, 0);
    //}

    for (auto& child : children)
    {
        child->Start();
    }
}

void GameObject::ShaderUniforms(glm::dmat4 view, glm::dmat4 projection, glm::dvec3 cameraPosition, std::list<GameObject*> lights, Shaders useShader)
{

	if (HasComponent<MeshRenderer>() == true ) 
    {
		//if (GetComponent<MeshRenderer>()->GetMaterial()->GetShader().GetProgram()  == 0) {
        //    GetComponent<MeshRenderer>()->GetMaterial()->SetShader(useShader);
		//}

        GetComponent<MeshRenderer>()->GetMaterial()->bindShaders();
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("aPos", glm::vec3(0, 0, 0));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("model", GetTransform()->GetMatrix());
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("modColor", glm::vec4(1, 0.2f, 0, 1));
        glUniform4f(
            glGetUniformLocation(GetComponent<MeshRenderer>()->GetMaterial()->shader.GetProgram(), "modColor"),
            static_cast<GLfloat>(GetComponent<MeshRenderer>()->GetMaterial()->GetColor().x),
            static_cast<GLfloat>(GetComponent<MeshRenderer>()->GetMaterial()->GetColor().y),
            static_cast<GLfloat>(GetComponent<MeshRenderer>()->GetMaterial()->GetColor().z),
            static_cast<GLfloat>(GetComponent<MeshRenderer>()->GetMaterial()->GetColor().w)
        );        //GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("model", GetComponent<Transform_Component>()->GetMatrix());
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("view", view);
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("projection", projection);

        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("viewPos", cameraPosition);

		int numPointLights = static_cast<int>( lights.size());
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("numPointLights", numPointLights);

        int i = 0;
        for (const auto& light : lights)
        {
            std::string pointLightstr = "pointLights[" + std::to_string(i) + "]";
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".position", light->GetComponent<Transform_Component>()->GetPosition());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".ambient", light->GetComponent<LightComponent>()->GetAmbient());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".diffuse", light->GetComponent<LightComponent>()->GetDiffuse());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".specular", light->GetComponent<LightComponent>()->GetSpecular());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".constant", light->GetComponent<LightComponent>()->GetConstant());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".linear", light->GetComponent<LightComponent>()->GetLinear());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".quadratic", light->GetComponent<LightComponent>()->GetQuadratic());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".radius", light->GetComponent<LightComponent>()->GetRadius());
            GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform(pointLightstr + ".intensity", light->GetComponent<LightComponent>()->GetIntensity());
            i++;
        }

        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("dirLight.intensity", 3.0f);

        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_Time", timeActive);
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_Amplitude", GetComponent<ShaderComponent>()->amplitude);
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_Frequency", GetComponent<ShaderComponent>()->frequency);
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_ColorLow", glm::vec3(0.0f, 0.0f, 1.0f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_ColorHigh", glm::vec3(1.0f, 1.0f, 1.0f));
        GetComponent<MeshRenderer>()->GetMaterial()->setShaderUniform("u_Factor", 0.8f);

	}

	//for (auto& child : children)
	//{
	//	child->ShaderUniforms(view,projection,cameraPosition,lights,useShader);
	//}
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
    if (child == this) { return; }

    if (child->GetParent() == nullptr)/* If child is in the scene */ {
        if (child->scene) {

            if (GetParent() == child) {
                for (size_t i = 0; i < child->children.size(); ++i) {
                    auto& uncle = child->children[i];
                    scene->AddGameObject(uncle);
                    child->RemoveChild(child->children[i].get());
                    i--;
                }
            }
            
            for (size_t i = 0; i < child->scene->_children.size(); ++i) {

                if (*child->scene->_children[i] == *child) {

                    children.push_back(child->shared_from_this());
                    if (scene->tree) {
                        scene->tree->Insert(scene->tree->root, *children.back(), 0);
                    }

                    children[children.size() - 1]->parent = this;
                    children[children.size() - 1]->GetTransform()->UpdateLocalMatrix(this->GetTransform()->GetMatrix());

                    child->scene->_children.erase(child->scene->_children.begin() + i);
                    return;
                }
            }
        }
    }
    else {
        GameObject* step_father = child->GetParent();
        /*The problem when you call AddChild when a child childifies its parent is that the paarent is being copied with the child that now parents it*/
        if ( GetParent() == child) {
                for (size_t i = 0; i < child->children.size(); ++i) {
                    auto& uncle = child->children[i];
                    step_father->AddChild(uncle.get());
                    i--;
                }
        }

        children.push_back(child->shared_from_this());

        if (scene->tree) {
            scene->tree->Insert(scene->tree->root, *children.back(), 0);
        }

        children[children.size() -1]->parent = this;
        children[children.size() - 1]->GetTransform()->UpdateLocalMatrix(this->GetTransform()->GetMatrix());
        step_father->RemoveChild(child);

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


MonoObject* GameObject::GetSharp() {
    if (CsharpReference) {
        return CsharpReference;
    }

    //Obtenemos el nombre del GO, creamos el string en mono y llamamos a la funcion que crea el GO
    MonoString* monoString = mono_string_new(MonoManager::GetInstance().GetDomain(), name.c_str());
    CsharpReference = EngineBinds::CreateGameObjectSharp(monoString);

    return CsharpReference;
}