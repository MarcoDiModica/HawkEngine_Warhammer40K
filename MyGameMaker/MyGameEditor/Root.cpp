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
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include "MyShadersEngine/ShaderComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "MyGameEngine/ShaderManager.h"

class GameObject;

Root::Root(App* app) : Module(app) {}

bool Root::Awake()
{
    SceneManagement = new SceneManager();
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");

    SoundComponent::InitSharedAudioEngine();
    ShaderManager::GetInstance().Initialize();

	/*auto blockout = CreateGameObject("Blockout");
	blockout->GetTransform()->SetScale(glm::vec3(1.685f, 1.685f, 1.685f));

	environment = CreateGameObjectWithPath("Assets/Meshes/environmentSplit.fbx");
	ParentGameObject(*environment, *blockout);
	blockout->GetTransform()->SetPosition(glm::vec3(282, -55, 125));*/

    return true;
}

bool Root::CleanUp()
{
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
	playerMesh->GetTransform()->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));
	playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	ParentGameObject(*playerMesh, *player);
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));

	auto objMainCamera = CreateCameraObject("MainCamera");
	objMainCamera->GetTransform()->SetPosition(glm::dvec3(0, 20.0f, -14.0f));
	objMainCamera->GetTransform()->Rotate(glm::radians(60.0f), glm::dvec3(1, 0, 0));
	auto camera = objMainCamera->AddComponent<CameraComponent>();
	objMainCamera->AddComponent<ScriptComponent>()->LoadScript("PlayerCamera");
	mainCamera = objMainCamera;

    auto cube = CreateCube("Cube");
  
    SceneManagement->Start();

    return true;
}

bool Root::PreUpdate() 
{
	return true;
}

bool Root::Update(double dt)
{
    return true;
}

bool Root::PostUpdate() 
{
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

        auto& MarcoVicePresidente2 = meshImp.fbx_object[i];

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
    }

    if (meshImp.meshes.size() > 0) {
		return MarcoVicePresidente;
	}

	return nullptr;
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