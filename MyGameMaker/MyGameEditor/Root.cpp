#include "Root.h"
#include <iostream>

using namespace std;

Root::Root(App* app) : Module(app) { ; }

bool  Root::Awake() { 
    


    CreateGameObject("Pau" , false)->AddChild(CreateGameObject("PauJr" , true));
    CreateGameObject("Pablo" , false);

    return true; 


}

bool Root::Update(double dt) { 

    for (shared_ptr<GameObject> object : children) {

        object->Update(dt);

    }



    return true; 


}

shared_ptr<GameObject> Root::CreateMeshObject(string path)
{
   

    

    return nullptr;
}

//shared_ptr<GameObject> Root:: CreateGameObject(string name, bool as_child) {
//
//
//    shared_ptr<GameObject> object = make_shared<GameObject>(name);
//
//    if (!as_child) {
//        children.push_back(object);
//    }
//
//    return object;
