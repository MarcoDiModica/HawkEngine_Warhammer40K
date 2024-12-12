#include "Root.h"

#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/LightComponent.h"
#include "MyGameEngine/CameraComponent.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Scene.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/ModelImporter.h"
#include "App.h"
#include "Input.h"

#include <SDL2/SDL.h>

#include <iostream>

using namespace std;

class GameObject;

Root::Root(App* app) : Module(app) { ; }

void MakeCity() {
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    auto MarcoVicePresidente = Application->root->CreateGameObject("BakerHouse");
    MarcoVicePresidente->GetTransform()->GetPosition() = vec3(0, 0, 0);

    ModelImporter meshImp;
    meshImp.loadFromFile("Assets/Meshes/BakerHouse.fbx");

    for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
        auto MarcoVicePresidente2 = meshImp.meshGameObjects[i];

        auto go = Application->root->CreateGameObject("GameObject");
        auto color = MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->color;
        Application->root->AddMeshRenderer(*go, MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMesh(), "Assets/default.png");
        go->GetComponent<MeshRenderer>()->GetMaterial()->SetColor(color);
        go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());
        Application->root->ParentGameObject(*go, *MarcoVicePresidente);
    }
}

bool Root::Awake()
{
    //Application->scene_serializer->DeSerialize("Assets/Adios.scene");
    //Application->scene_serializer->DeSerialize("Assets/HolaBuenas.scene");
    MakeCity();

    return true;
}

bool Root::Start()
{
    /*auto Street = CreateGameObject("Street");
    Street->GetTransform()->GetPosition() = vec3(0, 0, 0);
    ModelImporter meshImp;
    meshImp.loadFromFile("Assets/Meshes/ff.fbx");

    for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
        auto& Street2 = meshImp.meshGameObjects[i];
        currentScene->AddGameObject(Street2);
        ParentGameObject(*Street2, *Street);
    }*/

    /*auto MainCamera = CreateGameObject("MainCamera");
    MainCamera->GetTransform()->GetPosition() = vec3(0, 0, -10);
    auto camera = MainCamera->AddComponent<CameraComponent>();
    mainCamera = MainCamera;

    auto cube = CreateCube("Cube");
    cube->GetTransform()->GetPosition() = vec3(0, 0, 0);
    AddMeshRenderer(*cube, Mesh::CreateCube(), "Assets/default.png");*/

    for (shared_ptr<GameObject> object : currentScene->_children)
    {
        object->Start();

        if (object->HasComponent<CameraComponent>()) {
            mainCamera = object;
		}
    }

    return true;
}

bool Root::Update(double dt) {

    //LOG(LogType::LOG_INFO, "Active Scene %s", currentScene->GetName().c_str());
    currentScene->DebugDrawTree();

    currentScene->Update(dt);

    if (Application->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {

        if (currentScene->tree == nullptr) {
            currentScene->tree = new Octree(BoundingBox(vec3(-100, -100, -100), vec3(100, 100, 100)), 10, 1);
            for (auto child : currentScene->children()) {
                currentScene->tree->Insert(currentScene->tree->root, *child, 0);
            }

        }
        else {
            delete currentScene->tree;
            currentScene->tree = nullptr;
            int a = 7;
        }
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
    if (! gameObject->GetParent()) {
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
    else {
        if (gameObject->isSelected) { Application->input->ClearSelection(); }
        //currentScene->AddGameObject(gameObject->shared_from_this());
        gameObject->GetParent()->RemoveChild(gameObject);
        gameObject->Destroy();
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
    currentScene->AddGameObject(gameObject);
    //currentScene->_children.push_back(gameObject);
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

std::shared_ptr<GameObject> Root::CreateCameraObject(const std::string& name) {
	auto camera = CreateGameObject(name);
	camera->AddComponent<CameraComponent>();
	return camera;
}

std::shared_ptr<GameObject> Root::CreateLightObject(const std::string& name) {
	auto light = CreateGameObject(name);
	light->AddComponent<LightComponent>();
	return light;
}

void Root::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat)
{
    auto meshRenderer = go.AddComponent<MeshRenderer>();
    auto image = std::make_shared<Image>();
    auto material = std::make_shared<Material>();
    
    
    meshRenderer->SetMesh(mesh);
    if (mat) {
        material = mat;
        image = mat->getImg();
    }
    else {
        image->LoadTexture(texturePath);
    }
    material->setImage(image);
    meshRenderer->SetMaterial(material);
    //meshRenderer->GetMaterial()->SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));

    if (material->loadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl")) {
        material->useShader = true;
        material->bindShaders();
    }
    //meshRenderer->SetImage(image);
}

void Root::CreateScene(const std::string& name)
{
    auto scene = make_shared<Scene>( name);
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
            currentScene->Start();
            return;
        }
    }
}

std::shared_ptr<Scene> Root::GetActiveScene() const
{
    return currentScene;
}

bool Root::ParentGameObjectToScene(GameObject& child) {
    child.isSelected = false;
    Application->input->ClearSelection();

    GameObject* currentParent = child.GetParent();

    if (currentParent) {
        currentParent->RemoveChild(&child);
    }

    currentScene->_children.push_back(child.shared_from_this());
    child.SetParent(nullptr);

    return true;
}

bool Root::ParentGameObjectToObject(GameObject& child, GameObject& father) {
    child.isSelected = false;
    father.isSelected = false;
    Application->input->ClearSelection();

    auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
        [&child](const std::shared_ptr<GameObject>& obj) { return obj.get() == &child; });
    if (it != currentScene->_children.end()) {
        currentScene->_children.erase(it);
    }

    child.SetParent(&father);
    child.GetTransform()->UpdateLocalMatrix(father.GetTransform()->GetMatrix());

    return true;
}


bool Root::ParentGameObject(GameObject& child, GameObject& father) {

    father.AddChild(&child);

    return false;
}