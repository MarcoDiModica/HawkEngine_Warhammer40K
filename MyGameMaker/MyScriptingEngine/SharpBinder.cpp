#include "SharpBinder.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"

#include "MonoEnvironment.h"
#include "../MyGameEditor/Root.h" //Cosa ilegal
#include <mono/metadata/object.h>
#include <string>

//TODO : cambiar estos metodos a que no usen el root

MonoObject* SharpBinder::CreateGameObjectSharp(MonoString* name) {
	char* C_name = mono_string_to_utf8(name);
	std::shared_ptr<GameObject> obj = Application->root->CreateCameraObject(std::string(C_name));

	intptr_t C_Object = reinterpret_cast<intptr_t>(obj.get());
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");
	MonoObject* monoObject = mono_object_new(MonoEnvironment::m_ptr_MonoDomain, klass);

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.GameObject:.ctor(string,uintptr)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	// assign to C#object its ptr to C++ object
	uintptr_t goPtr = reinterpret_cast<uintptr_t>(obj.get());

	void* args[2];
	args[0] = mono_string_new( MonoEnvironment::m_ptr_MonoDomain  ,obj->GetName().c_str() );
	args[1] = &goPtr;

	mono_runtime_invoke(method, monoObject, args, NULL);

	return monoObject;
}


MonoObject* SharpBinder::FindObjectByName(MonoString* name){

	char* C_name = mono_string_to_utf8(name);
	std::shared_ptr<GameObject> obj = Application->root->FindGOByName(C_name);
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");
	MonoObject* monoObject = mono_object_new(MonoEnvironment::m_ptr_MonoDomain, klass);

	return monoObject;
}

MonoString* SharpBinder::GameObjectGetName(MonoObject* sharpRef) {



	return nullptr;
}
