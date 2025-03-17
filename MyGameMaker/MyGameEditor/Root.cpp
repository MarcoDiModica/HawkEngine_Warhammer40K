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
#include "../MyParticlesEngine/ParticleFX.h"
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
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "../MyAnimationEngine/BoneComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "MyGameEngine/ShaderManager.h"
#include <MyPhysicsEngine/MeshColliderComponent.h>

class GameObject;

Root::Root(App* app) : Module(app) { ; }


std::shared_ptr<GameObject> environment;

bool Root::Awake()
{
    SceneManagement = new SceneManager();
	Application->root->CreateScene("DefaultScene");
	Application->root->SetActiveScene("DefaultScene");
    
	SoundComponent::InitSharedAudioEngine();
	ShaderManager::GetInstance().Initialize();

	CreateMainMenuUI();

	//Application->scene_serializer->DeSerialize("Library/Scenes/MainMenu.scene");

    return true;
}

bool Root::CleanUp()
{
    SoundComponent::ShutdownSharedAudioEngine();
    return true;
}

bool Root::Start()
{
	/*auto player = CreateGameObject("Player");
	player->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
	player->AddComponent<SoundComponent>()->LoadAudio("Library/Audio/Menu Confirm.wav", true);

	auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/MainCharacterAnimated.fbx");
	playerMesh->SetName("playerMesh");
	playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	playerMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	ParentGameObject(*playerMesh, *player);
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	playerMesh->AddComponent<ScriptComponent>()->LoadScript("PlayerAnimations");
	player->AddComponent<RigidbodyComponent>(Application->physicsModule);*/
		
    auto objMainCamera = CreateCameraObject("MainCamera");
    objMainCamera->GetTransform()->SetPosition(glm::dvec3(0, 20.0f, -14.0f));
    objMainCamera->GetTransform()->Rotate(glm::radians(55.0f), glm::dvec3(1, 0, 0));
    auto camera = objMainCamera->AddComponent<CameraComponent>();
	camera->priority = 1;
    //objMainCamera->AddComponent<ScriptComponent>()->LoadScript("PlayerCamera");
    mainCamera = objMainCamera;
	UpdateCameraPriority();

	//auto particleFX = CreateGameObject("ParticleFX");
	//auto emitter = particleFX->AddComponent<ParticleFX>();
	//emitter->ConfigureSmoke();
	//emitter->SetTexture("Assets/SmokeParticleTexture.png");

	//Lictor
	auto lictor = CreateGameObject("Licotr");
	lictor->GetComponent<Transform_Component>()->SetPosition(glm::vec3(-5, 0, -5));
	lictor->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	lictor->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//enemy->AddComponent<RigidbodyComponent>(Application->physicsModule);
	lictor->AddComponent<ScriptComponent>()->LoadScript("EnemyController");
	auto lictorMesh = CreateGameObjectWithPath("Assets/Meshes/Lictor without armature.fbx");
	lictorMesh->SetName("LictorMesh");
	ParentGameObject(*lictorMesh, *lictor);

	//Hormagaunt
	auto hormagaunt = CreateGameObject("Hormagaunt");
	hormagaunt->GetComponent<Transform_Component>()->SetPosition(glm::vec3(5, 0, 5));
	hormagaunt->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	hormagaunt->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//enemy->AddComponent<RigidbodyComponent>(Application->physicsModule);
	hormagaunt->AddComponent<ScriptComponent>()->LoadScript("EnemyController");
	auto hormagauntMesh = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt without rig.fbx");
	hormagauntMesh->SetName("HormagauntMesh");
	ParentGameObject(*hormagauntMesh, *hormagaunt);

	CreateGameplayUI();
	//CreateMainMenuUI();
	
    SceneManagement->Start();

    return true;
}

static void AddCollidersEnv() {
	for (const auto& go : environment->GetChildren()) {
		auto collider = go->AddComponent<MeshColliderComponent>(Application->physicsModule);
		collider->Start();
	}
}

bool hasAddedColliders = false;	

bool Root::Update(double dt) 
{
	if (!hasAddedColliders) {
		//AddCollidersEnv();
		hasAddedColliders = true;
	}

    return true;
}

void Root::SetCameraPriority(std::shared_ptr<GameObject> camera, int priority)
{
	if (mainCamera == nullptr) {
		mainCamera = camera;
		prevCameraPriority = priority;
		mainCamera->SetName("MainCamera");
	}
	else if (priority > prevCameraPriority) {
		mainCamera->SetName("Camera");
		mainCamera = camera;
		prevCameraPriority = priority;
		mainCamera->SetName("MainCamera");
	}
}

void Root::UpdateCameraPriority()
{
	std::vector<std::shared_ptr<GameObject>> cameraGameObjects;

	auto& gameObjects = SceneManagement->GetActiveScene()->_children;

	for (const auto& gameObject : gameObjects) {
		if (gameObject->HasComponent<CameraComponent>()) {
			cameraGameObjects.push_back(gameObject);
		}
	}

	if (!cameraGameObjects.empty()) {
		
		// Sort cameras by priority
		std::sort(cameraGameObjects.begin(), cameraGameObjects.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
			return a->GetComponent<CameraComponent>()->GetPriority() > b->GetComponent<CameraComponent>()->GetPriority();
			});

		// Set the camera with the highest priority as the main camera
		mainCamera = cameraGameObjects[0];
	}
	
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
	ModelImporter meshImp;
	meshImp.loadFromFile(path);

	if (meshImp.meshes.empty()) {
		return nullptr;
	}

	std::shared_ptr<GameObject> rootObject;
	std::shared_ptr<GameObject> go;

	if (meshImp.meshes.size() > 1) {
		std::string fileName = path.substr(path.find_last_of("/\\") + 1);
		fileName = fileName.substr(0, fileName.find_last_of('.'));
		rootObject = Application->root->CreateGameObject(fileName);
	}

	glm::dvec3 globalMin(std::numeric_limits<double>::max());
	glm::dvec3 globalMax(std::numeric_limits<double>::lowest());
	std::vector<std::shared_ptr<GameObject>> createdObjects;

	for (int i = 0; i < meshImp.meshes.size(); i++) {
		if (i >= meshImp.fbx_object.size() || !meshImp.fbx_object[i]) {
			continue;
		}

		auto meshName = meshImp.meshes[i]->getModel()->GetMeshName();
		if (meshName.empty()) {
			meshName = "Mesh_" + std::to_string(i);
		}
		go = Application->root->CreateGameObject(meshName);
		createdObjects.push_back(go);

		auto meshRenderer = go->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(meshImp.meshes[i]);

		auto materialIndex = meshImp.meshes[i]->getModel()->GetMaterialIndex();
		if (materialIndex >= 0 && materialIndex < meshImp.materials.size()) {
			auto material = meshImp.materials[materialIndex];
			meshRenderer->SetMaterial(material);
			meshRenderer->GetMaterial()->SetColor(material->GetColor());
		}

		auto shaderComponent = go->AddComponent<ShaderComponent>();
		shaderComponent->SetOwnerMaterial(meshRenderer->GetMaterial().get());
		shaderComponent->SetShaderType(ShaderType::PBR);

		std::shared_ptr<BoundingBox> meshBBox = std::make_shared<BoundingBox>();
		const auto& vertices = meshRenderer->GetMesh()->getModel()->GetModelData().vertexData;
		if (!vertices.empty()) {
			meshBBox->min = vertices.front().position;
			meshBBox->max = vertices.front().position;
			for (const auto& v : vertices) {
				meshBBox->min = glm::min(meshBBox->min, glm::dvec3(v.position));
				meshBBox->max = glm::max(meshBBox->max, glm::dvec3(v.position));
			}

			glm::mat4 modelMatrix = meshImp.fbx_object[i]->GetTransform()->GetLocalMatrix();
			glm::dvec3 transformedMin = modelMatrix * glm::dvec4(meshBBox->min, 1.0);
			glm::dvec3 transformedMax = modelMatrix * glm::dvec4(meshBBox->max, 1.0);
			globalMin = glm::min(globalMin, transformedMin);
			globalMax = glm::max(globalMax, transformedMax);
		}

		if (!meshImp.animations.empty() && i < meshImp.bonesGameObjects.size()) {
			auto animationComponent = go->AddComponent<SkeletalAnimationComponent>();
			animationComponent->SetAnimation(meshImp.animations[0].get());
			animationComponent->Start();

			for (auto& animation : meshImp.animations) {
				animationComponent->AddAnimation(animation.get());
			}

			std::unordered_map<std::string, std::shared_ptr<GameObject>> boneMap;

			//for (auto& bone : meshImp.bonesGameObjects[i]) {
			//	if (!bone) continue;

			//	auto boneGO = Application->root->CreateGameObject(bone->GetName());
			//	Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
			//	animationComponent->GetAnimator()->AddBoneGameObject(boneGO);
			//	boneMap[bone->GetName()] = boneGO;
			//}

			//for (auto& bone : meshImp.bonesGameObjects[i]) {
			//	if (!bone) continue;

			//	auto boneGO = boneMap[bone->GetName()];
			//	Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
			//	if (boneTransform && !boneTransform->GetParentName().empty()) {
			//		auto it = boneMap.find(boneTransform->GetParentName());
			//		if (it != boneMap.end()) {
			//			ParentGameObject(*boneGO, *(it->second));
			//		}
			//		else {
			//			ParentGameObject(*boneGO, *go);
			//		}
			//	}
			//	else {
			//		ParentGameObject(*boneGO, *go);
			//	}
			//}
		}

		meshRenderer->GetMesh()->setBoundingBox(*meshBBox);
		meshRenderer->GetMesh()->loadToOpenGL();

		go->GetTransform()->SetLocalMatrix(meshImp.fbx_object[i]->GetTransform()->GetLocalMatrix());

		if (meshImp.meshes.size() > 1) {
			ParentGameObject(*go, *rootObject);
		}
	}

	glm::dvec3 modelSize = globalMax - globalMin;
	double maxDimension = std::max(modelSize.x, std::max(modelSize.y, modelSize.z));

	const double targetSize = 2.0;

	double scaleFactor = (maxDimension > 0.0) ? (targetSize / maxDimension) : 0.01;

	const double minScale = 0.001;
	const double maxScale = 100.0;
	scaleFactor = std::min(std::max(scaleFactor, minScale), maxScale);

	LOG(LogType::LOG_INFO, "Modelo '%s' cargado. Tamaño original: [%.2f, %.2f, %.2f], Factor de escala: %.5f",
		path.c_str(), modelSize.x, modelSize.y, modelSize.z, scaleFactor);

	/*for (auto& obj : createdObjects) {
		glm::mat4 originalMatrix = obj->GetTransform()->GetLocalMatrix();
		glm::mat4 scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor)) * originalMatrix;
		obj->GetTransform()->SetLocalMatrix(scaledMatrix);
	}*/

	if (rootObject) {
		glm::mat4 rootMatrix = rootObject->GetTransform()->GetLocalMatrix();
		glm::mat4 scaledRootMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor)) * rootMatrix;
		rootObject->GetTransform()->SetLocalMatrix(scaledRootMatrix);
	}

	if (meshImp.meshes.size() == 1) {
		glm::mat4 originalMatrix = go->GetTransform()->GetLocalMatrix();
		glm::mat4 scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor)) * originalMatrix;
		go->GetTransform()->SetLocalMatrix(scaledMatrix);
	}

	return (meshImp.meshes.size() > 1) ? rootObject : go;
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

void Root::CreateGameplayUI()
{
    auto canvas = CreateGameObject("Canvas");
    canvas->AddComponent<UICanvasComponent>();
    canvas->AddComponent<UITransformComponent>();

	auto playerLife = CreateGameObject("PlayerLife");
	Application->root->ParentGameObject(*playerLife, *canvas);
	playerLife->AddComponent<UIImageComponent>();
	playerLife->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/blood.png");
	playerLife->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.5, 0));
	playerLife->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.143, 0.831, 0), glm::vec3(0.195, 0.054, 1));
	playerLife->AddComponent<ScriptComponent>()->LoadScript("UIGameplay");

	auto playerStamina = CreateGameObject("PlayerStamina");
	Application->root->ParentGameObject(*playerStamina, *canvas);
	playerStamina->AddComponent<UIImageComponent>();
	playerStamina->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/blood.png");
	playerStamina->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.5, 0));
	playerStamina->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.139, 0.899, 0), glm::vec3(0.107, 0.035, 1));

	auto playerStats = CreateGameObject("PlayerStats");
	Application->root->ParentGameObject(*playerStats, *canvas);
	playerStats->AddComponent<UIImageComponent>();
	playerStats->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/healthbar_empty.png");
	playerStats->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	playerStats->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.190, 0.858, 0), glm::vec3(0.318, 0.241, 1));

	auto powerupFrame = CreateGameObject("PowerupFrame");
	Application->root->ParentGameObject(*powerupFrame, *canvas);
	powerupFrame->AddComponent<UIImageComponent>();
	powerupFrame->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/powerup_frame.png");
	powerupFrame->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	powerupFrame->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.738, 0.874, 0), glm::vec3(0.126, 0.108, 1));

	auto gunSwitcher = CreateGameObject("GunSwitcher");
	Application->root->ParentGameObject(*gunSwitcher , *canvas);
	gunSwitcher->AddComponent<UIImageComponent>();
	gunSwitcher->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/gunswitcher_frame.png");
	gunSwitcher->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	gunSwitcher->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.808, 0.852, 0), glm::vec3(0.301, 0.260, 1));

	auto railgunIcon = CreateGameObject("RailgunIcon");
	Application->root->ParentGameObject(*railgunIcon, *canvas);
	railgunIcon->AddComponent<UIImageComponent>();
	railgunIcon->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/railgun_icon.png");
	railgunIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	railgunIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.884, 0.831, 0), glm::vec3(0.100, 0.063, 1));

	auto bulletIcon = CreateGameObject("BulletIcon");
	Application->root->ParentGameObject(*bulletIcon, *canvas);
	bulletIcon->AddComponent<UIImageComponent>();
	bulletIcon->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/bullet_icon.png");
	bulletIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	bulletIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.704, 0.873, 0), glm::vec3(0.010, 0.067, 1));

	auto grenadeIcon = CreateGameObject("GrenadeIcon");
	Application->root->ParentGameObject(*grenadeIcon, *canvas);
	grenadeIcon->AddComponent<UIImageComponent>();
	grenadeIcon->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/grenade_icon.png");
	grenadeIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	grenadeIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.773, 0.872, 0), glm::vec3(0.021, 0.065, 1));	
}

void Root::CreateMainMenuUI()
{
    auto canvas = CreateGameObject("Canvas");
    canvas->AddComponent<UICanvasComponent>();
    canvas->AddComponent<UITransformComponent>();
	//canvas->AddComponent<ScriptComponent>()->LoadScript("MenuButtons");

    auto menuImage = CreateGameObject("MenuImage");
    Application->root->ParentGameObject(*menuImage, *canvas);
    menuImage->AddComponent<UIImageComponent>();
    menuImage->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/Main_Menu_Reference.png");
    menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));

    auto newGameButton = CreateGameObject("NewGameButton");
    Application->root->ParentGameObject(*newGameButton, *canvas);
    newGameButton->AddComponent<UIImageComponent>();
    newGameButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/New_Game.png");
    newGameButton->AddComponent<UIButtonComponent>();
    newGameButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    newGameButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.128, 0.318, 0), glm::vec3(0.182, 0.091, 1));

    auto continueButton = CreateGameObject("ContinueButton");
    Application->root->ParentGameObject(*continueButton, *canvas);
    continueButton->AddComponent<UIImageComponent>();
    continueButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/Continue_button.png");
    continueButton->AddComponent<UIButtonComponent>();
    continueButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    continueButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.128, 0.461, 0), glm::vec3(0.182, 0.091, 1));

    auto optionsButton = CreateGameObject("OptionsButton");
    Application->root->ParentGameObject(*optionsButton, *canvas);
    optionsButton->AddComponent<UIImageComponent>();
    optionsButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/Options_Button.png");
    optionsButton->AddComponent<UIButtonComponent>();
    optionsButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    optionsButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.128, 0.604, 0), glm::vec3(0.182, 0.091, 1));

    auto creditsButton = CreateGameObject("CreditsButton");
    Application->root->ParentGameObject(*creditsButton, *canvas);
    creditsButton->AddComponent<UIImageComponent>();
    creditsButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/Credits_Buttom.png");
    creditsButton->AddComponent<UIButtonComponent>();
    creditsButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    creditsButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.127, 0.748, 0), glm::vec3(0.182, 0.091, 1));

    auto quitButton = CreateGameObject("QuitButton");
    Application->root->ParentGameObject(*quitButton, *canvas);
    quitButton->AddComponent<UIImageComponent>();
    quitButton->GetComponent<UIImageComponent>()->SetTexture("../MyGameEditor/Assets/Textures/Quit_button.png");
    quitButton->AddComponent<UIButtonComponent>();
    quitButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    quitButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.127, 0.906, 0), glm::vec3(0.182, 0.091, 1));
}


void Root::SetMainCamera(std::shared_ptr<GameObject> camera)
{
	mainCamera = camera;
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