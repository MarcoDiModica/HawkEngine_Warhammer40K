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
#include "../MyPhysicsEngine/BoxColliderComponent.h"
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "App.h"
#include "Input.h"
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
#include <MyPhysicsEngine/CapsuleColliderComponent.h>
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyGameEngine/Tweening.h"

class GameObject;

Root::Root(App* app) : Module(app) { ; }


std::shared_ptr<GameObject> environment;

bool Root::Awake()
{
	resourceManager->LoadResources();
    SceneManagement = new SceneManager();
	resourceManager = new ResourceManager();
	Application->root->CreateScene("DefaultScene");
	Application->root->SetActiveScene("DefaultScene");
	
	ShaderManager::GetInstance().Initialize();
	MonoManager::GetInstance().EnableHotReloading();


	//CreateMainMenuUI();
	
	
    return true;
}

bool Root::CleanUp()
{
	resourceManager->CleanUp();
    return true;
}

bool Root::Start()
{

	//Application->scene_serializer->DeSerialize("Library/Scenes/Lvl1SundayRelease.scene");

	//Application->scene_serializer->DeSerialize("Library/Scenes/Lvl1SundayRelease.scene");


	//auto scene = CreateGameObjectWithPath("Assets/Meshes/SpaceShip.fbx");
	//auto scenezone23 = CreateGameObjectWithPath("Assets/Meshes/BlockingLvl2area2&3.fbx");
	////	
	////environment = CreateGameObjectWithPath("Assets/Meshes/Zone1.fbx");
	////environment->GetTransform()->SetScale(glm::dvec3(0.01f, 0.01f, 0.01f));
	//Audio Trigger
	//auto audioTrigger = CreateGameObject("AudioTrigger");
	//audioTrigger->GetTransform()->SetPosition(glm::vec3(10, 2, 0));
	//audioTrigger->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	//audioTrigger->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//audioTrigger->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(1, 1, 1));
	//audioTrigger->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, 0, 0));
	//audioTrigger->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//audioTrigger->AddComponent<SoundComponent>();
	//audioTrigger->AddComponent<ScriptComponent>()->LoadScript("SoundTrigger");

	//auto scene = CreateGameObjectWithPath("Assets/Meshes/Level1.fbx");
	//auto scenezone1 = CreateGameObjectWithPath("Assets/Meshes/Lvl1Zone3Blockout.fbx");
	//	
	//environment = CreateGameObjectWithPath("Assets/Meshes/Zone1.fbx");
	//environment->GetTransform()->SetScale(glm::dvec3(0.01f, 0.01f, 0.01f));

	////Application->scene_serializer->DeSerialize("Library/Scenes/MainMenu.scene");
	
	//auto canvasMainMenu = FindGOByName("Canvas_Main_Menu");
	//canvasMainMenu->AddComponent<ScriptComponent>()->LoadScript("MenuButtons");
	//auto canvasPauseMenu = FindGOByName("Canvas_PauseMenu");
	//canvasPauseMenu->AddComponent<ScriptComponent>()->LoadScript("PauseMenu");
	//auto canvasOptionsMenu = FindGOByName("Canvas_OptionsMenu");
	//canvasOptionsMenu->AddComponent<ScriptComponent>()->LoadScript("OptionMenu");
	//auto canvasLoseScreen = FindGOByName("Canvas_lose_screen");
	//canvasLoseScreen->AddComponent<ScriptComponent>()->LoadScript("LoseScreen");
	//auto canvasWinScreen = FindGOByName("Canvas_win_screen");
	//canvasWinScreen->AddComponent<ScriptComponent>()->LoadScript("WinScreen");
	//auto canvasHUD = FindGOByName("Canvas_HUD");
	//canvasHUD->AddComponent<ScriptComponent>()->LoadScript("HUD");

	//Application->scene_serializer->DeSerialize("Library/Scenes/Mortis_Level1.Scene");

	//auto player = CreateGameObject("Player");
	//player->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
	//player->AddComponent<ScriptComponent>()->LoadScript("PlayerPowerUp");
	//player->AddComponent<ScriptComponent>()->LoadScript("Boltgun");
	//player->AddComponent<ScriptComponent>()->LoadScript("Shotgun");
	//player->AddComponent<ScriptComponent>()->LoadScript("GrenadeLauncher");
	//player->AddComponent<ScriptComponent>()->LoadScript("Railgun");
	//player->AddComponent<ScriptComponent>()->LoadScript("LaserBeam");
	//player->AddComponent<ScriptComponent>()->LoadScript("EnergyBall");
	//player->AddComponent<ScriptComponent>()->LoadScript("ToggleMode");
	//player->AddComponent<ScriptComponent>()->LoadScript("Barrage");
	//player->AddComponent<ScriptComponent>()->LoadScript("HookShot");
	//player->AddComponent<ScriptComponent>()->LoadScript("ArcSnare");
	//player->AddComponent<ScriptComponent>()->LoadScript("RedThirstManager");
	//
	//auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/dieno zachael.fbx");
	//playerMesh->SetName("playerMesh");
	//playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//playerMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	//playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//ParentGameObject(*playerMesh, *player);
	//playerMesh->AddComponent<ScriptComponent>()->LoadScript("PlayerAnimations");
	//player->AddComponent<CapsuleColliderComponent>(Application->physicsModule);
	//player->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//player->GetComponent<RigidbodyComponent>()->SetFreezeRotations(true);
	//player->GetComponent<RigidbodyComponent>()->SetGravity(glm::vec3(0, -200, 0));
	//player->GetComponent<CapsuleColliderComponent>()->SetSize(glm::vec3(1.7f, 1.1f, 1));
	//player->GetComponent<CapsuleColliderComponent>()->SetOffset(glm::vec3(0, 2.1f, 0));
	////player->AddComponent<ScriptComponent>()->LoadScript("InteractionSystem");
	//auto riffleShotFX = CreateGameObject("RiffleShotFX");
	//riffleShotFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	//ParentGameObject(*riffleShotFX, *player);
	//riffleShotFX->AddComponent<ParticleFX>()->ApplyPreset(8);
	//riffleShotFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);
	//
	//auto inactiveDashFX = CreateGameObject("InactiveDashFX");
	//inactiveDashFX->GetTransform()->SetPosition(glm::vec3(0, 3, -1));
	//ParentGameObject(*inactiveDashFX, *player);
	//inactiveDashFX->AddComponent<ParticleFX>()->ApplyPreset(1);
	//inactiveDashFX->GetComponent<ParticleFX>()->SetParticleSize(2, 2);
	//
	//auto walkingFX = CreateGameObject("WalkingFX");
	//walkingFX->GetTransform()->SetPosition(glm::vec3(0, 0, -1));
	//ParentGameObject(*walkingFX, *player);
	//walkingFX->AddComponent<ParticleFX>()->ApplyPreset(1);

	/*auto player = CreateGameObject("Player");
	player->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerPowerUp");
	player->AddComponent<ScriptComponent>()->LoadScript("Boltgun");
	player->AddComponent<ScriptComponent>()->LoadScript("Shotgun");
	player->AddComponent<ScriptComponent>()->LoadScript("GrenadeLauncher");
	player->AddComponent<ScriptComponent>()->LoadScript("Railgun");
	player->AddComponent<ScriptComponent>()->LoadScript("LaserBeam");
	player->AddComponent<ScriptComponent>()->LoadScript("EnergyBall");
	player->AddComponent<ScriptComponent>()->LoadScript("ToggleMode");
	player->AddComponent<ScriptComponent>()->LoadScript("Barrage");
	player->AddComponent<ScriptComponent>()->LoadScript("HookShot");
	player->AddComponent<ScriptComponent>()->LoadScript("ArcSnare");
	player->AddComponent<ScriptComponent>()->LoadScript("RedThirstManager");
	player->AddComponent<SoundComponent>(Application->audioEngine);
	
	auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/dieno zachael.fbx");
	playerMesh->SetName("playerMesh");
	playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	playerMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	ParentGameObject(*playerMesh, *player);
	playerMesh->AddComponent<ScriptComponent>()->LoadScript("PlayerAnimations");
	player->AddComponent<CapsuleColliderComponent>(Application->physicsModule);
	player->AddComponent<RigidbodyComponent>(Application->physicsModule);
	player->GetComponent<RigidbodyComponent>()->SetFreezeRotations(true);
	player->GetComponent<RigidbodyComponent>()->SetGravity(glm::vec3(0, -200, 0));
	player->GetComponent<CapsuleColliderComponent>()->SetSize(glm::vec3(1.7f, 1.1f, 1));
	player->GetComponent<CapsuleColliderComponent>()->SetOffset(glm::vec3(0, 2.1f, 0));
	player->AddComponent<ScriptComponent>()->LoadScript("InteractionSystem");
	auto riffleShotFX = CreateGameObject("RiffleShotFX");
	riffleShotFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	ParentGameObject(*riffleShotFX, *player);
	riffleShotFX->AddComponent<ParticleFX>()->ApplyPreset(8);
	riffleShotFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);
	
	auto inactiveDashFX = CreateGameObject("InactiveDashFX");
	inactiveDashFX->GetTransform()->SetPosition(glm::vec3(0, 3, -1));
	ParentGameObject(*inactiveDashFX, *player);
	inactiveDashFX->AddComponent<ParticleFX>()->ApplyPreset(1);
	inactiveDashFX->GetComponent<ParticleFX>()->SetParticleSize(2, 2);
	
	auto walkingFX = CreateGameObject("WalkingFX");
	walkingFX->GetTransform()->SetPosition(glm::vec3(0, 0, -1));
	ParentGameObject(*walkingFX, *player);
	walkingFX->AddComponent<ParticleFX>()->ApplyPreset(1);*/
	//Application->scene_serializer->DeSerialize("Library/Scenes/Level1SundayDelivery.Scene");

	auto objMainCamera = CreateCameraObject("MainCamera");
	objMainCamera->GetTransform()->SetPosition(glm::dvec3(-14, 20, 14.0f));
	objMainCamera->GetTransform()->SetRotation(glm::dvec3(glm::radians(-130.0), glm::radians(45.0), glm::radians(180.0)));
	auto camera = objMainCamera->AddComponent<CameraComponent>();
	camera->priority = 1;
	objMainCamera->AddComponent<ScriptComponent>()->LoadScript("PlayerCamera");
	mainCamera = objMainCamera;
	UpdateCameraPriority();

	//particle->ApplyPreset(Particle)

	/*auto itemtest = CreateCube("item");
	itemtest->GetTransform()->SetPosition(glm::vec3(10, 2, 0));
	itemtest->GetTransform()->SetScale(glm::vec3(5, 5, 5));
	itemtest->AddComponent<BoxColliderComponent>(Application->physicsModule);
	itemtest->AddComponent<ScriptComponent>()->LoadScript("Item");
	itemtest->SetTag("Interactable");
	
	auto itemtest2 = CreateCube("item");
	itemtest2->GetTransform()->SetPosition(glm::vec3(-20, 2, 10));
	itemtest2->GetTransform()->SetScale(glm::vec3(2, 2, 2));
	itemtest2->AddComponent<BoxColliderComponent>(Application->physicsModule);
	itemtest2->AddComponent<ScriptComponent>()->LoadScript("AreaTrigger");
	itemtest2->SetTag("AreaTrigger");*/

	/*auto floor2 = CreateCube("Player2");
	floor2->GetTransform()->SetPosition(glm::vec3(10, 3, 0));
	floor2->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	floor2->AddComponent<RigidbodyComponent>(Application->physicsModule);*/

	//environment = CreateGameObjectWithPath("Assets/Meshes/Level1.fbx");
	///*environment = CreateGameObjectWithPath("Assets/Meshes/Lvl1Zone3Blockout.fbx");*/
	//environment->GetTransform()->SetScale(glm::dvec3(0.03f, 0.03f, 0.03f));

auto cube = CreateCube("Cube");
cube->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
cube->GetTransform()->SetScale(glm::vec3(1, 1, 1));
cube->AddComponent<ScriptComponent>()->LoadScript("Test1");

	
	//// Test PowerUps
	//
	//auto powerUp = CreateGameObjectWithPath("Assets/Meshes/MedicaeStimm.fbx");
	//powerUp->GetTransform()->SetPosition(glm::vec3(10, 3, 5));
	//powerUp->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> MedicaeStimmBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> MedicaeStimmRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> MedicaeStimmNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> MedicaeStimmMetallic = std::make_shared<Image>();
	//MedicaeStimmBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//MedicaeStimmRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//MedicaeStimmNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//MedicaeStimmMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp->GetComponent<MeshRenderer>()->GetMaterial()->setImage(MedicaeStimmBaseColor);
	//powerUp->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(MedicaeStimmRoughness);
	//powerUp->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(MedicaeStimmNormal);
	//powerUp->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(MedicaeStimmMetallic);
	//powerUp->AddComponent<ScriptComponent>()->LoadScript("MedicaeStimm");
	//powerUp->SetTag("PowerUp");
	////
	//auto powerUp5 = CreateGameObjectWithPath("Assets/Meshes/ChapterStandard.fbx");
	//powerUp5->GetTransform()->SetPosition(glm::vec3(-30, 3, 0));
	//powerUp5->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp5->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp5->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, 2.5, 0));
	//powerUp5->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> ChapterStandardBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> ChapterStandardRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> ChapterStandardNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> ChapterMetallic = std::make_shared<Image>();
	//ChapterStandardBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//ChapterStandardRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//ChapterStandardNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//ChapterMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp5->GetComponent<MeshRenderer>()->GetMaterial()->setImage(ChapterStandardBaseColor);
	//powerUp5->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(ChapterStandardRoughness);
	//powerUp5->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(ChapterStandardNormal);
	//powerUp5->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(ChapterMetallic);
	//powerUp5->AddComponent<ScriptComponent>()->LoadScript("ChapterStandard");
	//powerUp5->SetTag("PowerUp");
	////
	//auto powerUp6 = CreateGameObjectWithPath("Assets/Meshes/BlackHeart.fbx");
	//powerUp6->GetTransform()->SetPosition(glm::vec3(-40, 3, 0));
	//powerUp6->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp6->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp6->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> BlackHeartBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> BlackHeartRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> BlackHeartNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> BlackHeartMetallic = std::make_shared<Image>();
	//BlackHeartBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//BlackHeartRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//BlackHeartNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//BlackHeartMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp6->GetComponent<MeshRenderer>()->GetMaterial()->setImage(BlackHeartBaseColor);
	//powerUp6->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(BlackHeartRoughness);
	//powerUp6->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(BlackHeartNormal);
	//powerUp6->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(BlackHeartMetallic);
	//powerUp6->AddComponent<ScriptComponent>()->LoadScript("BlackHeart");
	//powerUp6->SetTag("PowerUp");
	//
	//auto powerUp2 = CreateGameObjectWithPath("Assets/Meshes/Magnet.fbx");
	//powerUp2->GetTransform()->SetPosition(glm::vec3(0, 3, -10));
	//powerUp2->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp2->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp2->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> MagnetBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> MagnetRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> MagnetNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> MagnetMetallic = std::make_shared<Image>();
	//MagnetBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//MagnetRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//MagnetNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//MagnetMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp2->GetComponent<MeshRenderer>()->GetMaterial()->setImage(MagnetBaseColor);
	//powerUp2->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(MagnetRoughness);
	//powerUp2->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(MagnetNormal);
	//powerUp2->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(MagnetMetallic);
	//powerUp2->AddComponent<ScriptComponent>()->LoadScript("Magnet");
	//powerUp2->SetTag("PowerUp");
	//////
	//auto powerUp3 = CreateGameObjectWithPath("Assets/Meshes/AmmunitionBlessing.fbx");
	//powerUp3->GetTransform()->SetPosition(glm::vec3(0, 3, 10));
	//powerUp3->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp3->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp3->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> AmmunitionBlessingBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> AmmunitionBlessingRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> AmmunitionBlessingNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> AmmunitionBlessingMetallic = std::make_shared<Image>();
	//AmmunitionBlessingBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//AmmunitionBlessingRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//AmmunitionBlessingNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//AmmunitionBlessingMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp3->GetComponent<MeshRenderer>()->GetMaterial()->setImage(AmmunitionBlessingBaseColor);
	//powerUp3->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(AmmunitionBlessingRoughness);
	//powerUp3->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(AmmunitionBlessingNormal);
	//powerUp3->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(AmmunitionBlessingMetallic);
	//powerUp3->AddComponent<ScriptComponent>()->LoadScript("AmmunitionBlessing");
	//powerUp3->SetTag("PowerUp");
	//
	//auto powerUp4 = CreateGameObjectWithPath("Assets/Meshes/PiercingBullets.fbx");
	//powerUp4->GetTransform()->SetPosition(glm::vec3(2, 3, 10));
	//powerUp4->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//powerUp4->GetTransform()->SetScale(glm::vec3(0.002, 0.002, 0.002));
	//powerUp4->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//powerUp4->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> PiercingBulletsBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> PiercingBulletsRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> PiercingBulletsNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> PiercingBulletsMetallic = std::make_shared<Image>();
	//PiercingBulletsBaseColor->LoadTexture("Assets/Textures/powerups_DefaultMaterial_BaseColor.png");
	//PiercingBulletsRoughness->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Roughness.png");
	//PiercingBulletsNormal->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Normal.png");
	//PiercingBulletsMetallic->LoadTexture("Assets/Textures/powerups_DefaultMaterial_Metallic.png");
	//powerUp4->GetComponent<MeshRenderer>()->GetMaterial()->setImage(PiercingBulletsBaseColor);
	//powerUp4->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(PiercingBulletsRoughness);
	//powerUp4->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(PiercingBulletsNormal);
	//powerUp4->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(PiercingBulletsMetallic);
	//powerUp4->AddComponent<ScriptComponent>()->LoadScript("PiercingBullets");
	//powerUp4->SetTag("PowerUp");
	//
	////// Test Ammunition
	//auto ShotgunShells = CreateGameObjectWithPath("Assets/Meshes/ShotgunShells.fbx");
	//ShotgunShells->GetTransform()->SetPosition(glm::vec3(0, 3, 20));
	//ShotgunShells->GetTransform()->SetScale(glm::vec3(0.008, 0.008, 0.008));
	//ShotgunShells->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//ShotgunShells->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> ShotgunShellsBaseColor = std::make_shared<Image>();
	//ShotgunShellsBaseColor->LoadTexture("Assets/Textures/ShotgunShells.png");
	//ShotgunShells->GetComponent<MeshRenderer>()->GetMaterial()->setImage(ShotgunShellsBaseColor);
	//ShotgunShells->AddComponent<ScriptComponent>()->LoadScript("ShotgunShells");
	//ShotgunShells->SetTag("Ammunition");
	//
	//auto BoltgunBullets = CreateGameObjectWithPath("Assets/Meshes/BoltgunBullets.fbx");
	//BoltgunBullets->GetTransform()->SetPosition(glm::vec3(10, 3, 20));
	//BoltgunBullets->GetTransform()->SetScale(glm::vec3(0.008, 0.008, 0.008));
	//BoltgunBullets->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//BoltgunBullets->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//std::shared_ptr<Image> BoltgunBulletsBaseColor = std::make_shared<Image>();
	//BoltgunBulletsBaseColor->LoadTexture("Assets/Textures/BoltgunBullets.png");
	//BoltgunBullets->GetComponent<MeshRenderer>()->GetMaterial()->setImage(BoltgunBulletsBaseColor);
	//BoltgunBullets->AddComponent<ScriptComponent>()->LoadScript("BoltgunBullets");
	//BoltgunBullets->SetTag("Ammunition");
	
	//Lictor
	//auto lictor = CreateGameObject("Lictor");
	//lictor->SetTag("Enemy");
	//lictor->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	//lictor->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	//lictor->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//lictor->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//lictor->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.4, 0.8, 0.4));
	//lictor->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, -1.8, -0.8));
	//auto lictorMesh = CreateGameObjectWithPath("Assets/Meshes/LictorAnimated.fbx");
	//lictorMesh->SetName(lictor->GetName() + "Mesh");
	//lictorMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//lictorMesh->GetTransform()->SetScale(glm::vec3(0.4, 0.4, 0.4));
	//ParentGameObject(*lictorMesh, *lictor);
	//lictor->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerStalker");

	/*auto lictor2 = CreateGameObject("Lictor2");
	lictor2->SetTag("Enemy");
	lictor2->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	lictor2->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	lictor2->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	lictor2->AddComponent<RigidbodyComponent>(Application->physicsModule);
	lictor2->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.4, 0.8, 0.4));
	lictor2->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, -1.8, -0.8));
	auto lictor2Mesh = CreateGameObjectWithPath("Assets/Meshes/LictorAnimated.fbx");
	lictor2Mesh->SetName(lictor2->GetName() + "Mesh");
	lictor2Mesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	lictor2Mesh->GetTransform()->SetScale(glm::vec3(0.4, 0.4, 0.4));
	ParentGameObject(*lictor2Mesh, *lictor2);
	lictor2->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerStalker");*/

	//auto lictor3 = CreateGameObject("Lictor3");
	//lictor3->SetTag("Enemy");
	//lictor3->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	//lictor3->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	//lictor3->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//lictor3->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//lictor3->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.4, 0.8, 0.4));
	//lictor3->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, -1.8, -0.8));
	//auto lictor3Mesh = CreateGameObjectWithPath("Assets/Meshes/LictorAnimated.fbx");
	//lictor3Mesh->SetName(lictor3->GetName() + "Mesh");
	//lictor3Mesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//lictor3Mesh->GetTransform()->SetScale(glm::vec3(0.4, 0.4, 0.4));
	//ParentGameObject(*lictor3Mesh, *lictor3);
	//lictor3->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerRanged");

	//auto lictor4 = CreateGameObject("Lictor4");
	//lictor4->SetTag("Enemy");
	//lictor4->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	//lictor4->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	//lictor4->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//lictor4->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//lictor4->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.4, 0.8, 0.4));
	//lictor4->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, -1.8, -0.8));
	//auto lictor4Mesh = CreateGameObjectWithPath("Assets/Meshes/LictorAnimated.fbx");
	//lictor4Mesh->SetName(lictor4->GetName() + "Mesh");
	//lictor4Mesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//lictor4Mesh->GetTransform()->SetScale(glm::vec3(0.4, 0.4, 0.4));
	//ParentGameObject(*lictor4Mesh, *lictor4);
	//lictor4->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerRanged");

	//auto lictor5 = CreateGameObject("Lictor5");
	//lictor5->SetTag("Enemy");
	//lictor5->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	//lictor5->GetComponent<Transform_Component>()->SetScale(glm::vec3(5, 5, 5));
	//lictor5->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//lictor5->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//lictor5->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.4, 0.8, 0.4));
	//lictor5->GetComponent<BoxColliderComponent>()->SetOffset(glm::vec3(0, -1.8, -0.8));
	//auto lictor5Mesh = CreateGameObjectWithPath("Assets/Meshes/LictorAnimated.fbx");
	//lictor5Mesh->SetName(lictor5->GetName() + "Mesh");
	//lictor5Mesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	//lictor5Mesh->GetTransform()->SetScale(glm::vec3(0.4, 0.4, 0.4));
	//ParentGameObject(*lictor5Mesh, *lictor5);
	//lictor5->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerRanged");

	//Hormagaunt
	auto hormagaunt = CreateGameObject("Hormagaunt");
	hormagaunt->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	hormagaunt->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	hormagaunt->AddComponent<RigidbodyComponent>(Application->physicsModule);
	hormagaunt->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	auto hormagauntMesh = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	hormagauntMesh->SetName("HormagauntMesh");
	hormagauntMesh->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	hormagauntMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0)); 
	hormagauntMesh->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	hormagauntMesh->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	ParentGameObject(*hormagauntMesh, *hormagaunt);
	hormagaunt->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	hormagaunt->SetTag("Enemy");

	//auto hormagaunt1= CreateGameObject("Hormagaunt1");
	//hormagaunt1->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	//hormagaunt1->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	//hormagaunt1->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//hormagaunt1->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//hormagaunt1->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	//auto hormagauntMesh1 = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	//hormagauntMesh1->SetName(hormagaunt1->GetName() + "Mesh");
	//hormagauntMesh1->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	//hormagauntMesh1->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//hormagauntMesh1->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	//hormagauntMesh1->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	//ParentGameObject(*hormagauntMesh1, *hormagaunt1);
	//hormagaunt1->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	//hormagaunt1->SetTag("Enemy");
	
	//auto hormagaunt2 = CreateGameObject("Hormagaunt2");
	//hormagaunt2->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	//hormagaunt2->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	//hormagaunt2->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//hormagaunt2->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//hormagaunt2->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	//auto hormagauntMesh2 = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	//hormagauntMesh2->SetName(hormagaunt2->GetName() + "Mesh");
	//hormagauntMesh2->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	//hormagauntMesh2->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//hormagauntMesh2->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	//hormagauntMesh2->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	//ParentGameObject(*hormagauntMesh2, *hormagaunt2);
	//hormagaunt2->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	//hormagaunt2->SetTag("Enemy");
	
	//auto hormagaunt3 = CreateGameObject("Hormagaunt3");
	//hormagaunt3->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	//hormagaunt3->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	//hormagaunt3->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//hormagaunt3->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//hormagaunt3->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	//auto hormagauntMesh3 = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	//hormagauntMesh3->SetName(hormagaunt3->GetName() + "Mesh");
	//hormagauntMesh3->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	//hormagauntMesh3->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//hormagauntMesh3->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	//hormagauntMesh3->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	//ParentGameObject(*hormagauntMesh3, *hormagaunt3);
	//hormagaunt3->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	//hormagaunt3->SetTag("Enemy");
	
	//auto hormagaunt4 = CreateGameObject("Hormagaunt4");
	//hormagaunt4->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	//hormagaunt4->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	//hormagaunt4->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//hormagaunt4->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//hormagaunt4->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	//auto hormagauntMesh4 = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	//hormagauntMesh4->SetName(hormagaunt4->GetName() + "Mesh");
	//hormagauntMesh4->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	//hormagauntMesh4->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//hormagauntMesh4->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	//hormagauntMesh4->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	//ParentGameObject(*hormagauntMesh4, *hormagaunt4);
	//hormagaunt4->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	//hormagaunt4->SetTag("Enemy");
	
	//auto hormagaunt5 = CreateGameObject("Hormagaunt5");
	//hormagaunt5->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 0, 10));
	//hormagaunt5->GetComponent<Transform_Component>()->SetScale(glm::vec3(2.2, 2.2, 2.2));
	//hormagaunt5->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//hormagaunt5->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//hormagaunt5->GetComponent<BoxColliderComponent>()->SetSize(glm::vec3(0.7f, 2.0f, 1.6));
	//auto hormagauntMesh5 = CreateGameObjectWithPath("Assets/Meshes/Hormagaunt.fbx");
	//hormagauntMesh5->SetName(hormagaunt5->GetName() + "Mesh");
	//hormagauntMesh5->GetTransform()->Rotate(glm::radians(90.0f), glm::dvec3(1, 0, 0));
	//hormagauntMesh5->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//hormagauntMesh5->GetTransform()->SetScale(glm::vec3(0.01, 0.01, 0.01));
	//hormagauntMesh5->AddComponent<ScriptComponent>()->LoadScript("HormagauntAnimation");
	//ParentGameObject(*hormagauntMesh5, *hormagaunt5);
	//hormagaunt5->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerMelee");
	//hormagaunt5->SetTag("Enemy");
	
	//Tyranid Warrior
	//auto tyranidWarrior = CreateGameObject("Tyranid_Warrior");
	//tyranidWarrior->SetTag("Enemy");
	//tyranidWarrior->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 4, 40));
	//tyranidWarrior->GetComponent<Transform_Component>()->SetScale(glm::vec3(1, 1, 1));
	//tyranidWarrior->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//auto tyranidWarriorMesh = CreateGameObjectWithPath("Assets/Meshes/TyranidWarrior_low.fbx");
	//tyranidWarriorMesh->SetName(tyranidWarrior->GetName() + "Mesh");
	//tyranidWarriorMesh->GetTransform()->SetPosition(glm::vec3(0, 0.65, -6));
	//tyranidWarriorMesh->GetTransform()->SetScale(glm::vec3(0.012, 0.012, 0.012));
	//ParentGameObject(*tyranidWarriorMesh, *tyranidWarrior);
	//tyranidWarrior->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerWarrior");

	//auto mawloc = CreateGameObject("Mawloc");
	//mawloc->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 10, 10));
	//mawloc->GetComponent<Transform_Component>()->SetScale(glm::vec3(2, 5, 2));
	//mawloc->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//mawloc->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//auto mawlocMesh = CreateCube("MawlocMesh");
	//ParentGameObject(*mawlocMesh, *mawloc);
	//mawloc->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerBoss");

	//auto mawlocTail = CreateGameObject("MawlocTail");
	//mawlocTail->GetComponent<Transform_Component>()->SetPosition(glm::vec3(0, 10, 10));
	//mawlocTail->GetComponent<Transform_Component>()->SetScale(glm::vec3(2, 5, 2));
	//mawlocTail->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/HormagauntMeleeAttack.wav");
	//mawlocTail->AddComponent<RigidbodyComponent>(Application->physicsModule);
	//auto mawlocTailMesh = CreateCube("MawlocTailMesh");
	//ParentGameObject(*mawlocTailMesh, *mawlocTail);
	//mawlocTail->AddComponent<ScriptComponent>()->LoadScript("EnemyControllerBossTail");

	//Floor
	auto floor = CreateCube("Floor");
	floor->GetTransform()->SetPosition(glm::vec3(0, -1, 0));
	floor->GetTransform()->SetScale(glm::vec3(50, 1, 50));
	auto floorCollider = floor->AddComponent<BoxColliderComponent>(Application->physicsModule);
	
    //auto envObj = CreateGameObject("Environment1");
    //envObj->GetTransform()->SetPosition(glm::vec3(0, -1, 0));
    //envObj->GetTransform()->SetScale(glm::vec3(1, 1, 1));
    //auto encObjScript = envObj->AddComponent<ScriptComponent>()->LoadScript("SoundEnvironmentController");
    //auto envObjCollider = envObj->AddComponent<BoxColliderComponent>(Application->physicsModule);
    //envObjCollider->SetTrigger(true); 

	//auto floorCollider = floor->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//
	
	//PLAYER BUENO CON TODOS LOS SCRIPTS PORFA NO LO BORREIS

	auto player = CreateGameObject("Player");
	player->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerPowerUp");
	player->AddComponent<ScriptComponent>()->LoadScript("Boltgun");
	player->AddComponent<ScriptComponent>()->LoadScript("Shotgun");
	player->AddComponent<ScriptComponent>()->LoadScript("GrenadeLauncher");
	player->AddComponent<ScriptComponent>()->LoadScript("Railgun");
	player->AddComponent<ScriptComponent>()->LoadScript("LaserBeam");
	player->AddComponent<ScriptComponent>()->LoadScript("EnergyBall");
	player->AddComponent<ScriptComponent>()->LoadScript("ToggleMode");
	player->AddComponent<ScriptComponent>()->LoadScript("Barrage");
	player->AddComponent<ScriptComponent>()->LoadScript("HookShot");
	player->AddComponent<ScriptComponent>()->LoadScript("ArcSnare");
	player->AddComponent<ScriptComponent>()->LoadScript("RedThirstManager");
//	
//	//player->AddComponent<SoundComponent>()->LoadSound("Assets/Audio/SFX/Weapons/Boltgun/BoltgunShot.wav", true, false);
//	
	auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/dieno zachael.fbx");
	playerMesh->SetName("playerMesh");
	playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	playerMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	ParentGameObject(*playerMesh, *player);
	playerMesh->AddComponent<ScriptComponent>()->LoadScript("PlayerAnimations");
	player->AddComponent<CapsuleColliderComponent>(Application->physicsModule);
	player->AddComponent<RigidbodyComponent>(Application->physicsModule);
	player->GetComponent<RigidbodyComponent>()->SetFreezeRotations(true);
	player->GetComponent<RigidbodyComponent>()->SetGravity(glm::vec3(0, -200, 0));
	player->GetComponent<CapsuleColliderComponent>()->SetSize(glm::vec3(1.7f, 1.1f, 1));
	player->GetComponent<CapsuleColliderComponent>()->SetOffset(glm::vec3(0, 2.1f, 0));
	//player->AddComponent<ScriptComponent>()->LoadScript("InteractionSystem");
	
	auto riffleShotFX = CreateGameObject("RiffleShotFX");
	riffleShotFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	ParentGameObject(*riffleShotFX, *player);
	riffleShotFX->AddComponent<ParticleFX>()->ApplyPreset(8);
	riffleShotFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);
	
	auto findPlayer = FindGOByName("Player");
	
	auto shotgunShotFX = CreateGameObject("ShotgunShotFX");
	shotgunShotFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	ParentGameObject(*shotgunShotFX, *findPlayer);
	shotgunShotFX->AddComponent<ParticleFX>()->ApplyPreset(20);
	shotgunShotFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);
	
	auto speedboostFX = CreateGameObject("SpeedBoostFX");
	speedboostFX->GetTransform()->SetPosition(glm::vec3(0, 3, -1));
	ParentGameObject(*speedboostFX, *player);
	speedboostFX->AddComponent<ParticleFX>()->ApplyPreset(21);
	speedboostFX->GetComponent<ParticleFX>()->SetParticleSize(2, 2);
	
	auto ammunitioBlessingFX = CreateGameObject("AmmunitionBlessingFX");
	ammunitioBlessingFX->GetTransform()->SetPosition(glm::vec3(0, 3, -1));
	ParentGameObject(*ammunitioBlessingFX, *findPlayer);
	ammunitioBlessingFX->AddComponent<ParticleFX>()->ApplyPreset(22);
	ammunitioBlessingFX->GetComponent<ParticleFX>()->SetParticleSize(2, 2);

	auto railgunShotAutoFX = CreateGameObject("RailgunShotAutoFX");
	railgunShotAutoFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	ParentGameObject(*railgunShotAutoFX, *findPlayer);
	railgunShotAutoFX->AddComponent<ParticleFX>()->ApplyPreset(13);
	railgunShotAutoFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);

	auto railgunShotSemiFX = CreateGameObject("RailgunShotSemiFX");
	railgunShotSemiFX->GetTransform()->SetPosition(glm::vec3(-0.8, 3, 0.5f));
	ParentGameObject(*railgunShotSemiFX, *findPlayer);
	railgunShotSemiFX->AddComponent<ParticleFX>()->ApplyPreset(14);
	railgunShotSemiFX->GetComponent<ParticleFX>()->SetParticleSize(3, 3);
	
	auto inactiveDashFX = CreateGameObject("InactiveDashFX");
	inactiveDashFX->GetTransform()->SetPosition(glm::vec3(0, 3, -1));
	ParentGameObject(*inactiveDashFX, *player);
	inactiveDashFX->AddComponent<ParticleFX>()->ApplyPreset(1);
	inactiveDashFX->GetComponent<ParticleFX>()->SetParticleSize(2, 2);
	
	auto walkingFX = CreateGameObject("WalkingFX");
	walkingFX->GetTransform()->SetPosition(glm::vec3(0, 0, -1));
	ParentGameObject(*walkingFX, *player);
	walkingFX->AddComponent<ParticleFX>()->ApplyPreset(1);
//	
//	//ARMAS NO TOCAR
//	
//	auto boltgun = CreateGameObject("Boltgun");
//	boltgun->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	auto boltgunMesh = CreateGameObjectWithPath("Assets/boltgun.fbx");
//	boltgunMesh->SetName("boltgunMesh");
//	boltgunMesh->GetTransform()->Rotate(glm::radians(0.0f), glm::dvec3(1, 0, 0));
//	boltgunMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
//	boltgunMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	ParentGameObject(*boltgunMesh, *boltgun);
//
//	auto shotgun = CreateGameObject("Shotgun");
//	shotgun->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	auto shotgunMesh = CreateGameObjectWithPath("Assets/shotgun.fbx");
//	shotgunMesh->SetName("shotgunMesh");
//	shotgunMesh->GetTransform()->Rotate(glm::radians(0.0f), glm::dvec3(1, 0, 0));
//	shotgunMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
//	shotgunMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	ParentGameObject(*shotgunMesh, *shotgun);
//
//	auto railgun = CreateGameObject("Railgun");
//	railgun->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	auto railgunMesh = CreateGameObjectWithPath("Assets/railgun.fbx");
//	railgunMesh->SetName("railgunMesh");
//	railgunMesh->GetTransform()->Rotate(glm::radians(0.0f), glm::dvec3(1, 0, 0));
//	railgunMesh->GetTransform()->SetScale(glm::vec3(1, 1, 1));
//	railgunMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
//	ParentGameObject(*railgunMesh, *railgun);


	//For rendering Interaction System text, remove the canvas if there is already one
	//auto canvas = CreateGameObject("Canvas");
	//canvas->AddComponent<UICanvasComponent>();
	//canvas->AddComponent<UITransformComponent>();
	//canvas->AddComponent<SoundComponent>();
	//
	//auto interactText = CreateGameObject("InteractText");
	//Application->root->ParentGameObject(*interactText, *canvas);
	//interactText->AddComponent<UIImageComponent>();
	//interactText->GetComponent<UIImageComponent>()->SetTexture("Assets/Textures/PressE.png");
	//interactText->AddComponent<UIButtonComponent>();
	//interactText->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	//interactText->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.559, 0.624, 0), glm::vec3(0.262, 0.464, 1));
	////
	////
	//auto areaText = CreateGameObject("dialogueText");
	//Application->root->ParentGameObject(*areaText, *canvas);
	//areaText->AddComponent<UIImageComponent>();
	//areaText->GetComponent<UIImageComponent>()->SetTexture("Assets/Textures/dialogueText.png");
	//areaText->AddComponent<UIButtonComponent>();
	//areaText->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	//areaText->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.559, 0.624, 0), glm::vec3(0.262, 0.464, 1));

	//floor->SetActive(false);

	/*auto collisionNextLevel = CreateGameObject("CollisionNextLevel");
	collisionNextLevel->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	collisionNextLevel->AddComponent<ScriptComponent>()->LoadScript("Scene1ToScene2");*/

	//CreateLocationBot();
	//CreateLocationSM();
	//CreateWinUI();
	//CreateLoseUI();
	//CreateGameplayUI();
	// 
	// 
	//CreateMainMenuUI();
	//CreateOptionsMenuUI();
	//CreatePauseMenuUI();

	//auto audioScene1 = CreateGameObject("AudioScene");
	//audioScene1->AddComponent<SoundComponent>()->LoadAudio("Assets/Audio/Scene1.wav");

	//auto shotgunPickUp = CreateGameObjectWithPath("Assets/shotgun.fbx");
	//shotgunPickUp->AddComponent<MeshRenderer>();
	//std::shared_ptr<Image> ShotgunBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> ShotgunRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> ShotgunNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> ShotgunMetallic = std::make_shared<Image>();
	//ShotgunBaseColor->LoadTexture("Assets/Textures/shotgun_DefaultMaterial_BaseColor.png");
	//ShotgunRoughness->LoadTexture("Assets/Textures/shotgun_DefaultMaterial_Roughness.png");
	//ShotgunNormal->LoadTexture("Assets/Textures/shotgun_DefaultMaterial_Normal.png");
	//ShotgunMetallic->LoadTexture("Assets/Textures/shotgun_DefaultMaterial_Metallic.png");
	//shotgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setImage(ShotgunBaseColor);
	//shotgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(ShotgunRoughness);
	//shotgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(ShotgunNormal);
	//shotgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(ShotgunMetallic);
	//shotgunPickUp->GetTransform()->SetPosition(glm::vec3(0, 2, 0));
	//shotgunPickUp->GetTransform()->SetScale(glm::vec3(0.003, 0.003, 0.003));
	//shotgunPickUp->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//shotgunPickUp->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//shotgunPickUp->AddComponent<ScriptComponent>()->LoadScript("ShotgunPickUp");
	//shotgunPickUp->SetTag("Weapon");

	//auto railgunPickUp = CreateGameObjectWithPath("Assets/railgun.fbx");
	//std::shared_ptr<Image> RailgunBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> RailgunRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> RailgunNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> RailgunMetallic = std::make_shared<Image>();
	//RailgunBaseColor->LoadTexture("Assets/Textures/railgun_DefaultMaterial_BaseColor.png");
	//RailgunRoughness->LoadTexture("Assets/Textures/railgun_DefaultMaterial_Roughness.png");
	//RailgunNormal->LoadTexture("Assets/Textures/railgun_DefaultMaterial_Normal.png");
	//RailgunMetallic->LoadTexture("Assets/Textures/railgun_DefaultMaterial_Metallic.png");
	//railgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setImage(RailgunBaseColor);
	//railgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(RailgunRoughness);
	//railgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(RailgunNormal);
	//railgunPickUp->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(RailgunMetallic);
	//railgunPickUp->GetTransform()->SetPosition(glm::vec3(30, 2, 0));
	//railgunPickUp->GetTransform()->SetScale(glm::vec3(0.003, 0.003, 0.003));
	//railgunPickUp->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//railgunPickUp->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//railgunPickUp->AddComponent<ScriptComponent>()->LoadScript("RailgunPickUp");
	//railgunPickUp->SetTag("Weapon");
	//
	//auto biblePagePickUp = CreateGameObjectWithPath("Assets/Meshes/BibblePage.fbx");
	//std::shared_ptr<Image> BiblePageBaseColor = std::make_shared<Image>();
	//std::shared_ptr<Image> BiblePageRoughness = std::make_shared<Image>();
	//std::shared_ptr<Image> BiblePageNormal = std::make_shared<Image>();
	//std::shared_ptr<Image> BiblePageMetallic = std::make_shared<Image>();
	//BiblePageBaseColor->LoadTexture("Assets/Textures/bible page_DefaultMaterial_BaseColor.png");
	//BiblePageRoughness->LoadTexture("Assets/Textures/bible page_DefaultMaterial_Roughness.png");
	//BiblePageNormal->LoadTexture("Assets/Textures/bible page_DefaultMaterial_Normal.png");
	//BiblePageMetallic->LoadTexture("Assets/Textures/bible page_DefaultMaterial_Metallic.png");
	//biblePagePickUp->GetComponent<MeshRenderer>()->GetMaterial()->setImage(BiblePageBaseColor);
	//biblePagePickUp->GetComponent<MeshRenderer>()->GetMaterial()->setRoughnessMap(BiblePageRoughness);
	//biblePagePickUp->GetComponent<MeshRenderer>()->GetMaterial()->setNormalMap(BiblePageNormal);
	//biblePagePickUp->GetComponent<MeshRenderer>()->GetMaterial()->setMetallicMap(BiblePageMetallic);
	//biblePagePickUp->GetTransform()->SetPosition(glm::vec3(0, 2, 0));
	//biblePagePickUp->GetTransform()->SetScale(glm::vec3(0.015, 0.015, 0.015));
	//biblePagePickUp->AddComponent<BoxColliderComponent>(Application->physicsModule);
	//biblePagePickUp->GetComponent<BoxColliderComponent>()->SetTrigger(true);
	//biblePagePickUp->AddComponent<ScriptComponent>()->LoadScript("BiblePagePickUp");
	//biblePagePickUp->SetTag("BiblePage");


#ifdef _BUILD
	Application->play = true;
	SceneManagement->currentScene->sceneState = Scene::SceneState::PLAY;
	Application->physicsModule->linkPhysicsToScene = true;
	SceneManagement->Awake();
	SceneManagement->Start();
#endif // _BUILD



    return true;
}

bool hasAddedColliders = false;	

void AddInteraction() {
	
	const auto & gameObjects = SceneManagement->GetActiveScene()->children();

	for (const auto& gameObject : gameObjects) {
		if (gameObject->GetName() == "ISE" || gameObject->GetName() == "ISE_1" || gameObject->GetName() == "ISE_2") {
			gameObject->AddComponent<ScriptComponent>()->LoadScript("Item");
			gameObject->SetTag("Interactable");
		}
	}
}



//void AddScriptComponentToExistingObj() {
//	auto gameobject = SceneManagement->FindGOByName("Lvl2Z1_Barrel1");
//	gameobject->AddComponent<ScriptComponent>()->LoadScript("DestroyEnviormentObject");
//	gameobject->SetTag("Destroyable");
//
//	auto gameobject2 = SceneManagement->FindGOByName("Lvl2Z1_Barrel2");
//	gameobject2->AddComponent<ScriptComponent>()->LoadScript("DestroyEnviormentObject");
//	gameobject2->SetTag("Destroyable");
//}

bool Root::Update(double dt)
{	
	if (!hasAddedColliders) {
		//AddScriptComponentToExistingObj();	
		hasAddedColliders = true;
		//AddInteraction();
	}

	LOG(LogType::LOG_INFO, "ResourceManager Meshes: %d", resourceManager->GetMeshCount());
	LOG(LogType::LOG_INFO, "ResourceManager Materials: %d", resourceManager->GetMaterialCount());

	//if (Application->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/DefaultScene.scene");
	//}

	//if (Application->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/Level2.scene");
	//}

	//if (Application->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/Level1.scene");
	//}
	/*if (Application->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		Tweening::Move(player.get(), glm::dvec3(10.0, 5.0, 0.0), 2.0f, Modes::EASE_IN_OUT);
	}

	if (Application->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		Tweening::Rotate(player.get(), glm::dvec3(0, 180, 0), 2.0f, Modes::EASE_IN_OUT);
	}

	if (Application->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		Tweening::Scale(player.get(), glm::dvec3(2.0, 2.0, 2.0), 2.0f, Modes::EASE_IN_OUT);
	}

	if (Application->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
		Tweening::UIMove(newGameButton.get(), glm::dvec3(1.0, 0.0, 0.0), 2.0f, Modes::EASE_IN_OUT);
	}

	if (Application->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
		Tweening::UIRotate(newGameButton.get(), glm::dvec3(0, 0.5, 0), 2.0f, Modes::EASE_IN_OUT);
	}

	if (Application->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
		Tweening::UIScale(newGameButton.get(), glm::dvec3(0.5, 0.8, 0.6), 2.0f, Modes::EASE_IN_OUT);
	}*/

	//if (Application->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/DefaultScene.scene");
	//}

	//if (Application->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/Level2.scene");
	//}

	//if (Application->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN) {
	//	Application->scene_serializer->DeSerialize("Library/Scenes/Level1.scene");
	//}

	Tweening::Update(dt);

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
    
    Application->input->ClearSelection();
	SceneManagement->RemoveGameObject(gameObject);
}

std::shared_ptr<GameObject> Root::CreateGameObject(const std::string& name)
{
    return SceneManagement->CreateGameObject(name);
}

std::shared_ptr<GameObject> Root::CreateCube(const std::string& name) 
{    
    return SceneManagement->CreateCube(name);
}

std::shared_ptr<GameObject> Root::CreateSphere(const std::string& name) 
{    
    return SceneManagement->CreateSphere(name);
}
std::shared_ptr<GameObject> Root::CreateCylinder(const std::string& name) 
{    
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

			for (auto& bone : meshImp.bonesGameObjects[i]) {
				if (!bone) continue;

				auto boneGO = Application->root->CreateGameObject(bone->GetName());
				Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
				animationComponent->GetAnimator()->AddBoneGameObject(boneGO);
				boneMap[bone->GetName()] = boneGO;
			}

			for (auto& bone : meshImp.bonesGameObjects[i]) {
				if (!bone) continue;

				auto boneGO = boneMap[bone->GetName()];
				Bone* boneTransform = meshImp.animations[0].get()->FindBone(bone->GetName());
				if (boneTransform && !boneTransform->GetParentName().empty()) {
					auto it = boneMap.find(boneTransform->GetParentName());
					if (it != boneMap.end()) {
						ParentGameObject(*boneGO, *(it->second));
					}
					else {
						ParentGameObject(*boneGO, *go);
					}
				}
				else {
					ParentGameObject(*boneGO, *go);
				}
			}
		}

		meshRenderer->GetMesh()->setBoundingBox(*meshBBox);
		std::string str = std::to_string(meshRenderer->GetMesh()->getModel()->GetID());
		meshRenderer->GetMesh()->SaveBinary(str);

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
    auto canvas = CreateGameObject("Canvas_HUD");
    canvas->AddComponent<UICanvasComponent>();
    canvas->AddComponent<UITransformComponent>();

	auto bloodbg1 = CreateGameObject("blood_bg1");
	Application->root->ParentGameObject(*bloodbg1, *canvas);
	bloodbg1->AddComponent<UIImageComponent>();
	bloodbg1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_blood_bg.png");
	bloodbg1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bloodbg1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.056, 0.887, 0), glm::vec3(0.109, 0.030, 1));

	auto bloodbg2 = CreateGameObject("blood_bg2");
	Application->root->ParentGameObject(*bloodbg2, *canvas);
	bloodbg2->AddComponent<UIImageComponent>();
	bloodbg2->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_blood_bg.png");
	bloodbg2->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bloodbg2->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.059, 0.929, 0), glm::vec3(0.059, 0.020, 1));

	auto bloodbar1 = CreateGameObject("blood_bar1");
	Application->root->ParentGameObject(*bloodbar1, *canvas);
	bloodbar1->AddComponent<UIImageComponent>();
	bloodbar1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_blood.png");
	bloodbar1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bloodbar1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.056, 0.887, 0), glm::vec3(0.109, 0.030, 1));
	
	auto bloodbar2 = CreateGameObject("blood_bar2");
	Application->root->ParentGameObject(*bloodbar2, *canvas);
	bloodbar2->AddComponent<UIImageComponent>();
	bloodbar2->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_blood.png");
	bloodbar2->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bloodbar2->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.064, 0.931, 0), glm::vec3(0.055, 0.018, 1));


	auto healthbarbg = CreateGameObject("healthbar_bg");
	Application->root->ParentGameObject(*healthbarbg, *canvas);
	healthbarbg->AddComponent<UIImageComponent>();
	healthbarbg->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_bg.png");
	healthbarbg->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	healthbarbg->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.017, 0.858, 0), glm::vec3(0.043, 0.095, 1));

	auto redthirstbar = CreateGameObject("red_thirst_bar");
	Application->root->ParentGameObject(*redthirstbar, *canvas);
	redthirstbar->AddComponent<UIImageComponent>();
	redthirstbar->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/hautoealthbar_blood.png");
	redthirstbar->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 1.0, 0));
	redthirstbar->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.019, 0.948, 0), glm::vec3(0.037, 0.080, 1));

	auto healthbar = CreateGameObject("healthbar_empty");
	Application->root->ParentGameObject(*healthbar, *canvas);
	healthbar->AddComponent<UIImageComponent>();
	healthbar->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/healthbar_empty.png");
	healthbar->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 1.0, 0));
	healthbar->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.010, 0.980, 0), glm::vec3(0.160, 0.125, 1));
	
	auto boltgunSmallF = CreateGameObject("boltgun_small_L");
	Application->root->ParentGameObject(*boltgunSmallF, *canvas);
	boltgunSmallF->AddComponent<UIImageComponent>();
	boltgunSmallF->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_boltgun_small_icon.png");
	boltgunSmallF->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	boltgunSmallF->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.923, 0.794, 0), glm::vec3(0.025, 0.050, 1));
	boltgunSmallF->SetActive(false);

	auto railgunSmallF = CreateGameObject("railgun_small_L");
	Application->root->ParentGameObject(*railgunSmallF, *canvas);
	railgunSmallF->AddComponent<UIImageComponent>();
	railgunSmallF->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_railgun_small_icon.png");
	railgunSmallF->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	railgunSmallF->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.923, 0.796, 0), glm::vec3(0.025, 0.050, 1));
	railgunSmallF->SetActive(true);

	auto shotgunSmallF = CreateGameObject("shotgun_small_L");
	Application->root->ParentGameObject(*shotgunSmallF, *canvas);
	shotgunSmallF->AddComponent<UIImageComponent>();
	shotgunSmallF->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_shotgun_small_icon.png");
	shotgunSmallF->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	shotgunSmallF->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.923, 0.793, 0), glm::vec3(0.025, 0.050, 1));
	shotgunSmallF->SetActive(false);
	
	auto boltgunSmallR = CreateGameObject("boltgun_small_R");
	Application->root->ParentGameObject(*boltgunSmallR, *canvas);
	boltgunSmallR->AddComponent<UIImageComponent>();
	boltgunSmallR->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_boltgun_small_icon.png");
	boltgunSmallR->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	boltgunSmallR->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.955, 0.794, 0), glm::vec3(0.025, 0.050, 1));
	boltgunSmallR->SetActive(false);

	auto railgunSmallR = CreateGameObject("railgun_small_R");
	Application->root->ParentGameObject(*railgunSmallR, *canvas);
	railgunSmallR->AddComponent<UIImageComponent>();
	railgunSmallR->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_railgun_small_icon.png");
	railgunSmallR->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	railgunSmallR->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.955, 0.796, 0), glm::vec3(0.025, 0.050, 1));
	railgunSmallR->SetActive(false);

	auto shotgunSmallR = CreateGameObject("shotgun_small_R");
	Application->root->ParentGameObject(*shotgunSmallR, *canvas);
	shotgunSmallR->AddComponent<UIImageComponent>();
	shotgunSmallR->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_shotgun_small_icon.png");
	shotgunSmallR->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	shotgunSmallR->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.955, 0.793, 0), glm::vec3(0.025, 0.050, 1));
	shotgunSmallR->SetActive(true);

	auto railgunIcon = CreateGameObject("railgun");
	Application->root->ParentGameObject(*railgunIcon, *canvas);
	railgunIcon->AddComponent<UIImageComponent>();
	railgunIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_railgun_icon_panel.png");
	railgunIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	railgunIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.918, 0.848, 0), glm::vec3(0.069, 0.104, 1));
	railgunIcon->SetActive(false);

	auto shotgunIcon = CreateGameObject("shotgun");
	Application->root->ParentGameObject(*shotgunIcon, *canvas);
	shotgunIcon->AddComponent<UIImageComponent>();
	shotgunIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_shotgun_icon_panel.png");
	shotgunIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	shotgunIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.916, 0.850, 0), glm::vec3(0.069, 0.104, 1));
	shotgunIcon->SetActive(false);

	auto boltgunIcon = CreateGameObject("boltgun");
	Application->root->ParentGameObject(*boltgunIcon, *canvas);
	boltgunIcon->AddComponent<UIImageComponent>();
	boltgunIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_boltgun_icon_panel.png");
	boltgunIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	boltgunIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.916, 0.850, 0), glm::vec3(0.069, 0.104, 1));
	boltgunIcon->SetActive(true);

	auto weaponSwitcher = CreateGameObject("weapon_switcher_frame");
	Application->root->ParentGameObject(*weaponSwitcher, *canvas);
	weaponSwitcher->AddComponent<UIImageComponent>();
	weaponSwitcher->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gunswitcher_frame.png");
	weaponSwitcher->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(1.0, 1.0, 0));
	weaponSwitcher->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.990, 0.980, 0), glm::vec3(0.153, 0.132, 1));
	
	auto boltgunability1 = CreateGameObject("boltgun_ability_1");
	Application->root->ParentGameObject(*boltgunability1, *canvas);
	boltgunability1->AddComponent<UIImageComponent>();
	boltgunability1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerupi_grenade_icon_1.png");
	boltgunability1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	boltgunability1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.849, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	boltgunability1->SetActive(true);

	auto boltgunability2 = CreateGameObject("boltgun_ability_2");
	Application->root->ParentGameObject(*boltgunability2, *canvas);
	boltgunability2->AddComponent<UIImageComponent>();
	boltgunability2->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_chainballs.png");
	boltgunability2->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	boltgunability2->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.876, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	boltgunability2->SetActive(true);

	auto shotgunability1 = CreateGameObject("shotgun_ability_1");
	Application->root->ParentGameObject(*shotgunability1, *canvas);
	shotgunability1->AddComponent<UIImageComponent>();
	shotgunability1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_hook.png");
	shotgunability1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	shotgunability1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.849, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	shotgunability1->SetActive(false);

	auto shotgunability2 = CreateGameObject("shotgun_ability_2");
	Application->root->ParentGameObject(*shotgunability2, *canvas);
	shotgunability2->AddComponent<UIImageComponent>();
	shotgunability2->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_explosion.png");
	shotgunability2->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	shotgunability2->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.876, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	shotgunability2->SetActive(false);

	auto railgunability1 = CreateGameObject("railgun_ability_1");
	Application->root->ParentGameObject(*railgunability1, *canvas);
	railgunability1->AddComponent<UIImageComponent>();
	railgunability1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_railgun_reload.png");
	railgunability1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	railgunability1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.849, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	railgunability1->SetActive(false);

	auto railgunability2a = CreateGameObject("railgun_ability_2a");
	Application->root->ParentGameObject(*railgunability2a, *canvas);
	railgunability2a->AddComponent<UIImageComponent>();
	railgunability2a->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_railgun_Eball.png");
	railgunability2a->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	railgunability2a->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.876, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	railgunability2a->SetActive(false);

	auto railgunability2b = CreateGameObject("railgun_ability_2b");
	Application->root->ParentGameObject(*railgunability2b, *canvas);
	railgunability2b->AddComponent<UIImageComponent>();
	railgunability2b->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_railgun_shot.png");
	railgunability2b->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	railgunability2b->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.876, 0.890, 0), glm::vec3(0.023, 0.050, 1));
	railgunability2b->SetActive(false);

	auto gunSwitcher = CreateGameObject("gun_small_frame");
	Application->root->ParentGameObject(*gunSwitcher, *canvas);
	gunSwitcher->AddComponent<UIImageComponent>();
	gunSwitcher->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/gun_weapon_frames.png");
	gunSwitcher->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	gunSwitcher->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.919, 0.792, 0), glm::vec3(0.064, 0.054, 1));

	auto frames = CreateGameObject("frames");
	Application->root->ParentGameObject(*frames, *canvas);
	frames->AddComponent<UIImageComponent>();
	frames->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/powerup_frame.png");
	frames->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	frames->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.847, 0.887, 0), glm::vec3(0.053, 0.057, 1));

	auto Lock1 = CreateGameObject("lock_L");
	Application->root->ParentGameObject(*Lock1, *canvas);
	Lock1->AddComponent<UIImageComponent>();
	Lock1->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/lock.png");
	Lock1->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	Lock1->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.923, 0.795, 0), glm::vec3(0.025, 0.050, 1));
	Lock1->SetActive(false);

	auto Lock2 = CreateGameObject("lock_R");
	Application->root->ParentGameObject(*Lock2, *canvas);
	Lock2->AddComponent<UIImageComponent>();
	Lock2->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/lock.png");
	Lock2->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	Lock2->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.955, 0.795, 0), glm::vec3(0.025, 0.050, 1));
	Lock2->SetActive(false);


	auto noDashIcon = CreateGameObject("nodash");
	Application->root->ParentGameObject(*noDashIcon, *canvas);
	noDashIcon->AddComponent<UIImageComponent>();
	noDashIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_nodash.png");
	noDashIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	noDashIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.066, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	noDashIcon->SetActive(false);

	auto msupIcon = CreateGameObject("msup");
	Application->root->ParentGameObject(*msupIcon, *canvas);
	msupIcon->AddComponent<UIImageComponent>();
	msupIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_msup.png");
	msupIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	msupIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.083, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	msupIcon->SetActive(false);

	auto noreloadIcon = CreateGameObject("noreload");
	Application->root->ParentGameObject(*noreloadIcon, *canvas);
	noreloadIcon->AddComponent<UIImageComponent>();
	noreloadIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_noreload.png");
	noreloadIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	noreloadIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.133, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	noreloadIcon->SetActive(false);

	auto defenseIcon = CreateGameObject("defenseup");
	Application->root->ParentGameObject(*defenseIcon, *canvas);
	defenseIcon->AddComponent<UIImageComponent>();
	defenseIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_defenseup.png");
	defenseIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	defenseIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.099, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	defenseIcon->SetActive(false);

	auto asupIcon = CreateGameObject("asup");
	Application->root->ParentGameObject(*asupIcon, *canvas);
	asupIcon->AddComponent<UIImageComponent>();
	asupIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_asup.png");
	asupIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	asupIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.149, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	asupIcon->SetActive(false);

	auto attackIcon = CreateGameObject("attackup");
	Application->root->ParentGameObject(*attackIcon, *canvas);
	attackIcon->AddComponent<UIImageComponent>();
	attackIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_attackup.png");
	attackIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	attackIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.116, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	attackIcon->SetActive(false);

	auto magnetIcon = CreateGameObject("magnet");
	Application->root->ParentGameObject(*magnetIcon, *canvas);
	magnetIcon->AddComponent<UIImageComponent>();
	magnetIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/status_magnet.png");
	magnetIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	magnetIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.168, 0.862, 0), glm::vec3(0.015, 0.020, 1));
	magnetIcon->SetActive(false);

	auto bookshelfIcon = CreateGameObject("bookshelf");
	Application->root->ParentGameObject(*bookshelfIcon, *canvas);
	bookshelfIcon->AddComponent<UIImageComponent>();
	bookshelfIcon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UdeI/bookshelf.png");
	bookshelfIcon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bookshelfIcon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.123, 0.927, 0), glm::vec3(0.019, 0.025, 1));

	auto bible1Icon = CreateGameObject("bible1");
	Application->root->ParentGameObject(*bible1Icon, *canvas);
	bible1Icon->AddComponent<UIImageComponent>();
	bible1Icon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/bible.png");
	bible1Icon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bible1Icon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.125, 0.929, 0), glm::vec3(0.003, 0.020, 1));

	auto bible2Icon = CreateGameObject("bible2");
	Application->root->ParentGameObject(*bible2Icon, *canvas);
	bible2Icon->AddComponent<UIImageComponent>();
	bible2Icon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/bible.png");
	bible2Icon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bible2Icon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.128, 0.929, 0), glm::vec3(0.003, 0.020, 1));

	auto bible3Icon = CreateGameObject("bible3");
	Application->root->ParentGameObject(*bible3Icon, *canvas);
	bible3Icon->AddComponent<UIImageComponent>();
	bible3Icon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/bible.png");
	bible3Icon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	bible3Icon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.131, 0.929, 0), glm::vec3(0.003, 0.020, 1));

	auto bible4Icon = CreateGameObject("bible4");
	Application->root->ParentGameObject(*bible4Icon, *canvas);
	bible4Icon->AddComponent<UIImageComponent>();
	bible4Icon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/bible.png");
	bible4Icon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bible4Icon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.134, 0.929, 0), glm::vec3(0.003, 0.020, 1));
	
	auto bible5Icon = CreateGameObject("bible5");
	Application->root->ParentGameObject(*bible5Icon, *canvas);
	bible5Icon->AddComponent<UIImageComponent>();
	bible5Icon->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/bible.png");
	bible5Icon->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.0, 0.0, 0));
	bible5Icon->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.137, 0.929, 0), glm::vec3(0.003, 0.020, 1));

	canvas->AddComponent<ScriptComponent>()->LoadScript("HUD");
}

void Root::CreateMainMenuUI()
{
    auto canvas = CreateGameObject("Canvas_Main_Menu");
    canvas->AddComponent<UICanvasComponent>();
    canvas->AddComponent<UITransformComponent>();

    auto menuImage = CreateGameObject("bg_menu");
    Application->root->ParentGameObject(*menuImage, *canvas);
    menuImage->AddComponent<UIImageComponent>();
    menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/menu_bg.png");
    menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));

	auto titleImage = CreateGameObject("title");
	Application->root->ParentGameObject(*titleImage, *canvas);
	titleImage->AddComponent<UIImageComponent>();
	titleImage->AddComponent<UITransformComponent>();
	titleImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/title.png");
	titleImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	titleImage->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.014, 0.087, 0), glm::vec3(0.387, 0.184, 1));

    auto newGameButton = CreateGameObject("new_game_button");
    Application->root->ParentGameObject(*newGameButton, *canvas);
    newGameButton->AddComponent<UIImageComponent>();
    newGameButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/menu_newGame.png");
    newGameButton->AddComponent<UIButtonComponent>();
    newGameButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    newGameButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.177, 0.361, 0), glm::vec3(0.182, 0.072, 1));

    auto continueButton = CreateGameObject("continue_button");
    Application->root->ParentGameObject(*continueButton, *canvas);
    continueButton->AddComponent<UIImageComponent>();
    continueButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/menu_continue.png");
    continueButton->AddComponent<UIButtonComponent>();
    continueButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    continueButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.177, 0.469, 0), glm::vec3(0.182, 0.072, 1));

    auto optionsButton = CreateGameObject("options_button");
    Application->root->ParentGameObject(*optionsButton, *canvas);
    optionsButton->AddComponent<UIImageComponent>();
    optionsButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/menu_options.png");
    optionsButton->AddComponent<UIButtonComponent>();
    optionsButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    optionsButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.177, 0.566, 0), glm::vec3(0.182, 0.072, 1));

    auto quitButton = CreateGameObject("exit_button");
    Application->root->ParentGameObject(*quitButton, *canvas);
    quitButton->AddComponent<UIImageComponent>();
    quitButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/menu_exit.png");
    quitButton->AddComponent<UIButtonComponent>();
    quitButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
    quitButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.177, 0.669, 0), glm::vec3(0.182, 0.072, 1));

	auto emmptyMusic = CreateGameObject("EmptyMusic");
	emmptyMusic->GetComponent<ScriptComponent>()->LoadScript("SceneAudio");

	canvas->AddComponent<ScriptComponent>()->LoadScript("MenuButtons");
}

void Root::CreatePauseMenuUI() {
	auto canvas = CreateGameObject("Canvas_PauseMenu");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();


	auto menuImage = CreateGameObject("PauseMenu");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/pause_menu.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
	
	auto resumeButton = CreateGameObject("Resume_Button");
	Application->root->ParentGameObject(*resumeButton, *canvas);
	resumeButton->AddComponent<UIImageComponent>();
	resumeButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/pause_resume.png");
	resumeButton->AddComponent<UIButtonComponent>();
	resumeButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	resumeButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.323, 0), glm::vec3(0.142, 0.083, 1));

	auto optionsButton = CreateGameObject("Options_Button");
	Application->root->ParentGameObject(*optionsButton, *canvas);
	optionsButton->AddComponent<UIImageComponent>();
	optionsButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/pause_options.png");
	optionsButton->AddComponent<UIButtonComponent>();
	optionsButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	optionsButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.427, 0), glm::vec3(0.142, 0.083, 1));

	auto mainMenuButton = CreateGameObject("MainMenu_Button");
	Application->root->ParentGameObject(*mainMenuButton, *canvas);
	mainMenuButton->AddComponent<UIImageComponent>();
	mainMenuButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/pause_mainmenu.png");
	mainMenuButton->AddComponent<UIButtonComponent>();
	mainMenuButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	mainMenuButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.529, 0), glm::vec3(0.142, 0.083, 1));

	auto quitButton = CreateGameObject("Exit_Button");
	Application->root->ParentGameObject(*quitButton, *canvas);
	quitButton->AddComponent<UIImageComponent>();
	quitButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/pause_exit.png");
	quitButton->AddComponent<UIButtonComponent>();
	quitButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	quitButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.631, 0), glm::vec3(0.142, 0.083, 1));

	canvas->AddComponent<ScriptComponent>()->LoadScript("PauseMenu");
}

void Root::CreateOptionsMenuUI() {
	
	auto canvas = CreateGameObject("Canvas_OptionsMenu");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();

	auto menuImage = CreateGameObject("Canvas_OptionsMenu_");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/options_menu.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));

	canvas->AddComponent<ScriptComponent>()->LoadScript("OptionMenu");
}

void Root::CreateWinUI() {

	auto canvas = CreateGameObject("Canvas_win_screen");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();

	auto menuImage = CreateGameObject("win_screen");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/WIN_screen.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));

	auto mainMenuButton = CreateGameObject("Menu_Button");
	Application->root->ParentGameObject(*mainMenuButton, *canvas);
	mainMenuButton->AddComponent<UIImageComponent>();
	mainMenuButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/WinMenu.png");
	mainMenuButton->AddComponent<UIButtonComponent>();
	mainMenuButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	mainMenuButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.385, 0.861, 0), glm::vec3(0.184, 0.064, 1));

	auto quitButton = CreateGameObject("Quit_Button");
	Application->root->ParentGameObject(*quitButton, *canvas);
	quitButton->AddComponent<UIImageComponent>();
	quitButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/WinQuit.png");
	quitButton->AddComponent<UIButtonComponent>();
	quitButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	quitButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.579, 0.861, 0), glm::vec3(0.184, 0.064, 1));
	
	canvas->AddComponent<ScriptComponent>()->LoadScript("WinScreen");
}

void Root::CreateLoseUI() {

	auto canvas = CreateGameObject("Canvas_lose_screen");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();

	auto menuImage = CreateGameObject("Menu_Image");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/GAME_OVER_screen.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));

	auto mainMenuButton = CreateGameObject("MM_button");
	Application->root->ParentGameObject(*mainMenuButton, *canvas);
	mainMenuButton->AddComponent<UIImageComponent>();
	mainMenuButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/game_over_MM.png");
	mainMenuButton->AddComponent<UIButtonComponent>();
	mainMenuButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	mainMenuButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.537, 0.755, 0), glm::vec3(0.175, 0.058, 1));

	auto quitButton = CreateGameObject("QUIT_button");
	Application->root->ParentGameObject(*quitButton, *canvas);
	quitButton->AddComponent<UIImageComponent>();
	quitButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/game_over_quit.png");
	quitButton->AddComponent<UIButtonComponent>();
	quitButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	quitButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.763, 0.755, 0), glm::vec3(0.181, 0.06, 1));

	auto restartButton = CreateGameObject("LLC_button");
	Application->root->ParentGameObject(*restartButton, *canvas);
	restartButton->AddComponent<UIImageComponent>();
	restartButton->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/game_over_LLC.png");
	restartButton->AddComponent<UIButtonComponent>();
	restartButton->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0.5, 0.5, 0));
	restartButton->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.282, 0.755, 0), glm::vec3(0.261, 0.059, 1));

	canvas->AddComponent<ScriptComponent>()->LoadScript("LoseScreen");
}

void Root::CreateLocationSM() {

	auto canvas = CreateGameObject("Canvas_LocationSM");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();

	auto menuImage = CreateGameObject("Menu_Image");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/location_sacarium_mortis.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
}

void Root::CreateLocationBot() {

	auto canvas = CreateGameObject("Canvas_LocationBot");
	canvas->AddComponent<UICanvasComponent>();
	canvas->AddComponent<UITransformComponent>();

	auto menuImage = CreateGameObject("Menu_Image");
	Application->root->ParentGameObject(*menuImage, *canvas);
	menuImage->AddComponent<UIImageComponent>();
	menuImage->GetComponent<UIImageComponent>()->SetTexture("Library/Textures/UI/location_butcher_of_truth.png");
	menuImage->GetComponent<UITransformComponent>()->SetPivotOffset(glm::vec3(0, 0, 0));
}
void Root::SetMainCamera(std::shared_ptr<GameObject> camera)
{
	mainCamera = camera;
}

//std::shared_ptr<GameObject> Root::CreateAudioObject(const std::string& name)
//{
//    auto gameObject = CreateGameObject(name);
//    if (!gameObject) {
//        LOG(LogType::LOG_ERROR, "Failed to create audio object");
//        return nullptr;
//    }
//
//    // Add SoundComponent
//    auto soundComponent = gameObject->AddComponent<SoundComponent>(Application->audioEngine);
//    if (!soundComponent) {
//        LOG(LogType::LOG_ERROR, "Failed to add SoundComponent to audio object");
//        return nullptr;
//    }
//
//    LOG(LogType::LOG_OK, "Created audio object: %s", name.c_str());
//    return gameObject;
//}