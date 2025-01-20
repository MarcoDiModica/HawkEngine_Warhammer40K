#pragma once
#ifndef ENGINE_BINDS_H
#define ENGINE_BINDS_H

#include <mono/metadata/object.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class GameObject;

struct Vector3 {
    float X, Y, Z; 
};

namespace EngineBinds {
    void BindEngine();

    // GameObject
    MonoObject* CreateGameObjectSharp(MonoString* name);

    GameObject* ConvertFromSharp(MonoObject* sharpObj);
    void GameObjectAddChild(MonoObject* parent, MonoObject* child);
    void Destroy(MonoObject* object_to_destroy);
    MonoObject* GetSharpComponent(MonoObject* ref, MonoString* comoponent_name);

    MonoObject* AddSharpComponent(MonoObject* ref, int component);

    template <class T>
    T* ConvertFromSharpComponent(MonoObject* sharpComp);

    MonoString* GameObjectGetName(MonoObject* sharpRef);
    void SetName(MonoObject* ref, MonoString* sharpName);

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
    Vector3 GetPosition(MonoObject* transformRef);
    void SetRotation(MonoObject* transformRef, float x, float y, float z);
    void SetRotationQuat(MonoObject* transformRef, glm::quat* rotation);
    Vector3 GetEulerAngles(MonoObject* transformRef);
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


    // MeshRenderer
	void SetMesh(MonoObject* meshRendererRef, MonoObject* meshRef);
	MonoObject* GetMesh(MonoObject* meshRendererRef);
    void SetMaterial(MonoObject* meshRendererRef, MonoObject* materialRef);
    MonoObject* GetMaterial(MonoObject* meshRendererRef);
	void SetColor(MonoObject* meshRendererRef, glm::vec3* color);
    void GetColor(MonoObject* meshRendererRef, glm::vec3* color);

    void Render(MonoObject* meshRendererRef);

}

#endif // ENGINE_BINDS_H