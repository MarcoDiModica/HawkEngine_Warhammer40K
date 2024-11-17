#pragma once
#include "Module.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/SceneManagement.h"
#include "MyGameEngine/Scene.h"
#include "SceneSerializer.h"
#include <list>

class Scene;

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

    std::shared_ptr<GameObject> CreateMeshObject(std::string name, std::shared_ptr<Mesh> mesh);
    void CreateCubeObject(std::string name);
    void CreateSphereObject(std::string name);
    void CreatePlaneObject(std::string name);
	void CreateEmptyObject(std::string name);

    std::shared_ptr<GameObject> CreateGameObject(std::string name, bool as_child = false);

    void AddMeshRenderer(GameObject& go, std::shared_ptr<   Mesh> mesh, const std::string& texturePath = "default.png");
    void RemoveGameObject(std::string name);

public:

    std::shared_ptr<Scene> currentScene = nullptr;
    
    //SceneManagement sceneManagement;
    //std::shared_ptr<Scene> currentScene = std::make_shared<Scene>("Scene1");

    friend SceneSerializer;

};

class Scene {
public:

    Scene(std::string SceneName = "Scene") {
        name = SceneName;
    }


    std::string name;

    std::vector< std::shared_ptr<GameObject> > children;
};