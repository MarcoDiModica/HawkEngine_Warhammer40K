#pragma once
#include "Module.h"
#include "MyGameEngine/GameObject.h"
#include <list>



class Root : public Module
{
public:


    Root(App* app);

    virtual ~Root() = default;

    bool Awake();

    bool Start() { return true; }

    bool PreUpdate() { return true; }
    bool Update(double dt);
    bool PostUpdate() { return true; }

    bool CleanUp() { return true; }

    std::shared_ptr<GameObject> CreateMeshObject(std::string path);

    std::shared_ptr<GameObject> CreateGameObject(std::string name, bool as_child) {


        std::shared_ptr<GameObject> object = make_shared<GameObject>(name);

        if (!as_child) {
            children.push_back(object);
        }

        return object;

    }

public:

	std::list< std::shared_ptr<GameObject> > children;


};

