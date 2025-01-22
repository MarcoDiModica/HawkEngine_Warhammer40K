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


bool SceneManager::Start() {
    for (shared_ptr<GameObject> object : currentScene->_children)
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
    for (auto scene : scenes) {
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

void SceneManager::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat, std::vector<Shaders> shaders)
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
    meshRenderer->GetMaterial()->SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));

    material->useShader = true;
    if (material->useShader) {
        if (!shaders.size() < 1) {
            material->SetShader(shaders[0]);
        }
    }
    meshRenderer->SetImage(image);
    //meshRenderer->GetMaterial()->SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //if (material->loadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl")) {
    //    material->useShader = true;
    //    material->bindShaders();
    //}
    //meshRenderer->SetImage(image);
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



bool SceneManager::ParentGameObjectToScene(GameObject& child) {
    //child.isSelected = false;
    //Application->input->ClearSelection();

    GameObject* currentParent = child.GetParent();

    if (currentParent) {
        currentParent->RemoveChild(&child);
    }

    currentScene->_children.push_back(child.shared_from_this());
    child.SetParent(nullptr);

    return true;
}

bool SceneManager::ParentGameObjectToObject(GameObject& child, GameObject& father) {
    //child.isSelected = false;
    //father.isSelected = false;
    //Application->input->ClearSelection();

    auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
        [&child](const std::shared_ptr<GameObject>& obj) { return obj.get() == &child; });
    if (it != currentScene->_children.end()) {
        currentScene->_children.erase(it);
    }

    child.SetParent(&father);
    child.GetTransform()->UpdateLocalMatrix(father.GetTransform()->GetMatrix());

    return true;
}


bool SceneManager::ParentGameObject(GameObject& child, GameObject& father) {

    father.AddChild(&child);

    return false;
}

std::shared_ptr<GameObject> SceneManager::FindGOByName(char* name) {
    std::string _name = std::string(name);

    for (int i = 0; i < currentScene->children().size(); ++i) {
        if (currentScene->_children[i]->GetName() == _name) {
            return currentScene->_children[i];
        }
    }
}