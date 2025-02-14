#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#pragma once

#include "Mesh.h"
#include "GameObject.h"
#include "Scene.h"
#include "readOnlyView.h"
#include "Material.h"

#include <list>
#include <string>
#include <memory>
class SceneManager
{
public:

    SceneManager() {
        int i = 8;
    }

    bool Start();
    bool Update(double dt);

    void CreateScene(const std::string& name);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(const std::string& name);
    void SetActiveScene(const std::string& name);



    std::shared_ptr<Scene> GetActiveScene() const;

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    std::shared_ptr<GameObject> CreateCube(const std::string& name);
    std::shared_ptr<GameObject> CreateSphere(const std::string& name);
    std::shared_ptr<GameObject> CreateCylinder(const std::string& name);
    std::shared_ptr<GameObject> CreatePlane(const std::string& name);
    std::shared_ptr<GameObject> CreateMeshObject(std::string name, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<GameObject> CreateCameraObject(const std::string& name);
    std::shared_ptr<GameObject> CreateLightObject(const std::string& name);

    void AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath = "default.png", std::shared_ptr<Material> mat = nullptr, std::vector<Shaders> shaders = std::vector<Shaders>());

    bool ParentGameObject(GameObject& child, GameObject& father);

    void RemoveGameObject(GameObject* gameObject);

    bool ParentGameObjectToScene(GameObject& child);
    bool ParentGameObjectToObject(GameObject& child, GameObject& father);

    std::shared_ptr<GameObject> FindGOByName(char* name);

    std::shared_ptr<Scene> currentScene = nullptr;

    //main camera
    std::shared_ptr<GameObject> mainCamera = nullptr;
private:
    std::vector<std::shared_ptr<Scene>> scenes;

};

extern SceneManager* SceneManagement;

#endif

