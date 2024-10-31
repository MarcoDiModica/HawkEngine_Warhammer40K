#include "MyGameEngine/Mesh.h"
#include "App.h"
#include "Log.h"
#include "MyGUI.h"
#include "Input.h"
#include "Camera.h"
#include "MyWindow.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "MyGameEngine/Image.h"
#include "MyGameEngine/Material.h"
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

    while (SDL_PollEvent(&event) != 0)
    {
        Application->gui->processEvent(event);

        switch (event.type)
        {
        case SDL_MOUSEWHEEL:
            mouse_z = event.wheel.y;
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
            case SDLK_ESCAPE: return false;
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
            // Just flag event here
            // This code elsewhere
            std::string fileDir = event.drop.file;
            std::string fileNameExt = fileDir.substr(fileDir.find_last_of('\\') + 1);

            std::string fbxName = fileDir.substr(fileDir.find_last_of("\\/") + 1, fileDir.find_last_of('.') - fileDir.find_last_of("\\/") - 1);

            // FBX
            if (fileDir.ends_with(".fbx") || fileDir.ends_with(".FBX"))
            {
                fs::path assetsDir = fs::path(ASSETS_PATH) / "Meshes" / fileNameExt;

                LOG(LogType::LOG_ASSIMP, "Importing %s from: %s", fileNameExt.data(), fileDir.data());

                /* Application->ElMesh.LoadMesh("BakerHouse.fbx");
                Application->ElMesh.LoadTexture("Baker_house.png");
                Application->ElMesh.LoadCheckerTexture();*/

                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
                mesh->LoadMesh(CopyFBXFileToProject(fileDir).c_str());
                //mesh->LoadCheckerTexture();

                Application->root->CreateMeshObject(fileNameExt,  mesh);

               

                //Application->ElMesh.LoadMesh(CopyFBXFileToProject( fileDir).c_str());

                //Application->ElMesh.LoadCheckerTexture();


            }

            // PNG / DDS
            else if (fileDir.ends_with(".png") || fileDir.ends_with(".dds"))
            {
                std::filesystem::copy(fileDir, "Assets", std::filesystem::copy_options::overwrite_existing);
				if (selectedObject != nullptr)
				{
                    auto meshRenderer = selectedObject->GetComponent<MeshRenderer>();
                    auto image = std::make_shared<Image>();
                    auto material = std::make_shared<Material>();
                    image->LoadTexture(fileDir);
                    material->setImage(image);
                    meshRenderer->SetMaterial(material);
				}
            }
            SDL_free(event.drop.file);
            break;
        }
        }
    }
    return true;
}

#pragma region TODO(look what to do with the camera)

glm::vec3 Input::MousePosToWorld()
{
   /* float ndc_x = (2.0f * mouse_x) / Application->window->width() - 1.0f;
    float ndc_y = 1.0f - (2.0f * mouse_y) / Application->window->height();

    glm::vec4 clip_coords = glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);


    glm::mat4 projection_matrix = camera.projection();
    glm::vec4 view_coords = glm::inverse(projection_matrix) * clip_coords;
    view_coords = glm::vec4(view_coords.x, view_coords.y, -1.0f, 0.0f);

    glm::mat4 view_matrix = camera.view();
    glm::vec4 world_coords = glm::inverse(view_matrix) * view_coords;


    return glm::vec3(world_coords.x + camera.transform().pos().x, world_coords.y + camera.transform().pos().y, world_coords.z + camera.transform().pos().z);*/
    return vec3(0.0f);
}

glm::vec3 Input::MouseRayPick() 
{
    //// Coordenadas del ratón en el espacio de la ventana
    //glm::vec3 window_coords = glm::vec3(mouse_x, Application->window->height() - mouse_y, 0.0f);

    //// Matrices de vista y proyección
    //glm::mat4 view_matrix = camera.view();
    //glm::mat4 projection_matrix = camera.projection();

    //// Viewport
    //glm::vec4 viewport = glm::vec4(0, 0, Application->window->width(), Application->window->height());


    //glm::vec3 v0 = glm::unProject(window_coords, view_matrix, projection_matrix, viewport);
    //glm::vec3 v1 = glm::unProject(glm::vec3(window_coords.x, window_coords.y, 1.0f), view_matrix, projection_matrix, viewport);

    //// Desproyectar las coordenadas del ratón
    //glm::vec3 world_coords = (v1 - v0);

    //return world_coords;
	return vec3(0.0f);
}
#pragma endregion

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