#include "InputEngine.h"
#include "MeshRendererComponent.h"
#include "Image.h"
#include "Material.h"
#include "ModelImporter.h"
#include "Mesh.h"
#include <SDL2/SDL.h> // idk what to do to remove this
#include <string>
#include <iostream>
#include <filesystem>
#include "../MyGameEditor/Log.h"

#define MAX_KEYS 300
#define SCREEN_SIZE 1
#define ASSETS_PATH "Assets\\"

namespace fs = std::filesystem;

InputEngine::InputEngine()
{
    keyboard = new KEY_STATE[MAX_KEYS];
    memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
    memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

InputEngine::~InputEngine() {
    delete[] keyboard;
}

bool InputEngine::Update(double dt)
{
    bool ret = true;

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
