#ifndef __UI_SCENE_WINDOW_H__
#define __UI_SCENE_WINDOW_H__
#pragma once

#include <string>

#include "UIElement.h"
#include "MyGameEngine/types.h"
#include "MyGameEngine/BoundingBox.h"
#include "MyGameEngine/Image.h"

class UISceneWindow : public UIElement
{
public:
    UISceneWindow(UIType type, std::string name);
    ~UISceneWindow() = default;
    void Init();
    bool Draw();
    vec2 winSize = vec2(0, 0);
    vec2 winPos = vec2(0, 0);
    vec2 winMousePos = vec2(0, 0);
    glm::vec3 gizmoOriginalScale = glm::vec3(1.0f);

    bool isFoucused = false;

    bool IsMouseOverWindow() const;
private:
    const float iconSize = 25.0f;
    const float iconSpacing = 5.0f; //spacing between icons
    Image m_Local;
    Image m_World;
    Image m_Trans;
    Image m_Rot;
    Image m_Sca;
    Image m_Setting;
};
#endif // !__UI_SCENE_WINDOW_H__