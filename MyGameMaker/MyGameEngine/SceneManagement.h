#pragma once
#include <vector>
#include <memory>
#include "Scene.h"

class SceneManagement 
{
public:
    SceneManagement();

    void CreateScene(const std::string& name);
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(const std::string& name);
    void SetActiveScene(const std::string& name);
    std::shared_ptr<Scene> GetActiveScene() const;

    //void Awake();
    void Start();
    //void FixedUpdate(float fixedDeltaTime);
    void Update(float deltaTime);
    //void LateUpdate(float deltaTime);
    void Destroy();

private:
    std::vector<std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> activeScene;
};