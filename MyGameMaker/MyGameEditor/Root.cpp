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
    Application->scene_serializer->DeSerialize("Assets/Salimos.scene");

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

    //LOG(LogType::LOG_INFO, "Active Scene %s", currentScene->GetName().c_str());

    for (shared_ptr<GameObject> object : currentScene->_children)
    {
        object->Update(dt);
    }

    if (Application->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {

        //destroy scene
    }


    //if press 1 active scene Viernes13 and press 2 active scene Salimos
    if (Application->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
        Application->scene_serializer->DeSerialize("Assets/Viernes13.scene");
	}
    else if (Application->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
        Application->scene_serializer->DeSerialize("Assets/Salimos.scene");
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
    if (!gameObject) {
        LOG(LogType::LOG_ERROR, "Error: Se ha intentado eliminar un GameObject nulo.");
        return;
    }

    auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
        [gameObject](const auto& child) { return child.get() == gameObject; });

    if (it == currentScene->_children.end()) {
        LOG(LogType::LOG_ERROR, "Error: El GameObject no se encuentra en la escena.");
        return;
    }

    try {
        if ((*it)->isSelected) {
            (*it)->isSelected = false;
            Application->input->ClearSelection();
        }
        (*it)->Destroy();
        it = currentScene->_children.erase(it);
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Error al destruir el GameObject: %s", e.what());
    }
}

std::shared_ptr<GameObject> Root::CreateGameObject(const std::string& name)
{
    std::string uniqueName = name;
    int counter = 1;
    for (std::shared_ptr<GameObject> child : currentScene->_children) {
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

    if (material->loadShaders("vertex_shader.glsl", "fragment_shader.glsl")) {
        material->useShader = true;
        material->bindShaders();
    }
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
            (*it)->Destroy();
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


bool Root::ParentGameObject(GameObject& child, GameObject& father) {

    child.isSelected = false;
    Application->input->ClearSelection();

    for (size_t i = 0; i < currentScene->_children.size(); ++i) {

        if (*currentScene->_children[i] == child) {

            std::shared_ptr<GameObject> _child = currentScene->_children[i];
            father.emplaceChild(*_child);
            currentScene->_children.erase(currentScene->_children.begin() + i);

            return true;

        }

    }

    return false;
}