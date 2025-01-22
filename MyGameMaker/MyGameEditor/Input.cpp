#include "MyGameEngine/Mesh.h"
#include "App.h"
#include "Log.h"
#include "MyGUI.h"
#include "imgui.h"
#include "UiSceneWindow.h"
#include "Input.h"
#include "EditorCamera.h"
#include "MyWindow.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include "../MyGameEngine/ModelImporter.h"
#include "../MyGameEngine/PhysVehicle3D.h"
#include "PhysicsModule.h"
#include <SDL2/SDL.h> // idk what to do to remove this
#include <string>
#include <iostream>
#include <filesystem>
#include "glm/glm.hpp"
#include <vector>


#define MAX_KEYS 300
#define SCREEN_SIZE 1
#define ASSETS_PATH "Assets\\"

namespace fs = std::filesystem;

Input::Input(App* app) : Module(app)
{
    keyboard = new KEY_STATE[MAX_KEYS];
    memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
    memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);

}

Input::~Input()
{
    delete[] keyboard;
}

bool Input::Awake()
{
    bool ret = true;
    LOG(LogType::LOG_INFO, "Initializing Input Events...");

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) == 0)
    {
        LOG(LogType::LOG_OK, "Init SDL Input Event subsystem");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "SDL Input Event subsystem could not be initialized! %s", SDL_GetError());
        ret = false;
    }

    LOG(LogType::LOG_OK, "-File System current path: %s", std::filesystem::current_path().string().c_str());

	camera = Application->camera;


    return ret;
}

bool Input::PreUpdate()
{
    bool ret = true;

    ret = processSDLEvents();

    return ret;
}

bool Input::Update(double dt)
{
    bool ret = true;

    return ret;
}
std::string CopyFBXFileToProject(const std::string& sourceFilePath);

void SpawnPhysCube() {
    //auto cube = Application->root->CreateCube("PhysicsCube");
    //cube->GetTransform()->SetPosition(glm::vec3(0, 10, 0));
    //Application->physicsModule->CreatePhysicsForGameObject(*cube, 1.0f); // Mass
    glm::vec3 cameraPosition = Application->camera->GetPosition(); // Reemplaza con la forma en que obtienes la posición de la cámara
	auto sphere = Application->root->CreateSphere("PhysicsSphere");
	Application->physicsModule->SpawnPhysSphereWithForce(*sphere, 1.0f, 15.0f, cameraPosition,500.0f);
}

void SpawnCar() {
    VehicleInfo car;

    // Vehicle chassis properties
    car.chassis_size = glm::vec3(2, 1, 4);
    car.chassis_offset = glm::vec3(0, 1, 0);
    car.mass = 500.0f;
    car.suspensionStiffness = 15.88f;
    car.suspensionCompression = 0.83f;
    car.suspensionDamping = 20;
    car.maxSuspensionTravelCm = 1000.0f;
    car.frictionSlip = 50.5;
    car.maxSuspensionForce = 2000.0f;

    // Wheel properties
    float half_width = 1.0f; // Separation from the chassis center
    float wheel_width = 0.4f; // Adjusted width
    float connection_height = -0.5f; // Directly under the chassis
    float half_length = 1.8f; // Slightly inward for compact placement
    float wheel_radius = 0.5f;
    float suspensionRestLength = 0.6f;

    glm::vec3 direction(-1, 0, 0); // Suspension direction
    glm::vec3 axis(0, -1, 0);      // Wheel rotation axis

    car.num_wheels = 4;
    car.wheels = new Wheel[4];

    // Configure each wheel
    car.wheels[0] = { glm::vec3(half_width, connection_height, half_length), direction, axis, suspensionRestLength, wheel_radius, wheel_width, true, true, true, true };
    car.wheels[1] = { glm::vec3(-half_width, connection_height, half_length), direction, axis, suspensionRestLength, wheel_radius, wheel_width, true, true, true, true };
    car.wheels[2] = { glm::vec3(half_width, connection_height, -half_length), direction, axis, suspensionRestLength, wheel_radius, wheel_width, false, true, true, false };
    car.wheels[3] = { glm::vec3(-half_width, connection_height, -half_length), direction, axis, suspensionRestLength, wheel_radius, wheel_width, false, true, true, false };

    // Set spawn height above ground
    float spawnHeight = 5.0f;

    // Create the vehicle in the physics module
    PhysVehicle3D* vehicle = Application->physicsModule->AddVehicle(car);
    vehicle->SetPos(0, spawnHeight, 0);

    // Create and configure the chassis
    auto chassis = Application->root->CreateCube("chassis");
    chassis->GetTransform()->SetPosition(glm::vec3(0, spawnHeight, 0));
    chassis->GetTransform()->SetScale(car.chassis_size);

    // Parenting for structure consistency
    std::vector<GameObject*> wheels;
    for (int i = 0; i < car.num_wheels; ++i) {
        auto wheel = Application->root->CreateCylinder("wheel" + std::to_string(i));

        // Position wheel directly under the chassis
        glm::vec3 wheel_position = car.wheels[i].connection;
        wheel_position.y += spawnHeight + connection_height; // Adjust height based on chassis

        wheel->GetTransform()->SetPosition(wheel_position);
        wheel->GetTransform()->SetScale(glm::vec3(car.wheels[i].radius * 2, car.wheels[i].width, car.wheels[i].radius * 2));

        // Rotate the wheel 90 degrees on the Z-axis
        wheel->GetTransform()->Rotate(90, glm::vec3(0, 0, 1));

        Application->root->ParentGameObject(*wheel, *chassis);
        wheels.push_back(wheel.get());
    }

    // Synchronize collisions and ensure alignment
    FinalVehicleInfo* finalVehicleInfo = new FinalVehicleInfo(chassis.get(), wheels, vehicle);
    Application->physicsModule->vehicles.add(finalVehicleInfo);

    Application->physicsModule->SyncVehicleComponents(vehicle, chassis.get(), wheels);

    std::cout << "Car spawned successfully at height " << spawnHeight << " with wheels aligned and collisions active.\n";
}



bool Input::processSDLEvents()
{
    SDL_PumpEvents();

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i] == 1)
        {
            if (keyboard[i] == KEY_IDLE)
                keyboard[i] = KEY_DOWN;
            else
                keyboard[i] = KEY_REPEAT;
        }
        else
        {
            if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
                keyboard[i] = KEY_UP;
            else
                keyboard[i] = KEY_IDLE;
        }
    }

    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x /= SCREEN_SIZE;
    mouse_y /= SCREEN_SIZE;
    mouse_z = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (buttons & SDL_BUTTON(i))
        {
            if (mouse_buttons[i] == KEY_IDLE)
                mouse_buttons[i] = KEY_DOWN;
            else
                mouse_buttons[i] = KEY_REPEAT;
        }
        else
        {
            if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
                mouse_buttons[i] = KEY_UP;
            else
                mouse_buttons[i] = KEY_IDLE;
        }
    }

    mouse_x_motion = mouse_y_motion = 0;

    static SDL_Event event;

    static bool f1Pressed = false;

    while (SDL_PollEvent(&event) != 0)
    {
        Application->gui->processEvent(event);
       
        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            mouse_z = event.wheel.y;
			//SpawnCar();
           if (f1Pressed && mouse_z > 0) {
               SpawnPhysCube();
           }
           if (f1Pressed && mouse_z < 0) {
			   SpawnCar();
           }
            break;

        case SDL_MOUSEMOTION:
            mouse_x = event.motion.x / SCREEN_SIZE;
            mouse_y = event.motion.y / SCREEN_SIZE;

            mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
            mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
            break;

        case SDL_QUIT: return false;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_1:
                break;
            case SDLK_ESCAPE:
                return false;
            case SDLK_F1:
                if (f1Pressed == false) {
                    f1Pressed = true; // Activar la bandera si F1 fue presionado

                }
                else {
					f1Pressed = false; // Desactivar la bandera si F1 fue liberado
                }
                break;
            case SDLK_DELETE: {
                int i = 0;
                while (i < selectedObjects.size()) {
                    GameObject* selectedObject = selectedObjects[i];
                    Application->root->RemoveGameObject(selectedObject);
                    i++;
                }
                selectedObjects.clear(); // Limpiar la selección después de borrar los objetos
                break;
            }

            case SDLK_c: // Ctrl + C
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Copied %d objects", selectedObjects.size());
                    if (!selectedObjects.empty()) {
                        copiedObjects.clear();
                        for (auto& selectedObject : selectedObjects) {
                            copiedObjects.push_back(selectedObject);
                        }
                    }
                }
                break;

            case SDLK_v: // Ctrl + V
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Pasted %d objects", copiedObjects.size());
                    if (!copiedObjects.empty()) {
                        for (auto& copiedObject : copiedObjects) {
                            auto newObject = std::make_shared<GameObject>(*copiedObject);
                            newObject->SetName(copiedObject->GetName() + "_copy");
                            Application->root->currentScene->AddGameObject(newObject);
                        }
                    }
                }
                break;

            case SDLK_d: // Ctrl + D
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Duplicated %d objects", selectedObjects.size());
                    if (!selectedObjects.empty()) {
                        for (auto& selectedObject : selectedObjects) {
                            auto newObject = std::make_shared<GameObject>(*selectedObject);
                            newObject->SetName(selectedObject->GetName() + "_copy");
                            Application->root->currentScene->AddGameObject(newObject);
                        }
                    }
                }
                break;
            }
            break;


        case SDL_WINDOWEVENT:
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
               /* Application->window->OnResizeWindow(event.window.data1, event.window.data2);*/
            }
            break;
        }

        case SDL_DROPFILE:
        {
            std::string fileDir = event.drop.file;
			HandleFileDrop(fileDir);
            SDL_free(event.drop.file);
            break;
        }
        }
    }
    return true;
}

void Input::HandleFileDrop(const std::string& fileDir)
{
    std::string fileNameExt = fileDir.substr(fileDir.find_last_of("\\/") + 1);
    std::string fileExt = fileDir.substr(fileDir.find_last_of('.') + 1);

    fs::path targetPath = fs::path(ASSETS_PATH) / fileNameExt;

    if (fileExt == "fbx" || fileExt == "FBX") {
        LOG(LogType::LOG_ASSIMP, "Importing FBX: %s from: %s", fileNameExt.c_str(), fileDir.c_str());

 /*       std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        mesh->LoadMesh(CopyFBXFileToProject(fileDir).c_str());

        auto go = Application->root->CreateGameObject(fileNameExt);
        Application->root->AddMeshRenderer(*go, mesh, "Assets/default.png");
*/


        // TODO , only save to filesystem if file doesnt exist 

        //new
        auto MarcoVicePresidente = Application->root->CreateGameObject("BakerHouse");
        MarcoVicePresidente->GetTransform()->GetPosition() = vec3(0, 0, 0);

        ModelImporter meshImp;
        meshImp.loadFromFile((CopyFBXFileToProject(fileDir).c_str()));

        for (int i = 0; i < meshImp.meshGameObjects.size(); i++) {
            auto MarcoVicePresidente2 = meshImp.meshGameObjects[i];

            auto go = Application->root->CreateGameObject("GameObject");
            auto color = MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMaterial()->color;
          
            Application->root->AddMeshRenderer(*go, MarcoVicePresidente2->GetComponent<MeshRenderer>()->GetMesh(), "Assets/default.png");
            go->GetComponent<MeshRenderer>()->GetMaterial()->SetColor(color);
            go->GetTransform()->SetLocalMatrix(MarcoVicePresidente2->GetTransform()->GetLocalMatrix());

            //Application->root->currentScene->AddGameObject(MarcoVicePresidente2);


            Application->root->ParentGameObject(*go, *MarcoVicePresidente);
        }

    }
    else if (fileExt == "png" || fileExt == "dds" || fileExt == "tga" || fileExt == "jpg" || fileExt == "jpeg") {
        LOG(LogType::LOG_INFO, "Loading Texture: %s from: %s", fileNameExt.c_str(), fileDir.c_str());

        if (draggedObject != nullptr) {
            auto meshRenderer = draggedObject->GetComponent<MeshRenderer>();

            draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->getImg()->LoadTexture(fileDir);
            //draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->setImage();
            //meshRenderer->SetMaterial(material);
        }
    }
    else if (fileExt == "image") {
        if (draggedObject != nullptr) {
            auto meshRenderer = draggedObject->GetComponent<MeshRenderer>();
            auto image = std::make_shared<Image>();
            auto material = std::make_shared<Material>();

            image->LoadBinary(fileDir);
            material->setImage(image);
            meshRenderer->SetMaterial(material);
        }
    }
    else if (fileExt == "scene")
    {
        LOG(LogType::LOG_INFO, "Loading Scene File: %s from: %s", fileNameExt.c_str(), fileDir.c_str());
    }
    else if (fileExt == "mesh")
    {
        LOG(LogType::LOG_INFO, "Detected Custom Mesh: %s from: %s", fileNameExt.c_str(), fileDir.c_str());
    }

    try {
        if (!fs::exists(targetPath) || fs::file_size(fileDir) != fs::file_size(targetPath)) {
            fs::copy(fileDir, targetPath, fs::copy_options::overwrite_existing);
            LOG(LogType::LOG_OK, "File copied to: %s", targetPath.string().c_str());
        }
        else {
            LOG(LogType::LOG_INFO, "File already exists: %s", targetPath.string().c_str());
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Failed to copy file: %s - %s", fileNameExt.c_str(), e.what());
    }
    // Lógica por si la file ya existe
}

glm::vec3 Input::getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    float x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportSize.y;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    return rayWorld;
}

glm::vec3 Input::getMousePickRay() 
{
    ImVec2 windowPos = ImVec2(static_cast<float>(Application->gui->UISceneWindowPanel->winPos.x), static_cast<float>(Application->gui->UISceneWindowPanel->winPos.y));
    ImVec2 windowSize = ImVec2(static_cast<float>(Application->gui->UISceneWindowPanel->winSize.x), static_cast<float>(Application->gui->UISceneWindowPanel->winSize.y));

    float mouseX = Application->input->GetMouseX() - windowPos.x + 10;
    float mouseY = Application->input->GetMouseY() - windowPos.y + 20;

    glm::ivec2 size = glm::ivec2(windowSize.x, windowSize.y);

    // Ensure the mouse coordinates are within the bounds of the ImGui window
    if (mouseX >= 0 && mouseX <= windowSize.x && mouseY >= 0 && mouseY <= windowSize.y) {

    }
   
    glm::vec3 rayDirection = Application->input->getRayFromMouse(static_cast<int>(mouseX), static_cast<int>(mouseY), camera->projection(), camera->view(), size);
	return rayDirection;
}

std::string CopyFBXFileToProject(const std::string& sourceFilePath) {


    std::string fileNameExt = sourceFilePath.substr(sourceFilePath.find_last_of('\\') + 1);
    fs::path assetsDir = fs::path(ASSETS_PATH) / "Meshes" / fileNameExt;

    try {
        /* to copy the source file to the destination path, overwriting if it already exists*/
        std::filesystem::copy(sourceFilePath, assetsDir, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
    std::cout << (assetsDir.string()).c_str();
    return assetsDir.string();
}

void Input::AddToSelection(GameObject* gameObject) {
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
    if (it == selectedObjects.end()) {
        selectedObjects.push_back(gameObject);
        gameObject->isSelected = true;
    }
}

void Input::RemoveFromSelection(GameObject* gameObject) {
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
    if (it != selectedObjects.end()) {
        selectedObjects.erase(it);
        gameObject->isSelected = false;
    }
}

std::vector<GameObject*> Input::GetSelectedGameObjects() const {
    return selectedObjects;
}

void Input::ClearSelection() {
	for (auto& selectedObject : selectedObjects) {
		selectedObject->isSelected = false;
	}
	selectedObjects.clear();
}

bool Input::IsGameObjectSelected(GameObject* gameObject) const {
	auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
	return it != selectedObjects.end();
}