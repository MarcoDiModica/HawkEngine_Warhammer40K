#include "Root.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include <iostream>

using namespace std;
GameObject Scene;

Root::Root(App* app) : Module(app) { ; }

bool  Root::Awake() { 
    
    MarcoPresidente = CreateGameObject("MarcoPresidente", false);
    MarcoPresidente->GetTransform()->GetPosition() = vec3(3, 0, 0);
    MarcoPresidente->GetTransform()->Scale(vec3(0.3f, 0.3f, 0.3f));
    auto meshRenderer = MarcoPresidente->AddComponent<MeshRenderer>();
    auto mesh = make_shared<Mesh>();
    auto image = make_shared<Image>();
    auto material = make_shared<Material>();
    mesh->LoadMesh("BakerHouse.fbx");
    image->LoadTexture("Baker_house.png");
    material->setImage(image);
    meshRenderer->SetMesh(mesh);
    meshRenderer->SetMaterial(material);

    MarcoVicePresidente = CreateGameObject("MarcoVicePresidente", false);
    MarcoVicePresidente->GetTransform()->GetPosition() = vec3(-3, 0, 0);
    auto meshRenderer2 = MarcoVicePresidente->AddComponent<MeshRenderer>();
    auto mesh2 = make_shared<Mesh>();
    auto image2 = make_shared<Image>();
    auto material2 = make_shared<Material>();
    mesh2->LoadMesh("BakerHouse.fbx");
    image2->LoadTexture("Baker_house2.png");
    material2->setImage(image2);
    meshRenderer2->SetMesh(mesh2);
    meshRenderer2->SetMaterial(material2);

    return true;
}

bool Root::Update(double dt) { 

    /*for (shared_ptr<GameObject> object : children) 
    {
        object->Update(dt);
    }*/

    //MarcoPresidente->GetTransform()->Rotate(0.01f, vec3(0, 1, 0));
    //MarcoVicePresidente->GetTransform()->Rotate(0.01f, vec3(0, -1, 0));

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