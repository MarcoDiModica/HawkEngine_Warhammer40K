#define GLM_ENABLE_EXPERIMENTAL
#include "SceneManager.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "Mesh.h"
#include "Scene.h"
#include "Image.h"
#include "Material.h"
#include "ModelImporter.h"
#include <string>

#include "../MyScriptingEngine/ScriptComponent.h"
#include "../MyGameEditor/Log.h"
#include "../MyAudioEngine/AudioListener.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "glm/gtx/matrix_decompose.inl"


bool SceneManager::Start() {
    for (const shared_ptr<GameObject>& object : currentScene->_children)
    {
        object->Start();

        if (object->HasComponent<CameraComponent>()) {
            mainCamera = object;
        }
    }

    return true;
}

bool SceneManager::Update(double dt) {
    currentScene->DebugDrawTree();

    

    currentScene->Update(static_cast<float>(dt));
    return true;
}

void SceneManager::CreateScene(const std::string& name)
{
    auto scene = make_shared<Scene>(name);
    scenes.push_back(scene);
}

void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    scenes.push_back(scene);

}

void SceneManager::RemoveScene(const std::string& name)
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

void SceneManager::SetActiveScene(const std::string& name)
{
    for (const auto& scene : scenes) {
        if (scene->name == name) {
            currentScene = scene;
            currentScene->Start();
            return;
        }
    }
}

std::shared_ptr<Scene> SceneManager::GetActiveScene() const
{
    return currentScene;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshObject(string name, shared_ptr<Mesh> mesh)
{
    auto go = CreateGameObject(name);
    AddMeshRenderer(*go, mesh, "Assets/default.png");

    return nullptr;
}

void SceneManager::RemoveGameObject(GameObject* gameObject) {
    if (!gameObject) {
        LOG(LogType::LOG_ERROR, "Error: Is have tried erased a GameObject null.");
        return;
    }
    if (!gameObject->GetParent()) {
        auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
            [gameObject](const auto& child) { return child.get() == gameObject; });

        if (it == currentScene->_children.end()) {
            LOG(LogType::LOG_ERROR, "Error: The GameObject not is find on the scene.");
            return;
        }


        try {
            if ((*it)->isSelected) {
               /* (*it)->isSelected = false;
                Application->input->ClearSelection();*/
            }
            (*it)->Destroy();
            it = currentScene->_children.erase(it);
        }
        catch (const std::exception& e) {
            LOG(LogType::LOG_ERROR, "Error to destroying the GameObject: %s", e.what());
        }
    }
    else {
       /* if (gameObject->isSelected) { Application->input->ClearSelection(); }*/
        //currentScene->AddGameObject(gameObject->shared_from_this());
        gameObject->GetParent()->RemoveChild(gameObject);
        gameObject->Destroy();
    }
}

std::shared_ptr<GameObject> SceneManager::CreateGameObject(const std::string& name)
{
    std::string uniqueName = name;
    int counter = 1;
    for (const std::shared_ptr<GameObject>& child : currentScene->_children) {
        if (child->GetName() == uniqueName) {
            uniqueName = name + "_" + std::to_string(counter++);
        }
    }

    auto gameObject = std::make_shared<GameObject>(uniqueName);
    currentScene->AddGameObject(gameObject);
    //currentScene->_children.push_back(gameObject);
    return gameObject;
}

std::shared_ptr<GameObject> SceneManager::CreateCube(const std::string& name) {
    auto cube = CreateGameObject(name);
    AddMeshRenderer(*cube, Mesh::CreateCube(), "Assets/default.png");
    return cube;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere(const std::string& name) {
    auto sphere = CreateGameObject(name);
    AddMeshRenderer(*sphere, Mesh::CreateSphere(), "Assets/default.png");
    return sphere;
}
std::shared_ptr<GameObject> SceneManager::CreateCylinder(const std::string& name) {
    auto cylinder = CreateGameObject(name);
    AddMeshRenderer(*cylinder, Mesh::CreateCylinder(), "Assets/default.png");
    return cylinder;
}
std::shared_ptr<GameObject> SceneManager::CreatePlane(const std::string& name) {
    auto plane = CreateGameObject(name);
    AddMeshRenderer(*plane, Mesh::CreatePlane(), "Assets/default.png");
    return plane;
}

std::shared_ptr<GameObject> SceneManager::CreateCameraObject(const std::string& name) {
    auto camera = CreateGameObject(name);
    camera->AddComponent<CameraComponent>();
    camera->AddComponent<AudioListener>();
    return camera;
}

std::shared_ptr<GameObject> SceneManager::CreateLightObject(const std::string& name) {
    auto light = CreateGameObject(name);
    light->AddComponent<LightComponent>();
    return light;
}

void SceneManager::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat)
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
    meshRenderer->GetMaterial()->SetColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

    auto shaderComponent = go.AddComponent<ShaderComponent>();
	shaderComponent->SetOwnerMaterial(meshRenderer->GetMaterial().get());
	shaderComponent->SetShaderType(ShaderType::PBR);

    meshRenderer->SetImage(image);

}

//void SceneManager::CreateScene(const std::string& name)
//{
//    auto scene = make_shared<Scene>(name);
//    scenes.push_back(scene);
//}
//
//void SceneManager::AddScene(std::shared_ptr<Scene> scene)
//{
//    scenes.push_back(scene);
//
//}
//
//void SceneManager::RemoveScene(const std::string& name)
//{
//    for (auto it = scenes.begin(); it != scenes.end(); ) {
//        if ((*it)->name == name) {
//            (*it)->Destroy();
//            it = scenes.erase(it);
//            return;
//        }
//        else {
//            ++it;
//        }
//    }
//}
//
//void SceneManager::SetActiveScene(const std::string& name)
//{
//    for (auto scene : scenes) {
//        if (scene->name == name) {
//            currentScene = scene;
//            currentScene->Start();
//            return;
//        }
//    }
//}

bool SceneManager::ParentGameObjectPreserve(GameObject& child, GameObject& father) {
	if (&child == &father) return false;

	auto childTransform = child.GetTransform();
	glm::dmat4 worldMatrix = childTransform->GetMatrix();

	father.AddChild(&child);

	glm::dvec3 worldPosition = glm::dvec3(worldMatrix[3]);

	childTransform->SetPosition(worldPosition);

	glm::dmat4 parentWorldMatrix = father.GetTransform()->GetMatrix();
	glm::dmat4 localMatrix = glm::inverse(parentWorldMatrix) * worldMatrix;

	glm::dvec3 skew;
	glm::dvec4 perspective;
	glm::dvec3 localScale;
	glm::dquat localRotation;
	glm::dvec3 localPosition;

	glm::decompose(localMatrix, localScale, localRotation, localPosition, skew, perspective);

	childTransform->SetLocalPosition(localPosition);
	childTransform->SetRotationQuat(localRotation);
	childTransform->SetScale(localScale);

	return true;
}


bool SceneManager::ParentGameObject(GameObject& child, GameObject& father) {

    father.AddChild(&child);

    return false;
}

std::shared_ptr<GameObject> SceneManager::FindGOByName(std::string name) const {
    return FindGOByNameRecursive(name, currentScene->_children);
}

std::shared_ptr<GameObject> SceneManager::FindGOByNameRecursive(const std::string& name, const std::vector<std::shared_ptr<GameObject>>& gameObjects) const {
    for (const auto& go : gameObjects) {
        if (go->GetName() == name) {
            return go;
        }
        auto found = FindGOByNameRecursive(name, go->GetChildren());
        if (found) {
            return found;
        }
    }
    return nullptr;
}
