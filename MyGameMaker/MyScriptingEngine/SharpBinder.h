#pragma once
#ifndef SHARP_BINDER_H
#define SHARP_BINDER_H
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>
#include "../MyGameEditor/Root.h" //Cosa ilegal


namespace SharpBinder
{

	MonoObject* CreateGameObjectSharp(MonoString* name);

	MonoObject* FindObjectByName(MonoString* name);


	MonoString* GameObjectGetName(MonoObject* sharpRef);


	/// <summary>
	/// Retrieve the GameObject cpp reference from a C# GameObject
	/// </summary>
	/// <param name="sharpObj"></param>
	/// <returns></returns>
	GameObject* ConvertFromSharp(MonoObject* sharpObj);

	void Destroy(MonoObject* object_to_destroy);

	void ChangeScene(MonoString* scene_name);

	void GameObjectUpdate(GameObject* object);

	void GameObjectAddChild(MonoObject* parent, MonoObject* child);

	// Input Class
	bool GetKey(int keyID);
	bool GetKeyDown(int keyID);
	bool GetKeyUp(int keyID);
	bool GetMouseButton(int buttonID);
	bool GetMouseButtonDown(int buttonID);
	bool GetMouseButtonUp(int buttonID);
	int GetAxis(MonoString* axisName);

	// Pongo esto aquí pero supongo que lo deberíamos delegar a otro sitio
	void RegisterTransformBindings();

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

	//Transform_Component* ConvertFromSharpTransform(Transform* sharpObj);

}



#endif