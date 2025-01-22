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
#include "../MyPhysicsEngine/PhysicsModule.h"
#include <SDL2/SDL.h> // idk what to do to remove this
#include <string>
#include <iostream>
#include <filesystem>


#define MAX_KEYS 300
#define SCREEN_SIZE 1
#define ASSETS_PATH "Assets\\"

namespace fs = std::filesystem;

Input::Input(App* app) : Module(app)
{
	InputManagement = new InputEngine();
}

Input::~Input()
{
    delete[]  InputManagement->keyboard;
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
	return InputManagement->Update(dt);
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


void SpawnCube() {
    auto cube = Application->root->CreateCube("PhysicsCube");
    cube->GetTransform()->SetPosition(glm::vec3(0, 10, 0));
    auto t = cube->GetTransform()->GetPosition();
    //Application->physicsModule->CreatePhysicsForCube(*cube, 1.0f); // Masa 1.0f
}

bool Input::processSDLEvents()
{
    SDL_PumpEvents();

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i] == 1)
        {
            if (InputManagement->keyboard[i] == KEY_IDLE)
                InputManagement->keyboard[i] = KEY_DOWN;
            else
                InputManagement->keyboard[i] = KEY_REPEAT;
        }
        else
        {
            if (InputManagement->keyboard[i] == KEY_REPEAT || InputManagement->keyboard[i] == KEY_DOWN)
                InputManagement->keyboard[i] = KEY_UP;
            else
                InputManagement->keyboard[i] = KEY_IDLE;
        }
    }

    Uint32 buttons = SDL_GetMouseState(&InputManagement->mouse_x, &InputManagement->mouse_y);

     InputManagement->mouse_x /= SCREEN_SIZE;
     InputManagement->mouse_y /= SCREEN_SIZE;
     InputManagement->mouse_z = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (buttons & SDL_BUTTON(i))
        {
            if (InputManagement->mouse_buttons[i] == KEY_IDLE)
                InputManagement->mouse_buttons[i] = KEY_DOWN;
            else
                InputManagement->mouse_buttons[i] = KEY_REPEAT;
        }
        else
        {
            if (InputManagement->mouse_buttons[i] == KEY_REPEAT || InputManagement->mouse_buttons[i] == KEY_DOWN)
                InputManagement->mouse_buttons[i] = KEY_UP;
            else
                InputManagement->mouse_buttons[i] = KEY_IDLE;
        }
    }

    InputManagement->mouse_x_motion = InputManagement->mouse_y_motion = 0;

    static SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        Application->gui->processEvent(event);
       
        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            InputManagement->mouse_z = event.wheel.y;
            break;

        case SDL_MOUSEMOTION:
            InputManagement->mouse_x = event.motion.x / SCREEN_SIZE;
            InputManagement->mouse_y = event.motion.y / SCREEN_SIZE;

            InputManagement->mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
            InputManagement->mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
            break;

        case SDL_QUIT: return false;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_1:
                break;
            case SDLK_ESCAPE:
                return false;

            case SDLK_DELETE: {
                int i = 0;
                while (i < InputManagement->selectedObjects.size()) {
                    GameObject* selectedObject = InputManagement->selectedObjects[i];
                    Application->root->RemoveGameObject(selectedObject);
                    i++;
                }
                InputManagement->selectedObjects.clear(); // Limpiar la selección después de borrar los objetos
                break;
            }

            case SDLK_c: // Ctrl + C
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Copied %d objects", InputManagement->selectedObjects.size());
                    if (!InputManagement->selectedObjects.empty()) {
                        InputManagement->copiedObjects.clear();
                        for (auto& selectedObject : InputManagement->selectedObjects) {
                            InputManagement->copiedObjects.push_back(selectedObject);
                        }
                    }
                }
                break;

            case SDLK_v: // Ctrl + V
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Pasted %d objects", InputManagement->copiedObjects.size());
                    if (!InputManagement->copiedObjects.empty()) {
                        for (auto& copiedObject : InputManagement->copiedObjects) {
                            auto newObject = std::make_shared<GameObject>(*copiedObject);
                            newObject->SetName(copiedObject->GetName() + "_copy");
                            Application->root->GetActiveScene()->AddGameObject(newObject);
                        }
                    }
                }
                break;

            case SDLK_d: // Ctrl + D
                if (SDL_GetModState() & KMOD_CTRL) {
                    LOG(LogType::LOG_INFO, "Duplicated %d objects", InputManagement->selectedObjects.size());
                    if (!InputManagement->selectedObjects.empty()) {
                        for (auto& selectedObject : InputManagement->selectedObjects) {
                            auto newObject = std::make_shared<GameObject>(*selectedObject);
                            newObject->SetName(selectedObject->GetName() + "_copy");
                            Application->root->GetActiveScene()->AddGameObject(newObject);
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


            Application->root->ParentGameObject(*go, *MarcoVicePresidente);
        }

    }
    else if (fileExt == "png" || fileExt == "dds" || fileExt == "tga" || fileExt == "jpg" || fileExt == "jpeg") {
        LOG(LogType::LOG_INFO, "Loading Texture: %s from: %s", fileNameExt.c_str(), fileDir.c_str());

        if (InputManagement->draggedObject != nullptr) {
            auto meshRenderer = InputManagement->draggedObject->GetComponent<MeshRenderer>();

            InputManagement->draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->getImg()->LoadTexture(fileDir);
            //draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->setImage();
            //meshRenderer->SetMaterial(material);
        }
    }
    else if (fileExt == "image") {
        if (InputManagement->draggedObject != nullptr) {
            auto meshRenderer = InputManagement->draggedObject->GetComponent<MeshRenderer>();
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

KEY_STATE Input::GetKey(int id)
{
	return InputManagement->GetKey(id);
}

KEY_STATE Input::GetMouseButton(int id)
{
	return InputManagement->GetMouseButton(id);
}

int Input::GetMouseX()
{
	return InputManagement->GetMouseX();
}

int Input::GetMouseY()
{
	return InputManagement->GetMouseY();
}

int Input::GetMouseZ()
{
	return InputManagement->GetMouseZ();
}

int Input::GetMouseXMotion()
{
	return InputManagement->GetMouseXMotion();
}

int Input::GetMouseYMotion()
{
	return InputManagement->GetMouseYMotion();
}

GameObject* Input::GetDraggedGameObject()
{
	return InputManagement->GetDraggedGameObject();
}

void Input::SetDraggedGameObject(GameObject* gameObject)
{
	InputManagement->SetDraggedGameObject(gameObject);
}

glm::vec3 Input::getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
	return InputManagement->getRayFromMouse(mouseX, mouseY, projection, view, viewportSize);
}


void Input::AddToSelection(GameObject* gameObject) {
	InputManagement->AddToSelection(gameObject);
}

void Input::RemoveFromSelection(GameObject* gameObject) {
	InputManagement->RemoveFromSelection(gameObject);
}

std::vector<GameObject*> Input::GetSelectedGameObjects() {
	return InputManagement->GetSelectedGameObjects();
}

void Input::ClearSelection() {
	InputManagement->ClearSelection();
}

bool Input::IsGameObjectSelected(GameObject* gameObject) {
	return InputManagement->IsGameObjectSelected(gameObject);
}

int Input::GetAxis(const char* axisName) {
	return InputManagement->GetAxis(axisName);
}