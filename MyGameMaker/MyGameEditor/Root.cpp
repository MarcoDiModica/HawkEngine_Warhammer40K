#include "Root.h"

#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/LightComponent.h"
#include "MyGameEngine/CameraComponent.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Scene.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/ModelImporter.h"
#include "MyUIEngine/CanvasComponent.h"
#include "MyUIEngine/ImageComponent.h"
#include "MyUIEngine/CheckBoxComponent.h"
#include "MyUIEngine/InputBoxComponent.h"


#include "App.h"
#include "Input.h"

#include <SDL2/SDL.h>

#include <iostream>

using namespace std;

class GameObject;

Root::Root(App* app) : Module(app) { ; }

void MakeCity() {
    Application->root->CreateScene("HolaBuenas");
    Application->root->SetActiveScene("HolaBuenas");
    auto MarcoVicePresidente = Application->root->CreateGameObject("City");

    ModelImporter meshImp;
    meshImp.loadFromFile("Assets/Meshes/Street environment_V01.FBX");

    for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
        auto MarcoVicePresidente2 = meshImp.meshGameObjects[i];

        auto go = Application->root->CreateGameObject("GameObject");
        auto color = MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->color;
        Application->root->AddMeshRenderer(*go, MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMesh(), MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->getImg()->image_path);
        go->GetComponent<MeshRenderer>()->GetMaterial()->SetColor(color);
        go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());
        Application->root->ParentGameObject(*go, *MarcoVicePresidente);
    }

    MarcoVicePresidente->GetTransform()->SetScale(vec3(0.5, 0.5, 0.5));
    MarcoVicePresidente->GetTransform()->SetPosition(vec3(0, 0.1, 0));
    MarcoVicePresidente->GetTransform()->Rotate(-1.5708, vec3(1,0,0));
}

bool Root::Awake()
{
    //Application->scene_serializer->DeSerialize("Assets/Adios.scene");
    //Application->scene_serializer->DeSerialize("Assets/HolaBuenas.scene");
    MakeCity();

    /*CreateScene("Viernes13");
    SetActiveScene("Viernes13");*/

    auto MainCamera = CreateCameraObject("MainCamera");
    MainCamera->GetTransform()->SetPosition(glm::dvec3(0, 0.5, 0));
    MainCamera->GetTransform()->Rotate(glm::radians(180.0), glm::dvec3(0, 1, 0));
    auto camera = MainCamera->AddComponent<CameraComponent>();
    mainCamera = MainCamera;

    return true;
}

bool Root::Start()
{
    /*auto Street = CreateGameObject("Street");
    Street->GetTransform()->GetPosition() = vec3(0, 0, 0);
    ModelImporter meshImp;
    meshImp.loadFromFile("Assets/Meshes/ff.fbx");

    for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
        auto& Street2 = meshImp.meshGameObjects[i];
        currentScene->AddGameObject(Street2);
        ParentGameObject(*Street2, *Street);
    }*/

    /*auto MainCamera = CreateGameObject("MainCamera");
    MainCamera->GetTransform()->GetPosition() = vec3(0, 0, -10);
    auto camera = MainCamera->AddComponent<CameraComponent>();
    mainCamera = MainCamera;

    auto cube = CreateCube("Cube");
    cube->GetTransform()->GetPosition() = vec3(0, 0, 0);
    AddMeshRenderer(*cube, Mesh::CreateCube(), "Assets/default.png");*/

    for (shared_ptr<GameObject> object : currentScene->_children)
    {
        object->Start();

        if (object->HasComponent<CameraComponent>()) {
            mainCamera = object;
		}
    }

    return true;
}

bool Root::Update(double dt) {

    //LOG(LogType::LOG_INFO, "Active Scene %s", currentScene->GetName().c_str());
    currentScene->DebugDrawTree();

    currentScene->Update(static_cast<float>(dt));

    //if (Application->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {
    //
    //    if (currentScene->tree == nullptr) {
    //        currentScene->tree = new Octree(BoundingBox(vec3(-100, -100, -100), vec3(100, 100, 100)), 10, 1);
    //        for (auto child : currentScene->children()) {
    //            currentScene->tree->Insert(currentScene->tree->root, *child, 0);
    //        }
    //
    //    }
    //    else {
    //        delete currentScene->tree;
    //        currentScene->tree = nullptr;
    //        int a = 7;
    //    }
    //}
    //
    //
    //
    ////if press 1 active scene Viernes13 and press 2 active scene Salimos
    //if (Application->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
    //    Application->scene_serializer->DeSerialize("Assets/Viernes13.scene");
	//}
    //else if (Application->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
    //    Application->scene_serializer->DeSerialize("Assets/Salimos.scene");
    //}
    

    return true;
}

shared_ptr<GameObject> Root::CreateMeshObject(string name, shared_ptr<Mesh> mesh)
{
    auto go = CreateGameObject(name);
    AddMeshRenderer(*go, mesh, "Assets/default.png");

    return nullptr;
}

void Root::RemoveGameObject(GameObject* gameObject) {
    if (!gameObject) {
        LOG(LogType::LOG_ERROR, "Error: Is have tried erased a GameObject null.");
        return;
    }
    if (! gameObject->GetParent()) {
        auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
            [gameObject](const auto& child) { return child.get() == gameObject; });

        if (it == currentScene->_children.end()) {
            LOG(LogType::LOG_ERROR, "Error: The GameObject not is find on the scene.");
            return;
        }


        try {
            if ((*it)->isSelected) {
                (*it)->isSelected = false;
                Application->input->ClearSelection();
            }
            (*it)->Destroy();
            it = currentScene->_children.erase(it);
        }
        catch (const std::exception& e) {
            LOG(LogType::LOG_ERROR, "Error to destroying the GameObject: %s", e.what());
        }
    }
    else {
        if (gameObject->isSelected) { Application->input->ClearSelection(); }
        //currentScene->AddGameObject(gameObject->shared_from_this());
        gameObject->GetParent()->RemoveChild(gameObject);
        gameObject->Destroy();
    }
}

std::shared_ptr<GameObject> Root::CreateGameObject(const std::string& name)
{
    std::string uniqueName = name;
    int counter = 1;
    for (std::shared_ptr<GameObject> child : currentScene->_children) {
        if (child->GetName() == uniqueName) {
            uniqueName = name + "_" + std::to_string(counter++);
        }
    }

    auto gameObject = std::make_shared<GameObject>(uniqueName);
    currentScene->AddGameObject(gameObject);
    //currentScene->_children.push_back(gameObject);
    return gameObject;
}

std::shared_ptr<GameObject> Root::CreateCube(const std::string& name) {
    auto cube = CreateGameObject(name);
    AddMeshRenderer(*cube, Mesh::CreateCube(), "Assets/default.png");
    return cube;
}

std::shared_ptr<GameObject> Root::CreateSphere(const std::string& name) {
    auto sphere = CreateGameObject(name);
    AddMeshRenderer(*sphere, Mesh::CreateSphere(), "Assets/default.png");
    return sphere;
}

std::shared_ptr<GameObject> Root::CreatePlane(const std::string& name) {
    auto plane = CreateGameObject(name);
    AddMeshRenderer(*plane, Mesh::CreatePlane(), "Assets/default.png");
    return plane;
}

std::shared_ptr<GameObject> Root::CreateCameraObject(const std::string& name) {
	auto camera = CreateGameObject(name);
	camera->AddComponent<CameraComponent>();
	return camera;
}

std::shared_ptr<GameObject> Root::CreateLightObject(const std::string& name) {
	auto light = CreateGameObject(name);
	light->AddComponent<LightComponent>();
	return light;
}

std::shared_ptr<GameObject> Root::CreateCanvasObject(const std::string& name) {
    auto canva = CreateGameObject(name);
    canva->AddComponent<CanvasComponent>();

    // Crear un plano vertical
    auto verticalPlane = CreatePlane("VerticalPlane");
    verticalPlane->GetTransform()->SetPosition(glm::dvec3(0, 3.5, -4)); // Asegúrate de que la posición sea correcta
    verticalPlane->GetTransform()->SetScale(glm::dvec3(6.5, 3.5, 1.0)); // Cambia los valores según sea necesario
    verticalPlane->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));

    ParentGameObject(*verticalPlane, *canva);

    // Asegúrate de que el MeshRenderer tenga un material y una textura asignados
    auto meshRenderer = verticalPlane->GetComponent<MeshRenderer>();
    if (meshRenderer) {
        auto material = std::make_shared<Material>();
        auto image = std::make_shared<Image>();
        image->LoadTexture("Assets/Default.png"); // Asegúrate de que la textura exista
        material->setImage(image);
        meshRenderer->SetMaterial(material);
    }
    else {
        std::cerr << "Error: MeshRenderer no encontrado en el plano vertical." << std::endl;
    }


    return canva;
}

std::shared_ptr<GameObject> Root::CreateButtonObject(const std::string& name) {
    auto button = CreateGameObject(name);
    button->AddComponent<CanvasComponent>(); // Assuming CanvasComponent is used for UI elements

    // Crear un plano vertical rojo
    auto verticalPlane = CreatePlane("VerticalPlane");
    verticalPlane->GetTransform()->SetPosition(glm::dvec3(0, 3.5, -3.99)); // Ajusta la posición según sea necesario
    verticalPlane->GetTransform()->SetScale(glm::dvec3(1.0, 1.0, 1.0)); // Ajusta la escala según sea necesario
    verticalPlane->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));

    // Asignar un material rojo al plano
    auto meshRenderer = verticalPlane->GetComponent<MeshRenderer>();
    if (meshRenderer) {
        auto material = std::make_shared<Material>();
        auto image = std::make_shared<Image>();
        image->LoadTexture("Assets/Button.png"); // Asegúrate de que la textura exista
        material->setImage(image);
        meshRenderer->SetMaterial(material);
    }
    else {
        std::cerr << "Error: MeshRenderer no encontrado en el plano vertical." << std::endl;
    }

    ParentGameObject(*verticalPlane, *button);


    // Additional setup for the button can be added here
    return button;
}


std::shared_ptr<GameObject> Root::CreateImageObject(const std::string& name, const std::string& imagePath) {
    
    auto imageObject = CreateGameObject("Crosshair");

    auto verticalPlane = CreatePlane("1");
    verticalPlane->GetTransform()->SetPosition(glm::dvec3(-0.75, 0, -3.99)); // Asegúrate de que la posición sea correcta
    verticalPlane->GetTransform()->SetScale(glm::dvec3(0.5, 0.2, 1.0)); // Cambia los valores según sea necesario
    verticalPlane->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));
    AddMeshRenderer(*verticalPlane, Mesh::CreatePlane(), "Assets/default.png");
    ParentGameObject(*verticalPlane, *imageObject);


    auto verticalPlane2 = CreatePlane("2");
    verticalPlane2->GetTransform()->SetPosition(glm::dvec3(0.75, 0, -3.99)); // Asegúrate de que la posición sea correcta
    verticalPlane2->GetTransform()->SetScale(glm::dvec3(0.5, 0.2, 1.0)); // Cambia los valores según sea necesario
    verticalPlane2->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));
    AddMeshRenderer(*verticalPlane2, Mesh::CreatePlane(), "Assets/default.png");
    ParentGameObject(*verticalPlane2, *imageObject);


    auto verticalPlane3 = CreatePlane("3");
    verticalPlane3->GetTransform()->SetPosition(glm::dvec3(0, -0.75, -3.99)); // Asegúrate de que la posición sea correcta
    verticalPlane3->GetTransform()->SetScale(glm::dvec3(0.2, 0.5, 1.0)); // Cambia los valores según sea necesario
    verticalPlane3->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));
    AddMeshRenderer(*verticalPlane3, Mesh::CreatePlane(), "Assets/default.png");
    ParentGameObject(*verticalPlane3, *imageObject);


    auto verticalPlane4 = CreatePlane("4");
    verticalPlane4->GetTransform()->SetPosition(glm::dvec3(0, 0.75, -3.99)); // Asegúrate de que la posición sea correcta
    verticalPlane4->GetTransform()->SetScale(glm::dvec3(0.2, 0.5, 1.0)); // Cambia los valores según sea necesario
    verticalPlane4->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));
    AddMeshRenderer(*verticalPlane4, Mesh::CreatePlane(), "Assets/default.png");
    ParentGameObject(*verticalPlane4, *imageObject);

	imageObject->GetTransform()->SetPosition(glm::dvec3(0, 1.5, 0));

    auto imageComponent = imageObject->AddComponent<ImageComponent>();
    auto image = std::make_shared<Image>();
    image->LoadTexture(imagePath);
    imageComponent->SetImage(image);

    return verticalPlane;
}

std::shared_ptr<GameObject> Root::CreateCheckBoxObject(const std::string& name) {
    // Crear un GameObject para la checkbox
    auto checkboxObject = CreateGameObject(name);

    // Agregar el componente CheckBoxComponent al objeto
    auto checkboxComponent = checkboxObject->AddComponent<CheckBoxComponent>();

    // Configurar el callback para manejar los cambios en el estado de la checkbox
    checkboxComponent->SetOnCheckedChanged([](bool isChecked) {
        std::cout << "Checkbox " << (isChecked ? "Checked" : "Unchecked") << std::endl;
        });

    // Crear un plano para representar la checkbox
    auto checkboxVisual = CreatePlane("CheckboxVisual");
    checkboxVisual->GetTransform()->SetPosition(glm::dvec3(0, 3.5, -4)); // Ajusta la posición según sea necesario
    checkboxVisual->GetTransform()->SetScale(glm::dvec3(0.5, 0.5, 1.0)); // Tamaño de la checkbox
    checkboxVisual->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));
    ParentGameObject(*checkboxVisual, *checkboxObject);

    // Asegúrate de que el MeshRenderer tenga un material y una textura asignados para representar la checkbox
    auto meshRenderer = checkboxVisual->GetComponent<MeshRenderer>();
    if (meshRenderer) {
        auto material = std::make_shared<Material>();
        auto image = std::make_shared<Image>();
        image->LoadTexture("Assets/CheckBoxEmpty.png"); // Textura para la checkbox desmarcada
        material->setImage(image);
        meshRenderer->SetMaterial(material);
    }
    else {
		
        std::cerr << "Error: MeshRenderer no encontrado en el plano visual de la checkbox." << std::endl;
    }

    // Callback para cambiar la textura según el estado de la checkbox
    checkboxComponent->SetOnCheckedChanged([checkboxVisual](bool isChecked) {
        auto meshRenderer = checkboxVisual->GetComponent<MeshRenderer>();
        if (meshRenderer) {
            auto material = std::make_shared<Material>();
            auto image = std::make_shared<Image>();
            if (isChecked) {
                image->LoadTexture("Assets/CheckBoxFull.png"); // Textura marcada
            }
            else {
                image->LoadTexture("Assets/CheckBoxEmpty.png"); // Textura desmarcada
            }
            material->setImage(image);
            meshRenderer->SetMaterial(material);
        }
        });

    return checkboxObject;
}

std::shared_ptr<GameObject> Root::CreateInputBoxObject(const std::string& name) {
    auto planeObject = CreateGameObject(name);
    planeObject->AddComponent<CanvasComponent>(); 

    // Crear un plano vertical
    auto verticalPlane = CreatePlane("VerticalPlane");
    verticalPlane->GetTransform()->SetPosition(glm::dvec3(0, 3.5, -3.99)); 
    verticalPlane->GetTransform()->SetScale(glm::dvec3(1.0, 1.0, 1.0)); 
    verticalPlane->GetTransform()->Rotate(glm::radians(90.0), glm::dvec3(1, 0, 0));

    // Asignar un material con la imagen al plano
    auto meshRenderer = verticalPlane->GetComponent<MeshRenderer>();
    if (meshRenderer) {
        auto material = std::make_shared<Material>();
        auto image = std::make_shared<Image>();
        image->LoadTexture("Assets/InputBox.png"); 
		material->setImage(image);
        meshRenderer->SetMaterial(material);
    }
    else {
		
        std::cerr << "Error: MeshRenderer no encontrado en el plano vertical." << std::endl;
    }

    ParentGameObject(*verticalPlane, *planeObject);

    // Additional setup for the plane object can be added here
    return planeObject;
}

void Root::AddMeshRenderer(GameObject& go, std::shared_ptr<Mesh> mesh, const std::string& texturePath, std::shared_ptr<Material> mat)
{
    auto meshRenderer = go.AddComponent<MeshRenderer>();
    auto image = std::make_shared<Image>();
    auto material = std::make_shared<Material>();
    
    
    meshRenderer->SetMesh(mesh);
    if (mat) {
        material = mat;
        image = mat->getImg();
    }
    else {
        image->LoadTexture(texturePath);
    }
    material->setImage(image);
    meshRenderer->SetMaterial(material);
    //meshRenderer->GetMaterial()->SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //if (material->loadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl")) {
    //    material->useShader = true;
    //    material->bindShaders();
    //}
    //meshRenderer->SetImage(image);
}

void Root::CreateScene(const std::string& name)
{
    auto scene = make_shared<Scene>( name);
    scenes.push_back(scene);
}

void Root::AddScene(std::shared_ptr<Scene> scene)
{
    scenes.push_back(scene);

}

void Root::RemoveScene(const std::string& name)
{
    for (auto it = scenes.begin(); it != scenes.end(); ) {
        if ((*it)->name == name) {
            (*it)->Destroy();
            it = scenes.erase(it);
            return;
        }
        else {
            ++it;
        }
    }
}

void Root::SetActiveScene(const std::string& name)
{
    for (auto scene : scenes) {
        if (scene->name == name) {
            currentScene = scene;
            currentScene->Start();
            return;
        }
    }
}

std::shared_ptr<Scene> Root::GetActiveScene() const
{
    return currentScene;
}

bool Root::ParentGameObjectToScene(GameObject& child) {
    child.isSelected = false;
    Application->input->ClearSelection();

    GameObject* currentParent = child.GetParent();

    if (currentParent) {
        currentParent->RemoveChild(&child);
    }

    currentScene->_children.push_back(child.shared_from_this());
    child.SetParent(nullptr);

    return true;
}

bool Root::ParentGameObjectToObject(GameObject& child, GameObject& father) {
    child.isSelected = false;
    father.isSelected = false;
    Application->input->ClearSelection();

    auto it = std::find_if(currentScene->_children.begin(), currentScene->_children.end(),
        [&child](const std::shared_ptr<GameObject>& obj) { return obj.get() == &child; });
    if (it != currentScene->_children.end()) {
        currentScene->_children.erase(it);
    }

    child.SetParent(&father);
    child.GetTransform()->UpdateLocalMatrix(father.GetTransform()->GetMatrix());

    return true;
}


bool Root::ParentGameObject(GameObject& child, GameObject& father) {

    father.AddChild(&child);

    return false;
}