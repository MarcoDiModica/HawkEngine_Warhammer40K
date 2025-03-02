#ifndef ROOT_H
#define ROOT_H

#pragma once
#include <vector>

#include "Module.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/SceneManager.h"
#include "../MyGameEngine/Shaders.h"
#include "SceneSerializer.h"

class Material;

class Root : public Module
{
public:
    Root(App* app);

    virtual ~Root() {
        free(SceneManagement);
    };

    bool Awake();

    bool Start();

    bool PreUpdate() { return true; }
    bool Update(double dt);
    bool PostUpdate() { return true; }

    bool CleanUp();

    void CreateScene(const std::string& name);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(const std::string& name);
    void SetActiveScene(const std::string& name);

    bool ParentGameObject(GameObject& child, GameObject& father);

    std::shared_ptr<Scene> GetActiveScene() const;

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    std::shared_ptr<GameObject> CreateCube(const std::string& name);
    std::shared_ptr<GameObject> CreateSphere(const std::string& name);
    std::shared_ptr<GameObject> CreateCylinder(const std::string& name);
    std::shared_ptr<GameObject> CreatePlane(const std::string& name);
    std::shared_ptr<GameObject> CreateMeshObject(std::string name, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<GameObject> CreateCameraObject(const std::string& name);
    std::shared_ptr<GameObject> CreateLightObject(const std::string& name);
    std::shared_ptr<GameObject> CreateAudioObject(const std::string& name);

    void AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath = "default.png", std::shared_ptr<Material> mat = nullptr, std::vector<Shaders> shaders = std::vector<Shaders>());

    std::shared_ptr<GameObject> CreateGameObjectWithPath(const std::string& path);

    void ChangeShader(GameObject& go, ShaderType shader);

    void RemoveGameObject(GameObject* gameObject);

    bool ParentGameObjectToScene(GameObject& child);
    bool ParentGameObjectToObject(GameObject& child, GameObject& father);

    std::shared_ptr<GameObject> FindGOByName(std::string name);

    std::shared_ptr<Scene> currentScene = nullptr;
    float emitterLifetime = 5.0f;
    //main camera
    std::shared_ptr<GameObject> mainCamera = nullptr;

    friend SceneSerializer;

    std::vector<Shaders> shaders;

private:
    std::vector<std::shared_ptr<Scene>> scenes;
};

#endif