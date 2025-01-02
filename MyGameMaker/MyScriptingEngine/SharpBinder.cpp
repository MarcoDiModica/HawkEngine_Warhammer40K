#include "SharpBinder.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"

#include "MonoEnvironment.h"
#include "../MyGameEditor/Root.h" //Cosa ilegal
#include <mono/metadata/object.h>
#include <string>

//MonoObject* SharpBinder::CreateGameObjectSharp(std::string name) {
//
//	std::shared_ptr<GameObject> obj = Application->root->CreateCameraObject(name);
//
//	intptr_t C_Object = reinterpret_cast<intptr_t>(obj.get());
//	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");
//	MonoObject* monoObject = mono_object_new(MonoEnvironment::m_ptr_MonoDomain, klass);
//
//	return monoObject;
//
//}



