#include "SceneSerializer.h"
#include "App.h"
#include "Root.h"
#include "Input.h"
#include "Log.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/CameraComponent.h"
#include "MyGameEngine/LightComponent.h"
#include "MyShadersEngine/ShaderComponent.h"
#include "MyAudioEngine/SoundComponent.h"
#include "MyAudioEngine/AudioListener.h"
#include "MyPhysicsEngine/ColliderComponent.h"
#include "MyPhysicsEngine/RigidBodyComponent.h"
#include <MyPhysicsEngine/MeshColliderComponent.h>
#include <MyAnimationEngine/SkeletalAnimationComponent.h>
#include "MyUIEngine/UICanvasComponent.h"
#include "MyUIEngine/UITransformComponent.h"
#include "MyUIEngine/UIImageComponent.h"
#include "MyUIEngine/UIButtonComponent.h"
#include "MyParticlesEngine/ParticleFX.h"

SceneSerializer::SceneSerializer(App* app) : Module(app) {
}

void SceneSerializer::Serialize(const std::string& directoryPath, bool play) {
	std::shared_ptr<Scene> activeScene = Application->root->GetActiveScene();
	std::vector<std::shared_ptr<GameObject>> gameObjects = activeScene->_children;

	if (gameObjects.empty()) {
		std::string filepath = directoryPath + "/" + activeScene->GetName() + ".scene";
		std::ofstream file(filepath);
		if (file.is_open()) {
			file << "";
			file.close();
		}
		return;
	}

	YAML::Node rootNode;

	YAML::Node sceneMetaNode;
	sceneMetaNode["SceneName"] = activeScene->GetName();
	rootNode["SceneMetadata"] = sceneMetaNode;

	for (size_t i = 0; i < gameObjects.size(); ++i) {
		rootNode["GameObject" + std::to_string(i)] = SerializeGameObject(*gameObjects[i]);
	}

	std::string filepath;
	if (!play) {
		filepath = directoryPath + "/" + activeScene->GetName() + ".scene";
	}
	else {
		filepath = "EngineAssets/" + activeScene->GetName() + ".scene";
	}

	SaveToFile(rootNode, filepath);
}

YAML::Node SceneSerializer::SerializeGameObject(GameObject& gameObject) {
	YAML::Node node;

	node["name"] = gameObject.GetName();
	node["tag"] = gameObject.tag;
	node["active"] = gameObject.IsActive();
	node["isStatic"] = gameObject.isStatic;

	node["Components"] = SerializeComponents(gameObject);

	YAML::Node children;
	for (const auto& child : gameObject.GetChildren()) {
		children.push_back(SerializeGameObject(*child));
	}
	node["Children"] = children;

	return node;
}

YAML::Node SceneSerializer::SerializeComponents(GameObject& gameObject) {
	YAML::Node componentsNode;

	for (auto& [type, component] : gameObject.components) {
        if (component && component->GetType() != ComponentType::SCRIPT) {
			YAML::Node componentNode = component->encode();
			componentsNode[component->GetName()] = componentNode;
		}
	}

	// ScriptComponents 
	if (!gameObject.scriptComponents.empty()) {
		YAML::Node scriptsNode;
		for (size_t i = 0; i < gameObject.scriptComponents.size(); ++i) {
			if (gameObject.scriptComponents[i]) {
				YAML::Node scriptNode = gameObject.scriptComponents[i]->encode();
				scriptsNode["Script" + std::to_string(i)] = scriptNode;
			}
		}
		componentsNode["ScriptComponents"] = scriptsNode;
	}

	return componentsNode;
}

void SceneSerializer::SerializeChildren(YAML::Node& parentNode, GameObject& gameObject) {
	const auto& children = gameObject.GetChildren();
	YAML::Node childrenNode;

	for (size_t i = 0; i < children.size(); ++i) {
		childrenNode["Child" + std::to_string(i)] = SerializeGameObject(*children[i]);
	}

	if (!children.empty()) {
		parentNode["Children"] = childrenNode;
	}
}

bool SceneSerializer::DeSerialize(const std::string& path) {
	try {
		YAML::Node rootNode = LoadFromFile(path);

		LOG(LogType::LOG_INFO, "Deserializing scene: %s", path.c_str());

		Application->root->mainCamera = nullptr;

		if (Application->root->GetActiveScene() != nullptr) {
			for (const auto& child : Application->root->GetActiveScene()->_children) {
				child->isSelected = false;
			}
			Application->input->ClearSelection();
		}

		std::string sceneName = path.substr(path.find_last_of("/\\") + 1);
		sceneName = sceneName.substr(0, sceneName.find_last_of('.'));

		if (rootNode["SceneMetadata"] && rootNode["SceneMetadata"]["SceneName"]) {
			sceneName = rootNode["SceneMetadata"]["SceneName"].as<std::string>();
		}

		LOG(LogType::LOG_INFO, "Loading Scene: %s", sceneName.c_str());

		if (Application->root->GetActiveScene() != nullptr) {
			Application->root->RemoveScene(Application->root->GetActiveScene()->GetName());
		}

		Application->root->CreateScene(sceneName);
		Application->root->SetActiveScene(sceneName);

		LOG(LogType::LOG_INFO, "Scene created: %s", sceneName.c_str());

		for (int i = 0; ; i++) {
			YAML::Node objectNode = rootNode["GameObject" + std::to_string(i)];
			if (!objectNode || !objectNode["name"].IsDefined()) {
				break;
			}

			auto gameObject = DeserializeGameObject(objectNode);
		}

		LOG(LogType::LOG_INFO, "Scene deserialized successfully: %s", sceneName.c_str());
		Application->root->UpdateCameraPriority();
		return true;
	}
	catch (const YAML::Exception& e) {
		LOG(LogType::LOG_ERROR, "YAML Exception during deserialization: %s", e.what());
		Application->root->UpdateCameraPriority();
		return false;
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Exception during deserialization: %s", e.what());
		Application->root->UpdateCameraPriority();
		return false;
	}

	
	//Application->root->GetActiveScene()->Start();
}

std::shared_ptr<GameObject> SceneSerializer::DeserializeGameObject(const YAML::Node& node) {
	if (!node["name"].IsDefined()) {
		LOG(LogType::LOG_ERROR, "GameObject node doesn't have a name defined!");
		return Application->root->CreateGameObject("Error_MissingName");
	}

	auto objectName = node["name"].as<std::string>();
	std::shared_ptr<GameObject> gameObject = Application->root->CreateGameObject(objectName);

	if (node["tag"].IsDefined()) {
		gameObject->tag = node["tag"].as<std::string>();
	}

	if (node["active"].IsDefined()) {
		gameObject->SetActive(node["active"].as<bool>());
	}

	if (node["isStatic"].IsDefined()) {
		gameObject->isStatic = node["isStatic"].as<bool>();
	}

	if (node["Components"].IsDefined()) {
		DeserializeComponents(gameObject.get(), node["Components"]);
	}

	if (node["Children"].IsDefined()) {
		DeserializeChildren(gameObject.get(), node["Children"]);
	}

	return gameObject;
}

void SceneSerializer::DeserializeComponents(GameObject* gameObject, const YAML::Node& componentsNode) {
	for (YAML::const_iterator it = componentsNode.begin(); it != componentsNode.end(); ++it) {
		const std::string& componentName = it->first.as<std::string>();
		const YAML::Node& componentData = it->second;

		if (componentName == "ScriptComponents") {
			continue;
		}

		if (componentName == "Transform_Component") {
			gameObject->GetTransform()->decode(componentData);
		}
		else if (componentName == "MeshRenderer") {
			MeshRenderer* meshRenderer = nullptr;
			if (gameObject->HasComponent<MeshRenderer>()) {
				meshRenderer = gameObject->GetComponent<MeshRenderer>();
			}
			else {
				meshRenderer = gameObject->AddComponent<MeshRenderer>();
			}

			meshRenderer->decode(componentData);
		}
		else if (componentName == "CameraComponent") {
			auto camera = gameObject->AddComponent<CameraComponent>();
			camera->decode(componentData);
			Application->root->UpdateCameraPriority();
		}
		else if (componentName == "LightComponent") {
			auto light = gameObject->AddComponent<LightComponent>();
			light->decode(componentData);
		}
		else if (componentName == "ShaderComponent") {
			auto shader = gameObject->AddComponent<ShaderComponent>();
			shader->decode(componentData);
		}
		else if (componentName == "SoundComponent") {
			auto sound = gameObject->AddComponent<SoundComponent>();
			sound->decode(componentData);
		}
		else if (componentName == "AudioListener") {
			auto listener = gameObject->AddComponent<AudioListener>();
			listener->decode(componentData);
		}
		else if (componentName == "RigidbodyComponent") {
			auto rb = gameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
			rb->decode(componentData);
		}
		else if (componentName == "ColliderComponent") {
			auto collider = gameObject->AddComponent<ColliderComponent>(Application->physicsModule);
			collider->decode(componentData);
		}
		else if (componentName == "MeshColliderComponent") {
			auto meshCollider = gameObject->AddComponent<MeshColliderComponent>(Application->physicsModule);
			meshCollider->decode(componentData);
		}
		else if (componentName == "SkeletalAnimationComponent") {
			auto skeletalComponent = gameObject->AddComponent<SkeletalAnimationComponent>();
			skeletalComponent->decode(componentData);
			
		}
		else if (componentName == "UITransformComponent") {
			if (gameObject->HasComponent<UITransformComponent>()) {
				auto uiTransform = gameObject->GetComponent<UITransformComponent>();
				uiTransform->decode(componentData);
			}
			else
			{
				auto uiTransform = gameObject->AddComponent<UITransformComponent>();
				uiTransform->decode(componentData);
			}
		}
		else if (componentName == "UICanvasComponent") {
			auto uiTransform = gameObject->AddComponent<UITransformComponent>();
			auto canvas = gameObject->AddComponent<UICanvasComponent>();
			canvas->decode(componentData);
		}
		else if (componentName == "UIImageComponent") {
			auto image = gameObject->AddComponent<UIImageComponent>();
			image->decode(componentData);
		}
		else if (componentName == "UIButtonComponent") {
			auto button = gameObject->AddComponent<UIButtonComponent>();
			button->decode(componentData);
		}
		else if (componentName == "ParticleFX") {
			auto particle = gameObject->AddComponent<ParticleFX>();
			particle->decode(componentData);
		}
		//mas componentes aqui
		else {
			LOG(LogType::LOG_WARNING, "Unknown component type: %s", componentName.c_str());
		}
	}

	if (componentsNode["ScriptComponents"] && componentsNode["ScriptComponents"].IsMap()) {
		for (YAML::const_iterator scriptIt = componentsNode["ScriptComponents"].begin();
			scriptIt != componentsNode["ScriptComponents"].end(); ++scriptIt) {

			const YAML::Node& scriptNode = scriptIt->second;
			auto scriptComponent = gameObject->AddComponent<ScriptComponent>();
			if (!scriptComponent->decode(scriptNode)) {
				LOG(LogType::LOG_WARNING, "Failed to load script: %s",
					scriptNode["name"].IsDefined() ? scriptNode["name"].as<std::string>().c_str() : "unknown");
			}
		}
	}
}

void SceneSerializer::DeserializeChildren(GameObject* parentGameObject, const YAML::Node& childrenNode) {
	if (!childrenNode.IsSequence()) {
		LOG(LogType::LOG_ERROR, "Children node is not a sequence");
		return;
	}

	for (const auto& childNode : childrenNode) {
		if (!childNode.IsDefined()) {
			LOG(LogType::LOG_WARNING, "Child node is not defined");
			continue;
		}

		auto childObject = DeserializeGameObject(childNode);
		if (childObject) {
			parentGameObject->AddChild(childObject.get());
		}
		else {
			LOG(LogType::LOG_WARNING, "Failed to deserialize child object");
		}
	}
}

void SceneSerializer::SaveToFile(const YAML::Node& root, const std::string& filepath) {
	try {
		std::filesystem::path p(filepath);
		std::filesystem::create_directories(p.parent_path());

		std::ofstream file(filepath);
		if (file.is_open()) {
			YAML::Emitter emitter;
			emitter << root;
			file << emitter.c_str();
			file.close();
			LOG(LogType::LOG_INFO, "Scene saved successfully: %s", filepath.c_str());
		}
		else {
			LOG(LogType::LOG_ERROR, "Could not open file to save the scene: %s", filepath.c_str());
		}
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error saving the scene: %s", e.what());
	}
}

YAML::Node SceneSerializer::LoadFromFile(const std::string& filepath) {
	try {
		YAML::Node root = YAML::LoadFile(filepath);
		if (root.IsNull()) {
			LOG(LogType::LOG_ERROR, "Error loading the scene: file is empty or invalid: %s", filepath.c_str());
			throw std::runtime_error("Invalid or empty YAML file");
		}
		return root;
	}
	catch (const YAML::Exception& e) {
		LOG(LogType::LOG_ERROR, "YAML Exception loading file %s: %s", filepath.c_str(), e.what());
		throw;
	}
}

std::string SceneSerializer::GetComponentTypeName(ComponentType type) {
	switch (type) {
	case ComponentType::TRANSFORM: return "Transform_Component";
	case ComponentType::MESH_RENDERER: return "MeshRenderer";
	case ComponentType::CAMERA: return "CameraComponent";
	case ComponentType::LIGHT: return "LightComponent";
	case ComponentType::SHADER: return "ShaderComponent";
	case ComponentType::AUDIO: return "SoundComponent";
	case ComponentType::AUDIO_LISTENER: return "AudioListener";
	case ComponentType::COLLIDER: return "ColliderComponent";
	case ComponentType::RIGIDBODY: return "RigidbodyComponent";
	case ComponentType::UITRANSFORM: return "UITransformComponent";
	case ComponentType::CANVAS: return "UICanvasComponent";
	case ComponentType::IMAGE: return "UIImageComponent";
	case ComponentType::BUTTON: return "UIButtonComponent";
	//mas casos/componentes
	default: return "Unknown";
	}
}

ComponentType SceneSerializer::GetComponentTypeFromName(const std::string& name) {
	if (name == "Transform_Component") return ComponentType::TRANSFORM;
	if (name == "MeshRenderer") return ComponentType::MESH_RENDERER;
	if (name == "CameraComponent") return ComponentType::CAMERA;
	if (name == "LightComponent") return ComponentType::LIGHT;
	if (name == "ShaderComponent") return ComponentType::SHADER;
	if (name == "SoundComponent") return ComponentType::AUDIO;
	if (name == "AudioListener") return ComponentType::AUDIO_LISTENER;
	if (name == "ColliderComponent") return ComponentType::COLLIDER;
	if (name == "RigidbodyComponent") return ComponentType::RIGIDBODY;
	if (name == "UITransformComponent") return ComponentType::UITRANSFORM;
	if (name == "UICanvasComponent") return ComponentType::CANVAS;
	if (name == "UIImageComponent") return ComponentType::IMAGE;
	if (name == "UIButtonComponent") return ComponentType::BUTTON;
	//mas mapeos aqui
	return ComponentType::NONE;
}