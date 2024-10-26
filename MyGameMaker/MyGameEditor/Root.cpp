#include "Root.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Material.h"
#include <iostream>

using namespace std;

Root::Root(App* app) : Module(app) { ; }

bool  Root::Awake() { 
    
    auto gameObject = CreateGameObject("OdioSonic");

    auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

    auto material = std::make_shared<Material>();
    material->LoadTexture("Baker_house.png");

    auto mesh = std::make_shared<Mesh>();
    glm::vec3 vertices[] = {
        {-0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {0.0f,  0.5f, 0.0f}
    };
    unsigned int indices[] = { 0, 1, 2 };
    mesh->Load(vertices, 3, indices, 3);

    meshRenderer->SetMaterial(material);
    meshRenderer->SetMesh(mesh);

    gameObject->transform->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));

    //CreateGameObject("Pablo" , false);
    //CreateGameObject("Pau" , false)->AddChild(CreateGameObject("PauJr" , true));
    //CreateGameObject("Sonic", false);

    return true;
}

bool Root::Update(double dt) { 

    for (shared_ptr<GameObject> object : children) 
    {
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

void Root::RemoveGameObject(std::string name) {
    for (auto it = children.begin(); it != children.end(); ) {
        if ((*it)->GetName() == name) {
            (*it)->Destroy();  // Call Destroy on the object.
            it = children.erase(it); // Erase returns the next iterator.
            return; // Exit after removing the object.
        }
        else {
            ++it; // Move to the next element if not removed.
        }
    }
}

shared_ptr<GameObject> Root::CreateGameObject(string name, bool as_child) {
	shared_ptr<GameObject> object = make_shared<GameObject>(name);

	if (!as_child) {
		children.push_back(object);
	}

	return object;
}