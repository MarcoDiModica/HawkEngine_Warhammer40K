#include <SDL2/SDL.h>
#include <iostream>

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
#include "../MyParticlesEngine/ParticlesEmitterComponent.h"
#include "../MyPhysicsEngine/ColliderComponent.h"
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "App.h"
#include "Input.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include "MyShadersEngine/ShaderComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyUIEngine/UIButtonComponent.h"
#include "../MyUIEngine/UISliderComponent.h"


std::vector<std::shared_ptr<GameObject>> gameObjectsWithColliders;
#include "../MyAudioEngine/SoundComponent.h"
#include "MyGameEngine/ShaderManager.h"

class GameObject;

struct EmitterInfo{
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

std::vector<EmitterInfo> activeEmitters;

Root::Root(App* app) : Module(app) { ; }



std::shared_ptr<GameObject> environment;

bool Root::Awake()
{
    SceneManagement = new SceneManager();
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    SoundComponent::InitSharedAudioEngine();
   /* CreateGameObjectWithPath("Assets/Meshes/Street2.FBX");
    MakeSmokerEmmiter();
    MakeSmokerEmiter2();*/

   /* environment = CreateGameObjectWithPath("Assets/Meshes/environmentSplit.fbx");
    environment->GetTransform()->SetPosition(glm::vec3(0, -34, 0));
    environment->GetTransform()->SetScale(glm::vec3(1, 1, 1));*/
    ShaderManager::GetInstance().Initialize();

    return true;
}

bool Root::CleanUp()
{
    SoundComponent::ShutdownSharedAudioEngine();
    return true;
}

bool Root::Start()
{
    auto player = CreateGameObject("Player");
    player->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
    player->AddComponent<RigidbodyComponent>(Application->physicsModule);
    player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
    player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
    player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
    player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
    player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
    player->AddComponent<SoundComponent>()->LoadAudio("Library/Audio/Menu Confirm.wav", true);

    auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/player.fbx");
    //playerMesh->GetTransform()->SetScale(glm::vec3(0.01f, 0.01f, 0.01f)); //UnComent for HawkEngine scale 
    playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
    ParentGameObject(*playerMesh, *player);
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));

    auto objMainCamera = CreateCameraObject("MainCamera");
    objMainCamera->GetTransform()->SetPosition(glm::dvec3(0, 20.0f, -14.0f));
    objMainCamera->GetTransform()->Rotate(glm::radians(60.0f), glm::dvec3(1, 0, 0));
    auto camera = objMainCamera->AddComponent<CameraComponent>();
    objMainCamera->AddComponent<ScriptComponent>()->LoadScript("PlayerCamera");
    mainCamera = objMainCamera;
  
    auto canvas = CreateGameObject("Canvas");
	canvas->AddComponent<UICanvasComponent>();
    canvas->AddComponent<UITransformComponent>();

    auto staminaBar = CreateGameObject("LifeBar");
    Application->root->ParentGameObject(*staminaBar, *canvas);
    staminaBar->AddComponent<UIImageComponent>();
    staminaBar->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/staminaBar.png");
    staminaBar->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.5, 0));
    staminaBar->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.010, 0.940, 0), glm::vec3(0.350, 0.02, 1));

    auto lifeBar = CreateGameObject("LifeBar");
    Application->root->ParentGameObject(*lifeBar, *canvas);
    lifeBar->AddComponent<UIImageComponent>();
    lifeBar->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/lifeBar.png");
    lifeBar->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.5, 0));
    lifeBar->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.010, 0.940, 0), glm::vec3(0.350, 0.02, 1));
    lifeBar->AddComponent<ScriptComponent>()->LoadScript("ImageAsSlider");


    auto button = CreateGameObject("Button");
    Application->root->ParentGameObject(*button, *canvas);
    button->AddComponent<UIImageComponent>();
    button->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/bottonprueba.png");
	button->AddComponent<UIButtonComponent>();
	button->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    button->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.5, 0), glm::vec3(0.180, 0.150, 1));

   /* auto slider = CreateGameObject("Slider");
    Application->root->ParentGameObject(*slider, *canvas);
    slider->AddComponent<UITransformComponent>();
    slider->AddComponent<UIImageComponent>();
    slider->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/sliderBar.png");
	slider->AddComponent<UISliderComponent>();
    auto sliderButton = CreateGameObject("SliderButton");
    Application->root->ParentGameObject(*sliderButton, *slider);
    sliderButton->AddComponent<UITransformComponent>();
    sliderButton->AddComponent<UIImageComponent>();
    sliderButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/sliderButton.png");*/

    

	
    SceneManagement->Start();

    return true;
}

void CreateEnvironmentColliders()
{
	for (auto& go : environment->GetChildren()) {
	    auto meshCollider = go->AddComponent<ColliderComponent>(Application->physicsModule, true);
		meshCollider->Start();

    }

}


bool hasCreatedColliders = false;

bool Root::Update(double dt) 
{

	if (!hasCreatedColliders) {
		//CreateEnvironmentColliders();
		hasCreatedColliders = true;
	}

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

std::shared_ptr<GameObject> Root::CreateGameObjectWithPath(const std::string& path)
{
    auto MarcoVicePresidente = Application->root->CreateGameObject(path);

    ModelImporter meshImp;
    meshImp.loadFromFile(path);

    for (int i = 0; i < meshImp.meshes.size(); i++) {

        auto MarcoVicePresidente2 = meshImp.fbx_object[i];

        auto go = Application->root->CreateGameObject(meshImp.fbx_object[i]->GetName());
        go->SetName(meshImp.meshes[i]->getModel()->GetMeshName());

        auto meshRenderer = go->AddComponent<MeshRenderer>();
        auto material = std::make_shared<Material>();

        meshRenderer->SetMesh(meshImp.meshes[i]);
        material = meshImp.materials[meshImp.meshes[i]->getModel()->GetMaterialIndex()];

        meshRenderer->SetMaterial(material);
        meshRenderer->GetMaterial()->SetColor(material->GetColor());

        auto shaderComponent = go->AddComponent<ShaderComponent>();
        shaderComponent->SetOwnerMaterial(meshRenderer->GetMaterial().get());
        shaderComponent->SetShaderType(ShaderType::PBR);

		std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
		

        meshBBox->min = meshRenderer->GetMesh()->getModel()->GetModelData().vertexData.front().position;
        meshBBox->max = meshRenderer->GetMesh()->getModel()->GetModelData().vertexData.front().position;

        for (const auto& v : meshRenderer->GetMesh()->getModel()->GetModelData().vertexData) {
            meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
            meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
        }

        go->GetComponent<MeshRenderer>()->GetMesh()->setBoundingBox(*meshBBox);

        go->GetComponent<MeshRenderer>()->GetMesh()->loadToOpenGL();

        go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());
        Application->root->ParentGameObject(*go, *MarcoVicePresidente);
        //gameObjectsWithColliders.push_back(go);
    }

    //for (auto& go : gameObjectsWithColliders) {
    //    go->AddComponent<ColliderComponent>(Application->physicsModule, true);
    //}

    if (meshImp.meshes.size() > 0) {
		return MarcoVicePresidente;
	}
	else {
		return nullptr;
	}
}

void Root::ChangeShader(GameObject& go, ShaderType shader)
{
	go.GetComponent<ShaderComponent>()->SetShaderType(shader);
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

bool Root::ParentGameObject(GameObject& child, GameObject& father) {

    return SceneManagement->ParentGameObject(child, father);
}

bool Root::ParentGameObjectPreserve(GameObject& child, GameObject& father) {
	return SceneManagement->ParentGameObjectPreserve(child, father);
}

std::shared_ptr<GameObject> Root::FindGOByName(std::string name) {
    
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