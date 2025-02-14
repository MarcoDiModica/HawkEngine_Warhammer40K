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
#include "../MyPhysicsEngine/ColliderComponent.h"
#include "App.h"
#include "Input.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyAudioEngine/AudioListener.h"

#include "../MyScriptingEngine/ScriptComponent.h"
#include <SDL2/SDL.h>

#include <iostream>
#include <cstdlib>

using namespace std;

std::vector<std::shared_ptr<GameObject>> gameObjectsWithColliders;

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
    particlesEmmiterComponent->isSmoking = true;
}

void MakeSmokerEmiter2() {
    auto particlesEmitter = Application->root->CreateGameObject("ParticlesEmitter1");
    auto transform = particlesEmitter->GetTransform();
    std::string texturePath = "../MyGameEditor/Assets/Textures/SmokeParticleTexture.png";
    transform->SetPosition(transform->GetPosition() + glm::dvec3(-10, 0, -16)); // Ejemplo de movimiento en el eje X
    transform->Rotate(glm::radians(1.0), glm::dvec3(0, 1, 0)); // Ejemplo de rotaci�n en el eje Y
    ParticlesEmitterComponent* particlesEmmiterComponent = particlesEmitter->AddComponent<ParticlesEmitterComponent>();
    particlesEmmiterComponent->SetTexture(texturePath);
    particlesEmmiterComponent->isSmoking = true;
}

std::shared_ptr<GameObject> CreateParticleEmitter(const glm::vec3& position, const std::string& texturePath) {
    auto particleEmitter = Application->root->CreateGameObject("ParticleEmitter");
    auto transform = particleEmitter->GetTransform();
    transform->SetLocalPosition(position);

    auto emitterComponent = particleEmitter->AddComponent<ParticlesEmitterComponent>();
    emitterComponent->SetTexture(texturePath); // Establecer la textura
    emitterComponent->isSmoking = false;
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
        gameObjectsWithColliders.push_back(go);
    }

    MarcoVicePresidente->GetTransform()->SetScale(vec3(0.5, 0.5, 0.5));
    MarcoVicePresidente->GetTransform()->SetPosition(vec3(0, 0.1, 0));
    MarcoVicePresidente->GetTransform()->Rotate(-1.5708, vec3(1,0,0));

    for (auto& go : gameObjectsWithColliders) {
        go->AddComponent<ColliderComponent>(Application->physicsModule, true);
    }

}

bool Root::Awake()
{
   // SceneManagement = (SceneManager*)malloc(sizeof(SceneManager));
    SceneManagement = new SceneManager();
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    //MonoEnvironment* env = new MonoEnvironment();
    //Application->scene_serializer->DeSerialize("Assets/Adios.scene");
    //Application->scene_serializer->DeSerialize("Assets/HolaBuenas.scene");
    SoundComponent::InitSharedAudioEngine();
    MakeCity();
	MakeSmokerEmmiter();
	MakeSmokerEmiter2();
    /*CreateScene("Viernes13");
    SetActiveScene("Viernes13");
    
    auto MainCamera = CreateCameraObject("MainCamera");
    MainCamera->GetTransform()->SetPosition(glm::dvec3(0, 0.5, 0));
    MainCamera->GetTransform()->Rotate(glm::radians(180.0), glm::dvec3(0, 1, 0));
    auto camera = MainCamera->AddComponent<CameraComponent>();
    mainCamera = MainCamera; */   

	auto Collider = CreateGameObject("Collider");
    //auto colliderComponent = Collider->AddComponent<ColliderComponent>();

    return true;
}

bool Root::CleanUp()
{
    SoundComponent::ShutdownSharedAudioEngine();
    return true;
}

bool Root::Start()
{
    //MonoEnvironment* mono = new MonoEnvironment();

    //auto Script = CreateGameObject("Script");
    //auto script = Script->AddComponent<ScriptComponent>();
    //script->LoadScript("TestingComponent");

    auto BlobFish = CreateGameObject("Tank");
    auto blob = BlobFish->AddComponent<ScriptComponent>();

	auto BlobFish2 = CreateGameObject("Turret");
	auto blob2 = BlobFish2->AddComponent<ScriptComponent>();

    auto BlobFish3 = CreateGameObject("SingleProjectile");
	auto blob3 = BlobFish3->AddComponent<ScriptComponent>();
    
    AddMeshRenderer(*BlobFish, Mesh::CreateCube());
	AddMeshRenderer(*BlobFish2, Mesh::CreateCube());
    AddMeshRenderer(*BlobFish3, Mesh::CreateSphere());

    //auto blob2 = BlobFish->AddComponent<ScriptComponent>();
    blob->LoadScript("TestingComponent");
	blob2->LoadScript("TankController");
	blob3->LoadScript("ProjectileScript");
    
	ParentGameObject(*BlobFish2, *BlobFish);
	ParentGameObject(*BlobFish3, *BlobFish2);
    //blob2->LoadScript("TestingComponent");
    
    //check if blobfish has 2 scripts

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

    SceneManagement->Start();

    return true;
}

bool Root::Update(double dt) {

//LOG(LogType::LOG_INFO, "Active Scene %s", currentScene->GetName().c_str());

    SceneManagement->Update(dt);
    
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
        emitterInfo.SetSpeed({ glm::vec3(0.0f, 3.0f, 0.0f) });
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
    return SceneManagement->CreateMeshObject(name, mesh);
}

void Root::RemoveGameObject(GameObject* gameObject) {
    
    SceneManagement->RemoveGameObject(gameObject);
}

std::shared_ptr<GameObject> Root::CreateGameObject(const std::string& name)
{
    return SceneManagement->CreateGameObject(name);
}

std::shared_ptr<GameObject> Root::CreateCube(const std::string& name) {
    
    return SceneManagement->CreateCube(name);
}

std::shared_ptr<GameObject> Root::CreateSphere(const std::string& name) {
    
    return SceneManagement->CreateSphere(name);
}
std::shared_ptr<GameObject> Root::CreateCylinder(const std::string& name) {
    
    return SceneManagement->CreateCylinder(name);
}

std::shared_ptr<GameObject> Root::CreatePlane(const std::string& name) {
    
       return SceneManagement->CreatePlane(name);
}

std::shared_ptr<GameObject> Root::CreateCameraObject(const std::string& name) {
	
    return SceneManagement->CreateCameraObject(name);
}

std::shared_ptr<GameObject> Root::CreateLightObject(const std::string& name) {
	
    return SceneManagement->CreateLightObject(name);
}

void Root::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat)
{
    return SceneManagement->AddMeshRenderer(go, mesh, texturePath, mat);
}

void Root::CreateScene(const std::string& name)
{
    SceneManagement->CreateScene(name);
}

void Root::AddScene(std::shared_ptr<Scene> scene)
{
    SceneManagement->AddScene(scene);

}

void Root::RemoveScene(const std::string& name)
{
    SceneManagement->RemoveScene(name);
}

void Root::SetActiveScene(const std::string& name)
{
    SceneManagement->SetActiveScene(name);
}

std::shared_ptr<Scene> Root::GetActiveScene() const
{
    return SceneManagement->GetActiveScene();
}

bool Root::ParentGameObjectToScene(GameObject& child) {
    return SceneManagement->ParentGameObjectToScene(child);
}

bool Root::ParentGameObjectToObject(GameObject& child, GameObject& father) {
    
    return SceneManagement->ParentGameObjectToObject(child, father);
}


bool Root::ParentGameObject(GameObject& child, GameObject& father) {

    return SceneManagement->ParentGameObject(child, father);
}

std::shared_ptr<GameObject> Root::FindGOByName(char* name) {
    
    return SceneManagement->FindGOByName(name);
}


std::shared_ptr<GameObject> Root::CreateAudioObject(const std::string& name)
{
    auto gameObject = CreateGameObject(name);
    if (!gameObject) {
        LOG(LogType::LOG_ERROR, "Failed to create audio object");
        return nullptr;
    }

    // Add SoundComponent
    auto soundComponent = gameObject->AddComponent<SoundComponent>();
    if (!soundComponent) {
        LOG(LogType::LOG_ERROR, "Failed to add SoundComponent to audio object");
        return nullptr;
    }

    LOG(LogType::LOG_OK, "Created audio object: %s", name.c_str());
    return gameObject;
}