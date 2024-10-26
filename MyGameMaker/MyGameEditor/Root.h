#pragma once
#include "Module.h"
#include "MyGameEngine/GameObject.h"
#include <list>



class Root : public Module
{
public:


    Root(App* app);

    virtual ~Root();

    bool Awake();

    bool Start();

    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();

    bool CleanUp();

    std::shared_ptr<GameObject> CreateMeshObject(std::string path);

private:

	std::list<GameObject> children;


};

