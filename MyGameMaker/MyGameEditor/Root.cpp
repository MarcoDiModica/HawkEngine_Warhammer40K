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
#include"../MyParticlesEngine/ParticlesEmitterComponent.h"
#include "App.h"
#include "Input.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <cstdlib>

using namespace std;

class GameObject;

struct EmitterInfo {
    std::shared_ptr<GameObject> gameObject;
    std::chrono::steady_clock::time_point creationTime;
    float lifetime = 0.0f;
    Particle* particle = nullptr;
    int maxParticles = 0;

    void SetSpeed(const glm::vec3& newSpeed) {
        if (particle) {
            particle->SetParticleSpeed(newSpeed);
        }
    }
};

// Lista para almacenar los emitters activos
std::vector<EmitterInfo> activeEmitters;

Root::Root(App* app) : Module(app) { ; }

void MakeSmokerEmmiter() {
    auto particlesEmitter = Application->root->CreateGameObject("ParticlesEmitter");
    auto transform = particlesEmitter->GetTransform();
    std::string texturePath = "../MyGameEditor/Assets/Textures/SmokeParticleTexture.png";
    transform->SetPosition(transform->GetPosition() + glm::dvec3(-14, 1, -10)); // Ejemplo de movimiento en el eje X
    transform->Rotate(glm::radians(1.0), glm::dvec3(0, 1, 0)); // Ejemplo de rotaci�n en el eje Y
    ParticlesEmitterComponent* particlesEmmiterComponent = particlesEmitter->AddComponent<ParticlesEmitterComponent>();
	particlesEmmiterComponent->SetTexture(texturePath);
}

void MakeSmokerEmiter2() {
    auto particlesEmitter = Application->root->CreateGameObject("ParticlesEmitter1");
    auto transform = particlesEmitter->GetTransform();
    std::string texturePath = "../MyGameEditor/Assets/Textures/SmokeParticleTexture.png";
    transform->SetPosition(transform->GetPosition() + glm::dvec3(-10, 0, -16)); // Ejemplo de movimiento en el eje X
    transform->Rotate(glm::radians(1.0), glm::dvec3(0, 1, 0)); // Ejemplo de rotaci�n en el eje Y
    ParticlesEmitterComponent* particlesEmmiterComponent = particlesEmitter->AddComponent<ParticlesEmitterComponent>();
    particlesEmmiterComponent->SetTexture(texturePath);
}

std::shared_ptr<GameObject> CreateParticleEmitter(const glm::vec3& position, const std::string& texturePath) {
    auto particleEmitter = Application->root->CreateGameObject("ParticleEmitter");
    auto transform = particleEmitter->GetTransform();
    transform->SetLocalPosition(position);

    auto emitterComponent = particleEmitter->AddComponent<ParticlesEmitterComponent>();
    emitterComponent->SetTexture(texturePath); // Establecer la textura

    return particleEmitter;
}
void MakeCity() {
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    auto MarcoVicePresidente = Application->root->CreateGameObject("City");

    ModelImporter meshImp;
    meshImp.loadFromFile("Assets/Meshes/Street environment_V01.FBX");

    for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
        auto MarcoVicePresidente2 = meshImp.meshGameObjects[i];

        auto go = Application->root->CreateGameObject("GameObject");
        auto color = MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->color;
        Application->root->AddMeshRenderer(*go, MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMesh(), MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->getImg()->image_path);
        go->GetComponent<MeshRenderer>()->GetMaterial()->SetColor(color);
        go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());
        Application->root->ParentGameObject(*go, *MarcoVicePresidente);
    }

    MarcoVicePresidente->GetTransform()->SetScale(vec3(0.5, 0.5, 0.5));
    MarcoVicePresidente->GetTransform()->SetPosition(vec3(0, 0.1, 0));
    MarcoVicePresidente->GetTransform()->Rotate(-1.5708, vec3(1,0,0));
}

bool Root::Awake()
{
    //Application->scene_serializer->DeSerialize("Assets/Adios.scene");
    //Application->scene_serializer->DeSerialize("Assets/HolaBuenas.scene");
    MakeCity();
	MakeSmokerEmmiter();
	MakeSmokerEmiter2();
    /*CreateScene("Viernes13");
    SetActiveScene("Viernes13");*/

    auto MainCamera = CreateCameraObject("MainCamera");
    MainCamera->GetTransform()->SetPosition(glm::dvec3(0, 0.5, 0));
    MainCamera->GetTransform()->Rotate(glm::radians(180.0), glm::dvec3(0, 1, 0));
    auto camera = MainCamera->AddComponent<CameraComponent>();
    mainCamera = MainCamera;

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


    if (Application->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
        float randomX = static_cast<float>(rand() % 20 - 10); // Rango de -10 a 10
        float randomY = 0.0f;
        float randomZ = static_cast<float>(rand() % 20 - 10); // Rango de -10 a 10
        glm::vec3 particlePosition = glm::vec3(randomX, randomY, randomZ);

        std::string texturePath = "../MyGameEditor/Assets/Textures/Esteladeluz.png"; // Reemplaza con la ruta de tu textura
        std::shared_ptr<GameObject> particleEmitter = CreateParticleEmitter(particlePosition, texturePath);

        EmitterInfo emitterInfo;
        emitterInfo.gameObject = particleEmitter;
        emitterInfo.creationTime = std::chrono::steady_clock::now();
        emitterInfo.lifetime = 2.0f;
        emitterInfo.particle = new Particle();
        emitterInfo.SetSpeed({ glm::vec3(0.0f, 2.0f, 0.0f) });
        emitterInfo.maxParticles = 3;
        activeEmitters.push_back(emitterInfo);
    }

    for (auto it = activeEmitters.begin(); it != activeEmitters.end(); ) {
        float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - it->creationTime).count();

        if (elapsedTime >= it->lifetime) {
            it->gameObject->Destroy();
            it = activeEmitters.erase(it);
        }
        else {
            auto transform = it->gameObject->GetTransform();
            glm::vec3 position = glm::vec3(transform->GetLocalMatrix()[3]);
            position += it->particle->speed[0] * static_cast<float>(dt);
            transform->SetLocalPosition(position);
            ++it;
        }
    }
        //LOG(LogType::LOG_INFO, "Active Scene %s", currentScene->GetName().c_str());
        currentScene->DebugDrawTree();

        currentScene->Update(static_cast<float>(dt));



        //if (Application->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {
        //
        //    if (currentScene->tree == nullptr) {
        //        currentScene->tree = new Octree(BoundingBox(vec3(-100, -100, -100), vec3(100, 100, 100)), 10, 1);
        //        for (auto child : currentScene->children()) {
        //            currentScene->tree->Insert(currentScene->tree->root, *child, 0);
        //        }
        //
        //    }
        //    else {
        //        delete currentScene->tree;
        //        currentScene->tree = nullptr;
        //        int a = 7;
        //    }
        //}
        //
        //
        //
        ////if press 1 active scene Viernes13 and press 2 active scene Salimos
        //if (Application->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
        //    Application->scene_serializer->DeSerialize("Assets/Viernes13.scene");
        //}
        //else if (Application->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
        //    Application->scene_serializer->DeSerialize("Assets/Salimos.scene");
        //}


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
        LOG(LogType::LOG_ERROR, "Error: Is have tried erased a GameObject null.");
        return;
    }
    if (! gameObject->GetParent()) {
        auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
            [gameObject](const auto& child) { return child.get() == gameObject; });

        if (it == currentScene->_children.end()) {
            LOG(LogType::LOG_ERROR, "Error: The GameObject not is find on the scene.");
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
            LOG(LogType::LOG_ERROR, "Error to destroying the GameObject: %s", e.what());
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

    //if (material->loadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl")) {
    //    material->useShader = true;
    //    material->bindShaders();
    //}
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