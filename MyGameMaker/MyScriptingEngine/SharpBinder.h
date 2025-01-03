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

}



#endif