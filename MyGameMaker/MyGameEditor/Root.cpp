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
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "../MyAudioEngine/SoundComponent.h"


std::vector<std::shared_ptr<GameObject>> gameObjectsWithColliders;

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


std::shared_ptr<GameObject> player;
const float moveSpeed = 100.0f;
const float jumpForce = 10.0f;

void CreatePlayer() {
    player = Application->root->CreateCube("Player");
    auto transform = player->GetTransform();
    transform->SetPosition(glm::vec3(0, 5, 0));
    transform->SetScale(glm::vec3(1, 1, 1)); 
}

void HandleInput(float dt) {
    if (!player) return;

    auto collider = player->GetComponent<RigidbodyComponent>();
    if (!collider) return;

    auto rigidBody = collider->GetRigidBody();
    if (!rigidBody) return;

    btVector3 desiredVelocity(0, rigidBody->getLinearVelocity().getY(), 0);

    //input
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_L]) desiredVelocity.setX(moveSpeed); 
    if (keys[SDL_SCANCODE_J]) desiredVelocity.setX(-moveSpeed);
    if (keys[SDL_SCANCODE_K]) desiredVelocity.setZ(moveSpeed); 
    if (keys[SDL_SCANCODE_I]) desiredVelocity.setZ(-moveSpeed); 

    // Spawn sphere if E is pressed
    if (keys[SDL_SCANCODE_E]) {

        auto sphere = Application->root->CreateSphere("PhysicsSphere");
        Application->physicsModule->SpawnPhysSphereWithForce(*player, *sphere, 1.0f, 15.0f, 500.0f);
    }

    //Apply acceleration + force
    if (desiredVelocity.length2() > 0) {
        desiredVelocity.setX(desiredVelocity.getX() * moveSpeed / desiredVelocity.length());
        desiredVelocity.setZ(desiredVelocity.getZ() * moveSpeed / desiredVelocity.length());
    }
    btVector3 currentVelocity = rigidBody->getLinearVelocity();
    float acceleration = 10.0f * dt; 
    btVector3 newVelocity = currentVelocity.lerp(desiredVelocity, acceleration);
    rigidBody->setLinearVelocity(btVector3(newVelocity.getX(), currentVelocity.getY(), newVelocity.getZ()));

    // Jumping 
    if (keys[SDL_SCANCODE_SPACE]) {
        if (currentVelocity.getY() < 0.1f && currentVelocity.getY() > -0.1f) {
            rigidBody->setLinearVelocity(btVector3(currentVelocity.getX(), jumpForce, currentVelocity.getZ()));
        }
    }
}


void MakeSmokerEmmiter() {
    auto particlesEmitter = Application->root->CreateGameObject("ParticlesEmitter");
    auto transform = particlesEmitter->GetTransform();
    std::string texturePath = "../MyGameEditor/Assets/Textures/SmokeParticleTexture.png";
    transform->SetPosition(transform->GetPosition() + glm::dvec3(-14, 1, -10)); // Ejemplo de movimiento en el eje X
    transform->Rotate(glm::radians(1.0), glm::dvec3(0, 1, 0)); // Ejemplo de rotaci?n en el eje Y
    ParticlesEmitterComponent* particlesEmmiterComponent = particlesEmitter->AddComponent<ParticlesEmitterComponent>();
    particlesEmmiterComponent->SetTexture(texturePath);
    particlesEmmiterComponent->isSmoking = true;
}

void MakeSmokerEmiter2() {
    auto particlesEmitter = Application->root->CreateGameObject("ParticlesEmitter1");
    auto transform = particlesEmitter->GetTransform();
    std::string texturePath = "../MyGameEditor/Assets/Textures/SmokeParticleTexture.png";
    transform->SetPosition(transform->GetPosition() + glm::dvec3(-10, 0, -16)); // Ejemplo de movimiento en el eje X
    transform->Rotate(glm::radians(1.0), glm::dvec3(0, 1, 0)); // Ejemplo de rotaci?n en el eje Y
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
std::shared_ptr<GameObject> environment = nullptr;

std::shared_ptr<GameObject> Root::CreateCanvasInScene(const std::string& name, const glm::vec3& position, const std::string& texturePath) {
    
    auto planeObject = Application->root->CreatePlane(name);
    auto transform = planeObject->GetTransform();
    transform->SetPosition(position);
    transform->Rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
    transform->Rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
    /*transform->SetScale(glm::dvec3(1.5f, 1.5f, 1.5f));*/

    auto material = std::make_shared<Material>();
	material->imagePtr = std::make_shared<Image>();
	material->imagePtr->LoadTexture(texturePath);

    
    auto mesh = Mesh::CreatePlane();
    auto meshRenderer = planeObject->AddComponent<MeshRenderer>();
    meshRenderer->SetMesh(mesh);
    meshRenderer->SetMaterial(material);

    auto shaderComponent = planeObject->AddComponent<ShaderComponent>();
    shaderComponent->SetOwnerMaterial(meshRenderer->GetMaterial().get());
    shaderComponent->SetShaderType(ShaderType::LIGHT);

    if (mainCamera) {
        initialCanvasOffset = glm::dvec3(position) - mainCamera->GetTransform()->GetPosition();
        initialCanvasRotationOffset = glm::inverse(mainCamera->GetTransform()->GetRotation()) * transform->GetRotation();
        /*initialCanvasScaleOffset = transform->GetScale();*/
        UpdateCanvasTransform(planeObject, mainCamera);
    }
    else {
        std::cerr << "Error: mainCamera es nullptr." << std::endl;
    }

   /* renderFirstObjects.push_back(planeObject);*/

    return planeObject;
}

void Root::UpdateCanvasTransform(std::shared_ptr<GameObject> canvas, std::shared_ptr<GameObject> mainCamera) {
    if (!canvas || !mainCamera) {
        std::cerr << "Error: canvas or mainCamera is null." << std::endl;
        return;
    }

    auto cameraTransform = mainCamera->GetTransform();
    auto canvasTransform = canvas->GetTransform();

    if (cameraTransform && canvasTransform) {
        glm::dvec3 cameraPosition = cameraTransform->GetPosition();
        glm::dvec3 cameraForward = cameraTransform->GetForward();
        glm::dvec3 newPosition = cameraPosition + cameraForward * glm::length(initialCanvasOffset);
        canvasTransform->SetPosition(newPosition);
        glm::dquat newRotation = cameraTransform->GetRotation() * initialCanvasRotationOffset;
        canvasTransform->SetRotationQuat(newRotation);
        /*glm::dvec3 newScale = initialCanvasScaleOffset;
        canvasTransform->SetScale(newScale);*/
    }
}

void Root::RenderScene() {
    //// Renderizar primero los objetos en renderFirstObjects
    //for (const auto& obj : renderFirstObjects) {
    //    obj->Render();
    //}

    //// Renderizar el resto de los objetos de la escena
    //for (const auto& obj : gameObjectsWithColliders) {
    //    obj->Render();
    //}
}
bool Root::Awake()
{
   // SceneManagement = (SceneManager*)malloc(sizeof(SceneManager));
    SceneManagement = new SceneManager();
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    //MonoEnvironment* env = new MonoEnvironment();
	shaders.resize(3);
    shaders[0].LoadShaders("Assets/Shaders/default_vertex_shader.glsl", "Assets/Shaders/default_fragment_shader.glsl");
    shaders[1].LoadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl");
    shaders[2].LoadShaders("Assets/Shaders/water_vertex_shader.glsl", "Assets/Shaders/water_fragment_shader.glsl");
    //Application->scene_serializer->DeSerialize("Assets/Adios.scene");
    //Application->scene_serializer->DeSerialize("Assets/HolaBuenas.scene");
    SoundComponent::InitSharedAudioEngine();
    //CreateGameObjectWithPath("Assets/Meshes/rabbit.FBX");
   /* CreateGameObjectWithPath("Assets/Meshes/Street2.FBX");
    MakeSmokerEmmiter();
    MakeSmokerEmiter2();*/

	auto blockout = CreateGameObject("Blockout");
	blockout->GetTransform()->SetScale(glm::vec3(1.685f, 1.685f, 1.685f));

    environment = CreateGameObjectWithPath("Assets/Meshes/environmentSplit.fbx");
    ParentGameObject(*environment, *blockout);
    blockout->GetTransform()->SetPosition(glm::vec3(0, -34, 0));

    return true;
}

bool Root::CleanUp()
{
    SoundComponent::ShutdownSharedAudioEngine();
    return true;
}

void Root::CreateSceneColliders() {
    for (auto& go : environment->GetChildren())
    {
        auto collider = go->AddComponent<ColliderComponent>(Application->physicsModule);
        collider->Start();
    }
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

    Application->root->CreateGameObjectWithPath("Assets/Meshes/rabbitSizeFix.fbx");

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
  
    auto myPlane = CreateCanvasInScene("UICanvas", glm::vec3(0.0f, 0.5f, -2.0f), "../MyGameEditor/Assets/Textures/UI_Final.png");

    SceneManagement->Start();

    return true;
}

bool hasCreatedCollider = false;
bool Root::Update(double dt) 
{

    auto canvas = FindGOByName(const_cast<char*>("UICanvas"));
    if (canvas) {
        UpdateCanvasTransform(canvas, mainCamera);
    }

    if (!hasCreatedCollider) {
        CreateSceneColliders();
        hasCreatedCollider = true;
    }

    HandleInput(dt);    

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

void Root::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat, std::vector<Shaders> shaders)
{
    return SceneManagement->AddMeshRenderer(go, mesh, texturePath, mat, shaders);
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
        shaderComponent->SetShaderType(ShaderType::LIGHT);

		std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
		

        meshBBox->min = meshRenderer->GetMesh()->getModel()->GetModelData().vertexData.front().position;
        meshBBox->max = meshRenderer->GetMesh()->getModel()->GetModelData().vertexData.front().position;

        for (const auto& v : meshRenderer->GetMesh()->getModel()->GetModelData().vertexData) {
            meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
            meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
        }


		if (meshImp.animations.size() > 0) {
			auto animationComponent = go->AddComponent<SkeletalAnimationComponent>();
            animationComponent->SetAnimation(meshImp.animations[0].get());
            animationComponent->Start();

            for (int i = 0; i < meshImp.animations.size(); ++i) 
            {
                animationComponent->AddAnimation(meshImp.animations[i].get());
            }

            std::unordered_map<std::string, std::shared_ptr<GameObject>> boneMap;

            // Create GameObjects for each bone and store them in the map
            for (auto& bone : meshImp.bonesGameObjects[i]) {
                auto boneGO = CreateGameObject(bone->GetName());
                Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
                //boneGO->GetTransform()->SetLocalMatrix(boneTransform->GetLocalTransform());
                boneMap[bone->GetName()] = boneGO;
            }

            // Establish parent-child relationships
            for (auto& bone : meshImp.bonesGameObjects[i]) {
                auto boneGO = boneMap[bone->GetName()];
                Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
                if (boneTransform->GetParentName() != "") {
                    auto parentGO = boneMap[boneTransform->GetParentName()];
                    ParentGameObject(*boneGO, *parentGO);
                }
                else {
                    ParentGameObject(*boneGO, *go);
                }
            }
		}

        go->GetComponent<MeshRenderer>()->GetMesh()->setBoundingBox(*meshBBox);

        go->GetComponent<MeshRenderer>()->GetMesh()->loadToOpenGL();

        go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());
        //Application->root->ParentGameObject(*go, *MarcoVicePresidente);
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