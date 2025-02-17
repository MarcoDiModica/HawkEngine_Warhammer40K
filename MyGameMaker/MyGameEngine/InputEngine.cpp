#include "InputEngine.h"
#include "MeshRendererComponent.h"
#include "Image.h"
#include "Material.h"
#include "ModelImporter.h"
#include "Mesh.h"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <filesystem>
#include "../MyGameEditor/Log.h"

#define MAX_KEYS 300
#define MAX_CONTROLLERS 4
#define SCREEN_SIZE 1
#define ASSETS_PATH "Assets\\"

namespace fs = std::filesystem;

InputEngine::InputEngine()
{
    keyboard = new KEY_STATE[MAX_KEYS];
    memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
    memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
    memset(&gamepads[0], 0, sizeof(GamePad) * MAX_CONTROLLERS);
    InitControllers();
}

InputEngine::~InputEngine() {
    delete[] keyboard;
    CleanUpControllers();
}

bool InputEngine::Update(double dt)
{
    bool ret = true;
    UpdateControllers();
    return ret;
}

glm::vec3 InputEngine::getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    float x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportSize.y;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    return rayWorld;
}

void InputEngine::AddToSelection(GameObject* gameObject) {
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
    if (it == selectedObjects.end()) {
        selectedObjects.push_back(gameObject);
        gameObject->isSelected = true;
    }
}

void InputEngine::RemoveFromSelection(GameObject* gameObject) {
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
    if (it != selectedObjects.end()) {
        selectedObjects.erase(it);
        gameObject->isSelected = false;
    }
}

std::vector<GameObject*> InputEngine::GetSelectedGameObjects() const {
    return selectedObjects;
}

void InputEngine::ClearSelection() {
    for (auto& selectedObject : selectedObjects) {
        selectedObject->isSelected = false;
    }
    selectedObjects.clear();
}

bool InputEngine::IsGameObjectSelected(GameObject* gameObject) const {
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), gameObject);
    return it != selectedObjects.end();
}

int InputEngine::GetAxis(const char* axisName) const {
    if (strcmp(axisName, "Mouse X") == 0)
    {
        return GetMouseXMotion();
    }
    else if (strcmp(axisName, "Mouse Y") == 0)
    {
        return GetMouseYMotion();
    }
    else if (strcmp(axisName, "Horizontal") == 0)
    {
        if (GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
        {
            return -1;
        }
        else if (GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(axisName, "Vertical") == 0)
    {
        if (GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
        {
            return 1;
        }
        else if (GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void InputEngine::InitControllers()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        std::cerr << "SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    const char* ps4Mapping = "030000004c050000c405000011810000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,platform:Windows,";
    if (SDL_GameControllerAddMapping(ps4Mapping) == -1)
    {
        std::cerr << "Failed to add PS4 controller mapping: " << SDL_GetError() << std::endl;
    }

    if (SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
    {
        std::cerr << "SDL_INIT_HAPTIC could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
}

void InputEngine::CleanUpControllers()
{
    for (int i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (gamepads[i].haptic != nullptr)
        {
            SDL_HapticStopAll(gamepads[i].haptic);
            SDL_HapticClose(gamepads[i].haptic);
        }
        if (gamepads[i].controller != nullptr) SDL_GameControllerClose(gamepads[i].controller);
    }

    SDL_QuitSubSystem(SDL_INIT_HAPTIC);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void InputEngine::UpdateControllers()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case(SDL_CONTROLLERDEVICEADDED):
            HandleDeviceConnection(event.cdevice.which);
            break;
        case(SDL_CONTROLLERDEVICEREMOVED):
            HandleDeviceRemoval(event.cdevice.which);
            break;
        }
    }

    for (int i = 0; i < MAX_CONTROLLERS; ++i)
    {
        GamePad& pad = gamepads[i];

        if (pad.enabled)
        {
            pad.a = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_A) == 1;
            pad.b = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_B) == 1;
            pad.x = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_X) == 1;
            pad.y = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_Y) == 1;
            pad.l1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1;
            pad.r1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1;
            pad.l3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1;
            pad.r3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1;
            pad.up = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1;
            pad.down = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1;
            pad.left = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1;
            pad.right = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1;

            pad.start = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_START) == 1;
            pad.guide = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_GUIDE) == 1;
            pad.back = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_BACK) == 1;

            pad.l2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / 32767.0f;
            pad.r2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / 32767.0f;

            pad.l_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTX)) / 32767.0f;
            pad.l_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTY)) / 32767.0f;
            pad.r_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTX)) / 32767.0f;
            pad.r_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTY)) / 32767.0f;

            pad.l_x = (fabsf(pad.l_x) > pad.l_dz) ? pad.l_x : 0.0f;
            pad.l_y = (fabsf(pad.l_y) > pad.l_dz) ? pad.l_y : 0.0f;
            pad.r_x = (fabsf(pad.r_x) > pad.r_dz) ? pad.r_x : 0.0f;
            pad.r_y = (fabsf(pad.r_y) > pad.r_dz) ? pad.r_y : 0.0f;

            if (pad.rumble_countdown > 0)
                pad.rumble_countdown--;
        }
    }
}

void InputEngine::HandleDeviceConnection(int index)
{
    if (SDL_IsGameController(index))
    {
        for (int i = 0; i < MAX_CONTROLLERS; ++i)
        {
            GamePad& pad = gamepads[i];

            if (!pad.enabled)
            {
                if (pad.controller = SDL_GameControllerOpen(index))
                {
                    std::cout << "Found a gamepad with id " << i << " named " << SDL_GameControllerName(pad.controller) << std::endl;
                    pad.enabled = true;
                    pad.haptic = SDL_HapticOpen(index);
                    if (pad.haptic != nullptr)
                        std::cout << "... gamepad has force feedback capabilities" << std::endl;
                    pad.index = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad.controller));
                }
            }
        }
    }
}

void InputEngine::HandleDeviceRemoval(int index)
{
    for (int i = 0; i < MAX_CONTROLLERS; ++i)
    {
        GamePad& pad = gamepads[i];
        if (pad.enabled && pad.index == index)
        {
            SDL_HapticClose(pad.haptic);
            SDL_GameControllerClose(pad.controller);
            memset(&pad, 0, sizeof(GamePad));
        }
    }
}