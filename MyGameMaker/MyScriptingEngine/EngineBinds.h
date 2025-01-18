#pragma once
#ifndef ENGINE_BINDS_H
#define ENGINE_BINDS_H

#include <mono/metadata/object.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class GameObject;

namespace EngineBinds {
    void BindEngine();

    // GameObject
    MonoObject* CreateGameObjectSharp(MonoString* name);
    MonoString* GameObjectGetName(MonoObject* sharpRef);
    GameObject* ConvertFromSharp(MonoObject* sharpObj);
    void GameObjectAddChild(MonoObject* parent, MonoObject* child);
    void Destroy(MonoObject* object_to_destroy);
    MonoObject* GetSharpComponent(MonoObject* ref, MonoString* comoponent_name);

    template <class T>
    T* ConvertFromSharpComponent(MonoObject* sharpComp);

    // Input
    bool GetKey(int keyID);
    bool GetKeyDown(int keyID);
    bool GetKeyUp(int keyID);
    bool GetMouseButton(int buttonID);
    bool GetMouseButtonDown(int buttonID);
    bool GetMouseButtonUp(int buttonID);
    int GetAxis(MonoString* axisName);
    glm::vec3 GetMousePosition();

    // Transform
    void SetPosition(MonoObject* transformRef, float x, float y, float z);
    void GetPosition(MonoObject* transformRef, glm::vec3* position);
    void SetRotation(MonoObject* transformRef, float x, float y, float z);
    void SetRotationQuat(MonoObject* transformRef, glm::quat* rotation);
    void Rotate(MonoObject* transformRef, float radians, glm::vec3* axis);
    void RotateLocal(MonoObject* transformRef, float radians, glm::vec3* axis);
    void LookAt(MonoObject* transformRef, glm::vec3* target);
    void SetScale(MonoObject* transformRef, float x, float y, float z);
    void Scale(MonoObject* transformRef, float x, float y, float z);
    void TranslateLocal(MonoObject* transformRef, float x, float y, float z);
    void AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp);
    void SetForward(MonoObject* transformRef, glm::vec3* forward);

	// Camera
    void SetCameraFieldOfView(MonoObject* cameraRef, float fov);
    void SetCameraNearClipPlane(MonoObject* cameraRef, float nearClipPlane);
    void SetCameraFarClipPlane(MonoObject* cameraRef, float farClipPlane);
    void SetCameraAspectRatio(MonoObject* cameraRef, float aspectRatio);
    void SetCameraOrthographicSize(MonoObject* cameraRef, float orthographicSize);
    void SetCameraProjectionType(MonoObject* cameraRef, int projectionType);
}

#endif // ENGINE_BINDS_H