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

}



#endif