#pragma once
#ifndef ENGINE_BINDS_H
#define ENGINE_BINDS_H

#include <mono/metadata/object.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <SDL2/SDL_gamecontroller.h>
#include <MyGameEngine/Tweening.h>

class GameObject;

struct Vector3 {
    float X, Y, Z; 
};

namespace EngineBinds {
    void BindEngine();

    // GameObject
    //MonoObject* CreateGameObjectSharp(MonoString* name, GameObject* Cgo = nullptr);

    MonoObject* CreateGameObjectSharp(MonoString* name, GameObject* Cgo = nullptr);

    MonoObject* GetGameObject(MonoObject* ref);

    GameObject* ConvertFromSharp(MonoObject* sharpObj);
    void GameObjectAddChild(MonoObject* parent, MonoObject* child);
    void Destroy(MonoObject* object_to_destroy);
    MonoObject* GetSharpComponent(MonoObject* ref, MonoString* comoponent_name);
    MonoObject* AddSharpComponent(MonoObject* ref, int component);
    GameObject* GetScriptOwner(MonoObject* ref);
    template <class T>
    T* ConvertFromSharpComponent(MonoObject* sharpComp);

	void AddScript(MonoObject* ref, MonoString* scriptName);
	void SetActive(MonoObject* ref, bool active);

    MonoString* GameObjectGetName(MonoObject* sharpRef);
    MonoString* GameObjectGetTag(MonoObject* sharpRef);
    void SetName(MonoObject* ref, MonoString* sharpName);
	MonoString* GetTag(MonoObject* ref);
	void SetTag(MonoObject* ref, MonoString* tag);
    MonoObject* GetGameObjectByName(MonoString* name);	
    MonoObject* GameObjectFindChild(MonoObject* parent, MonoString* name);
    void GameObjectSetActive(MonoObject* ref, bool active);
    MonoObject* InstantiatePrefab(MonoObject* prefabObj, MonoObject* parentTransformObj, bool worldPositionStays);
	bool GameObjectIsActive(MonoObject* ref);

    // Input
    bool GetKey(int keyID);
    bool GetKeyDown(int keyID);
    bool GetKeyUp(int keyID);
    bool GetMouseButton(int buttonID);
    bool GetMouseButtonDown(int buttonID);
    bool GetMouseButtonUp(int buttonID);
    int GetAxis(MonoString* axisName);
    glm::vec3 GetMousePosition();

     // Controller
    bool GetControllerButton(int buttonID);
    bool GetControllerButtonDown(int buttonID);
    bool GetControllerButtonUp(int buttonID);
    float GetControllerAxis(int gamepadIndex, int axis);

    // Transform
    void SetPosition(MonoObject* transformRef, float x, float y, float z);
    void SetLocalPosition(MonoObject* transformRef, float x, float y, float z);
    Vector3 GetPosition(MonoObject* transformRef);
    Vector3 GetLocalPosition(MonoObject* transformRef);
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
    Vector3 GetForward(MonoObject* transformRef);
    void SetForward(MonoObject* transformRef, glm::vec3* forward);
    Vector3 GetUp(MonoObject* transformRef);
   // Quaternion GetRotationQuat(MonoObject* transformRef);
    void SetUp(MonoObject* transformRef, glm::vec3* newUp);
    Vector3 GetRight(MonoObject* transformRef);
    void SetRight(MonoObject* transformRef, glm::vec3* newIn);
    Vector3 GetLocalScale(MonoObject* transformRef);
    void SetLocalScale(MonoObject* transformRef, Vector3 scale);

    // Camera
    void SetCameraFieldOfView(MonoObject* cameraRef, double fov);
    void SetCameraNearClipPlane(MonoObject* cameraRef, float nearClipPlane);
    void SetCameraFarClipPlane(MonoObject* cameraRef, float farClipPlane);
    void SetCameraAspectRatio(MonoObject* cameraRef, float aspectRatio);
    void SetCameraOrthographicSize(MonoObject* cameraRef, float orthographicSize);
    void SetCameraProjectionType(MonoObject* cameraRef, int projectionType);
    void SetFollowTarget(MonoObject* cameraRef, MonoObject* target, glm::vec3* offset, float distance, bool followX, bool followY, bool followZ, float smoothness);
    void SetDistance(MonoObject* cameraRef, float distance);
    void SetOffset(MonoObject* cameraRef, glm::vec3* offset);

    // MeshRenderer
	void SetMesh(MonoObject* meshRendererRef, MonoObject* meshRef);
    void SetCubeMesh(MonoObject* meshRendererRef);
	MonoObject* GetMesh(MonoObject* meshRendererRef);
    void SetMaterial(MonoObject* meshRendererRef, MonoObject* materialRef);
    MonoObject* GetMaterial(MonoObject* meshRendererRef);
	void SetColor(MonoObject* meshRendererRef, glm::vec3* color);
    void GetColor(MonoObject* meshRendererRef, glm::vec3* color);
    void Render(MonoObject* meshRendererRef);
    

    //Physics
    MonoArray* OverlapSphere(glm::vec3* position, float radius, MonoString* tag);

    //Physics Collider
    void SetTrigger(MonoObject* colliderRef, bool trigger);
    bool IsTrigger(MonoObject* colliderRef);
    glm::vec3 GetColliderPosition(MonoObject* colliderRef);
    void SetColliderPosition(MonoObject* colliderRef, glm::vec3* position);
    glm::quat GetColliderRotation(MonoObject* colliderRef);
    void SetColliderRotation(MonoObject* colliderRef, glm::quat* rotation);
    glm::vec3 GetColliderSize(MonoObject* colliderRef);
    void SetColliderSize(MonoObject* colliderRef, glm::vec3* size);
    void SetColliderActive(MonoObject* colliderRef, bool active);
    void SnapColliderToPosition(MonoObject* colliderRef);

    //Physics Rigidbody
    void SetVelocity(MonoObject* rigidbodyRef, glm::vec3* velocity);
    glm::vec3 GetVelocity(MonoObject* rigidbodyRef);
    void AddForce(MonoObject* rigidbodyRef, glm::vec3* force);
    void SetMass(MonoObject* rigidbodyRef, float mass);
    float GetMass(MonoObject* rigidbodyRef);
    void SetFriction(MonoObject* rigidbodyRef, float friction);
    float GetFriction(MonoObject* rigidbodyRef);
    void SetGravity(MonoObject* rigidbodyRef, glm::vec3* gravity);
    glm::vec3 GetGravity(MonoObject* rigidbodyRef);
    void SetDamping(MonoObject* rigidbodyRef, float linearDamping, float angularDamping);
    glm::vec2 GetDamping(MonoObject* rigidbodyRef);
    void SetKinematic(MonoObject* rigidbodyRef, bool isKinematic);
    bool IsKinematic(MonoObject* rigidbodyRef);
    void EnableContinuousCollision(MonoObject* rigidbodyRef);

    // Raycast
	MonoObject* Raycast(glm::vec3* origin, glm::vec3* direction, float maxDistance, glm::vec3& hitPoint, glm::vec3& normal, float& distance);
    
    //Audio
	static int AudioPlay(MonoString* path, bool loop);
	static int AudioPlayOneShot(MonoString* path);
	static void AudioStop(int audioId);
	static void AudioStopPath(MonoString* path);
	static void AudioPause(int audioId);
	static void AudioPausePath(MonoString* path);
	static void AudioResume(int audioId);
	static void AudioResumePath(MonoString* path);
	static int AudioPlayMusic(MonoString* path);
	static void AudioStopMusic(MonoString* path);
	static void AudioStopAllMusic();
	static float AudioGetMasterVolume();
	static void AudioSetMasterVolume(float volume);
	static float AudioGetMusicVolume();
	static void AudioSetMusicVolume(float volume);
	static float AudioGetSfxVolume();
	static void AudioSetSfxVolume(float volume);
	static void AudioSetVolumeById(int audioId, float volume);
	static void AudioSetVolumeByPath(MonoString* path, float volume);
	static void AudioStopAll();
	static void AudioPauseAll();
	static void AudioResumeAll();
	static void AudioSchedulePlay(MonoString* path, float delay, bool loop);

    //UIImage
    void SetTexture(MonoObject* uiImageRef, MonoString* path);
	void SetImageEnabled(MonoObject* uiImageRef, bool enabled);
	void SetImageHasAnimation(MonoObject* uiImageRef, bool hasAnimation);
	void SetImageAnimationSpeed(MonoObject* uiImageRef, float speed);
	void SetImageAnimationIndexLimit(MonoObject* uiImageRef, int indexLimit);
	void SetImageAnimation(MonoObject* uiImageRef, int index);
	void SetImageSpriteSize(MonoObject* uiImageRef, float width, float height);
	void SetImageAnimIndex(MonoObject* uiImageRef, int index);
	void PlayStopAnimation(MonoObject* uiImageRef, bool play);

	//UIButton
    int GetState(MonoObject* uiButtonRef);

	//UITransform
	void SetUIScale(MonoObject* uiTransformRef, glm::vec3* scale);
  
    //SkeletalAnimation
	void SetAnimationSpeed(MonoObject* skeletalAnimationRef, float speed);
	float GetAnimationSpeed(MonoObject* skeletalAnimationRef);
	void SetAnimation(MonoObject* skeletalAnimationRef, int index);
	int GetAnimationIndex(MonoObject* skeletalAnimationRef);
	float GetAnimationLength(MonoObject* skeletalAnimationRef);
	float GetAnimationTime(MonoObject* skeletalAnimationRef);
	void SetAnimationPlayTime(MonoObject* skeletalAnimationRef, float time);
	void SetAnimationPlayState(MonoObject* skeletalAnimationRef, bool play);
	bool GetAnimationPlayState(MonoObject* skeletalAnimationRef);
	void TransitionAnimations(MonoObject* skeletalAnimationRef, int oldAnim, int newAnim, float timeToTransition);
	void SetLoop(MonoObject* skeletalAnimationRef, bool isLoop);
	void PlayAnimOnce(MonoObject* skeletalAnimationRef, int index, float timeToTransitionAnim);
	bool IsAnimationFinished(MonoObject* skeletalAnimationRef);

	//Tweening
    void DOMove(MonoObject* transformRef, glm::vec3* targetPosition, float duration, Modes mode);

    //cambia los de UI a que sean DOMoveUI, es mas intuitivo para el usuario !!!!!IMPORTANTE!!!!!
    void DOMoveUI(MonoObject* transformRef, glm::vec3* targetPosition, float duration, Modes mode);
	void DOMoveX(MonoObject* transformRef, float targetX, float duration, Modes mode);
	void DOMoveXUI(MonoObject* uiTransformRef, float targetX, float duration, Modes mode);
	void DOMoveY(MonoObject* transformRef, float targetY, float duration, Modes mode);
	void DOMoveYUI(MonoObject* uiTransformRef, float targetY, float duration, Modes mode);
	void DOMoveZ(MonoObject* transformRef, float targetZ, float duration, Modes mode);
	void DOMoveZUI(MonoObject* uiTransformRef, float targetZ, float duration, Modes mode);
	void DORotate(MonoObject* transformRef, glm::vec3* targetRotation, float duration, Modes mode);
	void DORotateX(MonoObject* transformRef, float targetX, float duration, Modes mode);
	void DORotateY(MonoObject* transformRef, float targetY, float duration, Modes mode);
	void DORotateZ(MonoObject* transformRef, float targetZ, float duration, Modes mode);
	void DOScale(MonoObject* transformRef, glm::vec3* targetScale, float duration, Modes mode);
	void DOScaleUI(MonoObject* uiTransformRef, glm::vec3* targetScale, float duration, Modes mode);
	void DOScaleX(MonoObject* transformRef, float targetX, float duration, Modes mode);
	void DOScaleY(MonoObject* transformRef, float targetY, float duration, Modes mode);
	void DOScaleZ(MonoObject* transformRef, float targetZ, float duration, Modes mode);
	void DOScaleXUI(MonoObject* uiTransformRef, float targetX, float duration, Modes mode);
	void DOScaleYUI(MonoObject* uiTransformRef, float targetY, float duration, Modes mode);
	void DOScaleZUI(MonoObject* uiTransformRef, float targetZ, float duration, Modes mode);
    void DOColor(glm::vec4* color, const glm::vec4 startColor, const glm::vec4 targetColor, float duration, Modes mode);
	void DOVec3(glm::vec3* vec, const glm::vec3 start, const glm::vec3 target, float duration, Modes mode);
    void DOValue(float* value, float start, float target, float duration, Modes mode);
    void CleanAllTweens();

    //SceneManagement
    bool LoadScene(MonoString* sceneName);
    void SetScenePlay();

    //VFX (particles)
    void ApplyPreset(MonoObject* particleRef, int presetName);
	void SetOneShot(MonoObject* particleRef, bool oneShot);
	void PlayParticle(MonoObject* particleRef);
	void StopParticle(MonoObject* particleRef);
	void EmitBurst(MonoObject* particleRef, int burstCount);
}

#endif // ENGINE_BINDS_H