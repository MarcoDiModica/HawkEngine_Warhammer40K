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

class GameObject;

Root::Root(App* app) : Module(app) { ; }

bool Root::Awake()
{
    SceneManagement = new SceneManager();
    Application->root->CreateScene("DefaultScene");
    Application->root->SetActiveScene("DefaultScene");
    
	SoundComponent::InitSharedAudioEngine();
	ShaderManager::GetInstance().Initialize();

   /*CreateGameObjectWithPath("Assets/Meshes/Street2.FBX");
    MakeSmokerEmmiter();
    MakeSmokerEmiter2();*/

	/*environment = CreateGameObjectWithPath("Assets/Meshes/environmentSplit.fbx");
	environment->GetTransform()->SetPosition(glm::vec3(0, -34, 0));
	environment->GetTransform()->SetScale(glm::vec3(1, 1, 1));*/

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
	player->AddComponent<RigidbodyComponent>(Application->physicsModule);
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerController");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerDash");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerInput");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerMovement");
	player->AddComponent<ScriptComponent>()->LoadScript("PlayerShooting");
	player->AddComponent<SoundComponent>()->LoadAudio("Library/Audio/Menu Confirm.wav", true);

	auto playerMesh = CreateGameObjectWithPath("Assets/Meshes/MainCharacterAnimated.fbx");
	playerMesh->SetName("playerMesh");
	playerMesh->GetTransform()->Rotate(glm::radians(-90.0f), glm::dvec3(1, 0, 0));
	ParentGameObject(*playerMesh, *player);
	playerMesh->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	playerMesh->AddComponent<ScriptComponent>()->LoadScript("PlayerAnimations");
	if (playerMesh->HasComponent<SkeletalAnimationComponent>()) {
		LOG(LogType::LOG_INFO, "Player has SkeletalAnimationComponent");
	}
	else
	{
		LOG(LogType::LOG_ERROR, "Player does not have SkeletalAnimationComponent");
	}
		
    auto objMainCamera = CreateCameraObject("MainCamera");
    objMainCamera->GetTransform()->SetPosition(glm::dvec3(0, 20.0f, -14.0f));
    objMainCamera->GetTransform()->Rotate(glm::radians(60.0f), glm::dvec3(1, 0, 0));
    auto camera = objMainCamera->AddComponent<CameraComponent>();
    objMainCamera->AddComponent<ScriptComponent>()->LoadScript("PlayerCamera");
    mainCamera = objMainCamera;

	auto particleFX = CreateGameObject("ParticleFX");
	auto emitter = particleFX->AddComponent<ParticleFX>();
	emitter->ConfigureSmoke();
	emitter->SetTexture("Baker_house.png");
  
	/*auto canvas = CreateGameObject("Canvas");
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
	button->GetComponent<UITransformComponent>()->SetTransform(glm::vec3(0.5, 0.5, 0), glm::vec3(0.180, 0.150, 1));*/

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

    //testing the serialization of scripts
	/*auto cube = CreateCube("Cube");
	cube->AddComponent<ScriptComponent>()->LoadScript("Test1");*/
	
    SceneManagement->Start();

    return true;
}

bool Root::Update(double dt) 
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