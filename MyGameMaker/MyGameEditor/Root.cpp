#include "Root.h"

#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Scene.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include "App.h"
#include "Input.h"

#include <SDL2/SDL.h>

#include <iostream>

using namespace std;

class GameObject;

Root::Root(App* app) : Module(app) { ; }

bool  Root::Awake() 
{       
    AddScene(make_shared<Scene>("Scene1"));
    SetActiveScene("Scene1");
    
    auto MarcoVicePresidente = CreateGameObject("BakerHouse");
    MarcoVicePresidente->GetTransform()->GetPosition() = vec3(0, 0, 0);
    auto mesh = make_shared<Mesh>();
    mesh->LoadMesh("Assets/Meshes/BakerHouse.fbx");
    AddMeshRenderer(*MarcoVicePresidente, mesh, "Assets/Baker_house.png");

    return true;
}

bool Root::Start() 
{ 
    for (shared_ptr<GameObject> object : currentScene->_children)
    {
        object->Start();
    }

    return true;
}

bool Root::Update(double dt) { 

    for (shared_ptr<GameObject> object : currentScene->_children) 
    {
        object->Update(dt);
    }

    if (Application->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {

        //destroy scene

    }

    return true; 
}

shared_ptr<GameObject> Root::CreateMeshObject(string name, shared_ptr<Mesh> mesh)
{
    auto go = CreateGameObject(name);
    AddMeshRenderer(*go, mesh, "Assets/default.png");

    return nullptr;
}

void Root::RemoveGameObject(GameObject* gameObject) {
    for (auto it = currentScene->_children.begin(); it != currentScene->_children.end(); ) {
        if ((*it).get() == gameObject) { // Compara las direcciones de memoria
            if ((*it)->isSelected) {
                (*it)->isSelected = false;
                Application->input->SetSelectedGameObject(nullptr);
            }
            (*it)->Destroy(); // Call Destroy on the object.
            it = currentScene->_children.erase(it); // Erase returns the next iterator.
            return; // Exit after removing the object.
        }
        else {
            ++it; // Move to the next element if not removed.
        }
    }
}

std::shared_ptr<GameObject> Root::CreateGameObject(const std::string& name) 
{
    std::string uniqueName = name;
    int counter = 1;
    for (const auto& child : currentScene->_children) {
        if (child->GetName() == uniqueName) {
            uniqueName = name + "_" + std::to_string(counter++);
        }
    }

    auto gameObject = std::make_shared<GameObject>(uniqueName);
    currentScene->_children.push_back(gameObject);
    return gameObject;
}

std::shared_ptr<GameObject> Root::CreateCube(const std::string& name) {
    auto cube = CreateGameObject(name);
    AddMeshRenderer(*cube, Mesh::CreateCube(), "Assets/default.png");
    return cube;
}

std::shared_ptr<GameObject> Root::CreateSphere(const std::string& name) {
    auto sphere = CreateGameObject(name);
    AddMeshRenderer(*sphere, Mesh::CreateSphere(), "Assets/default.png");
    return sphere;
}

std::shared_ptr<GameObject> Root::CreatePlane(const std::string& name) {
    auto plane = CreateGameObject(name);
    AddMeshRenderer(*plane, Mesh::CreatePlane(), "Assets/default.png");
    return plane;
}

void Root::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath)
{
    auto meshRenderer = go.AddComponent<MeshRenderer>();
    auto image = std::make_shared<Image>();
    auto material = std::make_shared<Material>();
    image->LoadTexture(texturePath);
    material->setImage(image);
    meshRenderer->SetMesh(mesh);
    meshRenderer->SetMaterial(material);
    meshRenderer->SetImage(image);
}

void Root::CreateScene(const std::string& name)
{
	auto scene = make_shared<Scene>(name);
	scenes.push_back(scene);
}

void Root::AddScene(std::shared_ptr<Scene> scene)
{
	scenes.push_back(scene);
}

void Root::RemoveScene(const std::string& name)
{
	for (auto it = scenes.begin(); it != scenes.end(); ) {
		if ((*it)->name == name) {
			it = scenes.erase(it);
			return;
		}
		else {
			++it;
		}
	}
}

void Root::SetActiveScene(const std::string& name)
{
	for (auto scene : scenes) {
		if (scene->name == name) {
			currentScene = scene;
			return;
		}
	}
}

std::shared_ptr<Scene> Root::GetActiveScene() const
{
	return currentScene;
}