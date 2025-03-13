#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <filesystem>

#include "imgui.h"

#include "MyGameEngine/Mesh.h"
#include "App.h"
#include "Log.h"
#include "MyGUI.h"
#include "UiSceneWindow.h"
#include "Input.h"
#include "EditorCamera.h"
#include "MyWindow.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
#include "../MyGameEngine/ModelImporter.h"
#include "../MyPhysicsEngine/PhysVehicle3D.h"
#include "../MyPhysicsEngine/PhysicsModule.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyAudioEngine/AudioAssetProcessor.h"
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include <SDL2/SDL.h> // idk what to do to remove this
#include <string>
#include <iostream>
#include <filesystem>
#include "../MyShadersEngine/ShaderComponent.h"


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



void SpawnPhysCube() {
    //auto cube = Application->root->CreateCube("PhysicsCube");
    //cube->GetTransform()->SetPosition(glm::vec3(0, 10, 0));
    //Application->physicsModule->CreatePhysicsForGameObject(*cube, 1.0f); // Mass
    //glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition(); // Reemplaza con la forma en que obtienes la posici�n de la c�mara
    //auto sphere = Application->root->CreateSphere("PhysicsSphere");
    //glm::vec3 cameraDirection = Application->camera->GetTransform().GetForward();
    //Application->physicsModule->SpawnPhysSphereWithForce(,*sphere, 1.0f, 15.0f, cameraPosition,cameraDirection, 500.0f);
}
bool Input::processSDLEvents()
{
    SDL_PumpEvents();

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j) {
        if (SDL_GameControllerGetButton(InputManagement->gamepads[0].controller, (SDL_GameControllerButton)j))
        {
            if (InputManagement->controller_buttons[j] == KEY_IDLE)
            {
                InputManagement->controller_buttons[j] = KEY_DOWN;
            }
            else
            {
                InputManagement->controller_buttons[j] = KEY_REPEAT;
            }
        }
        else
        {
            if (InputManagement->controller_buttons[j] == KEY_REPEAT || InputManagement->controller_buttons[j] == KEY_DOWN)
            {
                InputManagement->controller_buttons[j] = KEY_UP;
            }
            else
            {
                InputManagement->controller_buttons[j] = KEY_IDLE;
            }
        }
    }
        
	/*if (GetControllerKey(SDL_CONTROLLER_BUTTON_A) == KEY_UP)
	{
		LOG(LogType::LOG_INFO, "A button pressed");
	}

	if (int leftXAxisValue = GetJoystickAxis(0, SDL_CONTROLLER_AXIS_LEFTX))
	{
		LOG(LogType::LOG_INFO, "Left X Axis Value: %d", leftXAxisValue);
	}*/

    //const Uint8* controllerKeys = SDL_Getcontroller

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

    static bool pPressed = false;

    int gamepadIndex = event.cbutton.which;
	int button = event.cbutton.button;

    while (SDL_PollEvent(&event) != 0)
    {
        Application->gui->processEvent(event);
       
        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            InputManagement->mouse_z = event.wheel.y;
            if (pPressed && InputManagement->mouse_z > 0) {
                SpawnPhysCube();
            }
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
            case SDLK_DELETE: {
                int i = 0;
                while (i < InputManagement->selectedObjects.size()) {
                    GameObject* selectedObject = InputManagement->selectedObjects[i];
                    Application->root->RemoveGameObject(selectedObject);
                    i++;
                }
                InputManagement->selectedObjects.clear(); // Limpiar la selecci�n despu�s de borrar los objetos
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
            case SDLK_p:
                if (pPressed == false) {
                    pPressed = true; // Activar la bandera si P fue presionado
                    //Application->root->CreateSceneColliders();
                    Application->physicsModule->linkPhysicsToScene = true;
                }
                else {
                    pPressed = false;
                    Application->physicsModule->linkPhysicsToScene = false;
                }
                break; 

                
            }
            
            break;

        case(SDL_CONTROLLERDEVICEADDED):
            InputManagement->HandleDeviceConnection(event.cdevice.which);
            break;
        case(SDL_CONTROLLERDEVICEREMOVED):
            InputManagement->HandleDeviceRemoval(event.cdevice.which);
            break;

		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			// Update gamepad button state
			gamepadIndex = event.cbutton.which;
			button = event.cbutton.button;

			// Map the button correctly based on SDL_GameControllerButton enum
			switch (button) {
			case SDL_CONTROLLER_BUTTON_A:
				// Handle A button (already working)
				break;
			case SDL_CONTROLLER_BUTTON_B:
				// Handle B button (already working)
				break;
			case SDL_CONTROLLER_BUTTON_X:
				// Handle X button (already working)
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				// Handle Y button (already working)
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				// This should be treated as left bumper/shoulder
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				// This should be treated as right bumper/shoulder
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSTICK:
				// Left stick press
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
				// Right stick press
				break;
				// Add other buttons as needed
			}

			if (event.type == SDL_CONTROLLERBUTTONDOWN)
			{
				InputManagement->gamepads[gamepadIndex].buttons[button] = KEY_DOWN;
			}
			else
			{
				InputManagement->gamepads[gamepadIndex].buttons[button] = KEY_UP;
			}
			break;
        case SDL_CONTROLLERAXISMOTION:
            // Manejar eventos de ejes del controlador aquí
            switch (event.caxis.axis) {
            case SDL_CONTROLLER_AXIS_LEFTX:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTX] = event.caxis.value;
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTY] = event.caxis.value;
                break;
            case SDL_CONTROLLER_AXIS_RIGHTX:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTX] = event.caxis.value;
                break;
            case SDL_CONTROLLER_AXIS_RIGHTY:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTY] = event.caxis.value;
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERLEFT] = event.caxis.value;
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = event.caxis.value;
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

    fs::path targetPath = fs::path(LIBRARY_PATH) / fileNameExt;

    if (fileExt == "fbx" || fileExt == "FBX") {
        LOG(LogType::LOG_ASSIMP, "Importing FBX: %s from: %s", fileNameExt.c_str(), fileDir.c_str());

        Application->root->CreateGameObjectWithPath(fileDir);

    }
    else if (fileExt == "png" || fileExt == "dds" || fileExt == "tga" || fileExt == "jpg" || fileExt == "jpeg") {
        LOG(LogType::LOG_INFO, "Loading Texture: %s from: %s", fileNameExt.c_str(), fileDir.c_str());

        // Create Audio directory if it doesn't exist
        fs::path textureDir = fs::path(ASSETS_PATH) / "Textures";
        if (!fs::exists(textureDir)) {
            fs::create_directories(textureDir);
        }

        
        targetPath = textureDir / fileNameExt;

        
        fs::path libraryPath = fs::path("Library/Textures") / fileNameExt;
        if (!fs::exists(libraryPath.parent_path())) {
            fs::create_directories(libraryPath.parent_path());
        }

        try {
            if (!fs::exists(libraryPath) || fs::file_size(fileDir) != fs::file_size(libraryPath)) {
                fs::copy(fileDir, libraryPath, fs::copy_options::overwrite_existing);
                LOG(LogType::LOG_OK, "Texture file copied to Assets: %s", libraryPath.string().c_str());
            }

            LOG(LogType::LOG_OK, "Texture file processed to Library: %s", libraryPath.string().c_str());
        }
        catch (const std::exception& e) {
            LOG(LogType::LOG_ERROR, "Error processing texture file: %s", e.what());
        }
        if (InputManagement->draggedObject != nullptr) {
            auto meshRenderer = InputManagement->draggedObject->GetComponent<MeshRenderer>();

            InputManagement->draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->getImg()->LoadTexture(fileDir);
            //draggedObject->GetComponent<MeshRenderer>()->GetMaterial()->setImage();
            //meshRenderer->SetMaterial(material);
        }
    }
    else if (fileExt == "wav" || fileExt == "ogg" || fileExt == "mp3") {
        LOG(LogType::LOG_INFO, "Importing Audio: %s from: %s", fileNameExt.c_str(), fileDir.c_str());
        
        // Create Audio directory if it doesn't exist
        fs::path audioDir = fs::path(ASSETS_PATH) / "Audio";
        if (!fs::exists(audioDir)) {
            fs::create_directories(audioDir);
        }
        
        // Update target path to Audio subdirectory
        targetPath = audioDir / fileNameExt;

        // Process the audio file for Library
        fs::path libraryPath = fs::path("Library/Audio") / fileNameExt;
        if (!fs::exists(libraryPath.parent_path())) {
            fs::create_directories(libraryPath.parent_path());
        }

        // First copy to Assets
        try {
            if (!fs::exists(targetPath) || fs::file_size(fileDir) != fs::file_size(targetPath)) {
                fs::copy(fileDir, targetPath, fs::copy_options::overwrite_existing);
                LOG(LogType::LOG_OK, "Audio file copied to Assets: %s", targetPath.string().c_str());
            }

            // Then process to Library
            MyGameEngine::AudioAssetProcessor::ProcessAudioFile(targetPath.string(), libraryPath.string());
            LOG(LogType::LOG_OK, "Audio file processed to Library: %s", libraryPath.string().c_str());
        }
        catch (const std::exception& e) {
            LOG(LogType::LOG_ERROR, "Error processing audio file: %s", e.what());
        }
        
        if (InputManagement->draggedObject != nullptr) {
            auto soundComponent = InputManagement->draggedObject->GetComponent<SoundComponent>();
            if (soundComponent) {
                soundComponent->LoadAudio(targetPath.string());
            }
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
    // L�gica por si la file ya existe
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

KEY_STATE Input::GetControllerKey(int id) 
{
	return InputManagement->controller_buttons[id];
}

int Input::GetJoystickAxis(int gamepadIndex, SDL_GameControllerAxis axis) {
    return (InputManagement->gamepads[gamepadIndex].axes[axis]/ 32767.0f);
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