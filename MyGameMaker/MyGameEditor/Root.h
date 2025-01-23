
#ifndef ROOT_H
#define ROOT_H

#pragma once
#include "Module.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Input.h"
#include "../MyGameEngine/readOnlyView.h"
#include "SceneSerializer.h"
#include <list>

class Material;

class Root : public Module
{
public:
    Root(App* app);

    virtual ~Root() = default;

    bool Awake();

    bool Start();

    bool PreUpdate() { return true; }
    bool Update(double dt);
    bool PostUpdate() { return true; }

    bool CleanUp() { return true; }

    void CreateScene(const std::string& name);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(const std::string& name);
    void SetActiveScene(const std::string& name);


    bool ParentGameObject(GameObject& child, GameObject& father);

    std::shared_ptr<Scene> GetActiveScene() const;

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    std::shared_ptr<GameObject> CreateCube(const std::string& name);
    std::shared_ptr<GameObject> CreateSphere(const std::string& name);
    std::shared_ptr<GameObject> CreatePlane(const std::string& name);
    std::shared_ptr<GameObject> CreateMeshObject(std::string name, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<GameObject> CreateCameraObject(const std::string& name);
    std::shared_ptr<GameObject> CreateLightObject(const std::string& name);
    std::shared_ptr<GameObject> CreateCanvasObject(const std::string& name);
    std::shared_ptr<GameObject> CreateButtonObject(const std::string& name);
    std::shared_ptr<GameObject> CreateImageObject(const std::string& name, const std::string& imagePath="Assets/crosshair.png");

    void AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath = "default.png", std::shared_ptr<Material> mat = nullptr);

    

    void RemoveGameObject(GameObject* gameObject);

    bool ParentGameObjectToScene(GameObject& child);
    bool ParentGameObjectToObject(GameObject& child, GameObject& father);

    std::shared_ptr<Scene> currentScene = nullptr;

    //main camera
    std::shared_ptr<GameObject> mainCamera = nullptr;

    friend SceneSerializer;

private:
    std::vector<std::shared_ptr<Scene>> scenes;
};

#endif