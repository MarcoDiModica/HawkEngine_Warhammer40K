#include "Root.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Mesh.h"
#include <iostream>

using namespace std;

Root::Root(App* app) : Module(app) { ; }

bool  Root::Awake() { 
    



    CreateGameObject("Pablo" , false);
    CreateGameObject("Pau" , false)->AddChild(CreateGameObject("PauJr" , true));
    CreateGameObject("Sonic", false);

    return true; 


}

bool Root::Update(double dt) { 

    for (shared_ptr<GameObject> object : children) {

        object->Update(dt);

    }



    return true; 


}

shared_ptr<GameObject> Root::CreateMeshObject(string name, shared_ptr<Mesh> mesh)
{
   
    auto object = CreateGameObject(name, false);

    object->AddComponent<MeshRenderer>();

    auto meshRenderer = object->GetComponent<MeshRenderer>();

    // Load Mesh

    meshRenderer->SetMesh( mesh);


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
