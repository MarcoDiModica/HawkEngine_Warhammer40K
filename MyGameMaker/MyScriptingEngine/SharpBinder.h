#pragma once
#ifndef SHARP_BINDER_H
#define SHARP_BINDER_H
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"

#include <mono/metadata/object.h>
#include "../MyGameEditor/Root.h" //Cosa ilegal


namespace SharpBinder
{

	MonoObject* CreateGameObjectSharp(std::string name);










}


#endif