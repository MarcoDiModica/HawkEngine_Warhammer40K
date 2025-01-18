#include "SharpBinder.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/Scene.h"
#include "ComponentLookUpTable.h"
#include "MonoEnvironment.h"
#include "../MyGameEditor/Root.h" //Cosa ilegal
#include <mono/metadata/object.h>
#include <string>

#include "../MyGameEngine/TransformComponent.h"
#include "glm/glm.hpp"

//TODO : cambiar estos metodos a que no usen el root


// componentes : Transform, Camera , Mesh etc. (Jordi)

// funciones del engine : (SceneClass )loadScene, DestroyGameObject, (Marco)

// *optional: Break Points in C#

// Separate editor functions to engine 

// lifecycle update of Script Components editor (Didac)


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

	obj->CsharpReference = monoObject; // store ref to C#ref to call lifecycle functions

	return monoObject;
}


MonoObject* SharpBinder::FindObjectByName(MonoString* name){

	char* C_name = mono_string_to_utf8(name);
	std::shared_ptr<GameObject> obj = Application->root->FindGOByName(C_name);
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");
	MonoObject* monoObject = mono_object_new(MonoEnvironment::m_ptr_MonoDomain, klass);

	return monoObject;
}

//               the sharpRef is an automatic parameter from C# with the C#instance of GO
MonoString* SharpBinder::GameObjectGetName(MonoObject* sharpRef) {

	std::string name = ConvertFromSharp(sharpRef)->GetName();

	MonoString* Mname = mono_string_new(MonoEnvironment::m_ptr_MonoDomain, name.c_str());

	return Mname;
}


GameObject* SharpBinder::ConvertFromSharp(MonoObject* sharpObj)
{
	if (sharpObj == nullptr) { return nullptr; }

	uintptr_t Cptr;
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg,"HawkEngine","GameObject");

	// Retrieve the C++ ptr GameObject from the C# gameObject's ptr parameter
	mono_field_get_value(sharpObj, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
	return reinterpret_cast<GameObject*>(Cptr);
}

template <class T>
T* SharpBinder::ConvertFromSharp(MonoObject* sharpComp) {

	if (sharpComp == nullptr) { return nullptr; }

	// DIEDIEDIEIDIEDEIDIEIDIIEDIEDEDIIDDIEDIDIDDEIDIEDID DIEDDIEID DIEDIE BEGONE !!!!!!!!
	auto t = std::type_index(typeid(T));

	auto it = SharpLookUpTable.find(std::type_index(typeid(Transform_Component)));
	if (it != SharpLookUpTable.end()) {
		std::string typeName = it->second;
		LOG(LogType::LOG_INFO, typeName.c_str());
	}
	else {
		LOG(LogType::LOG_INFO, "Type not found or value is empty");
	}
	std::string typeName = SharpLookUpTable [std::type_index(typeid(Transform_Component))];


	int z = SharpLookUpTable2[std::type_index(typeid(Transform_Component))];

	for (const auto& pair : SharpLookUpTable) {

		if (pair.first == std::type_index(typeid(T))) {
			typeName = pair.second;
		}

	}




	uintptr_t Cptr;
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", typeName.c_str());

	// Retrieve the C++ ptr GameObject from the C# gameObject's ptr parameter
	mono_field_get_value(sharpComp, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
	return reinterpret_cast<T*>(Cptr);

}

void SharpBinder::Destroy(MonoObject* object_to_destroy){

	if (object_to_destroy == nullptr) { return ; }

	uintptr_t Cptr;
	MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");

	// Retrieve the C++ ptr GameObject from the C# gameObject's ptr parameter
	mono_field_get_value(object_to_destroy, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
	GameObject* actor = reinterpret_cast<GameObject*>(Cptr);

	Application->root->RemoveGameObject(actor);

	//actor->Destroy();


}

void SharpBinder::ChangeScene(MonoString* scene_name) {

	char* C_name = mono_string_to_utf8(scene_name);

	Application->scene_serializer->DeSerialize(std::string(C_name));

}

void SharpBinder::GameObjectUpdate(GameObject* object) {

	if (object && object->CsharpReference) {
		MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.GameObject:Update", true);
		MonoClass* klass = mono_class_from_name(MonoEnvironment::m_ptr_GameAssemblyImg, "HawkEngine", "GameObject");

		MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
		// assign to C#object its ptr to C++ object
		//uintptr_t goPtr = reinterpret_cast<uintptr_t>(obj.get());

		

		mono_runtime_invoke(method, object->CsharpReference, nullptr, NULL);
	}

}

void SharpBinder::GameObjectAddChild(MonoObject* parent, MonoObject* child) {

	if (!parent || !child) { return; }

	GameObject* _parent = ConvertFromSharp(parent);
	GameObject* _child = ConvertFromSharp(child);

	_parent->AddChild(_child);

}

// Input Class functions

bool SharpBinder::GetKey(int keyID) {
	
	if (Application->input->GetKey(keyID) == KEY_REPEAT)
	{
		return true;
	}

	return false;
}

bool SharpBinder::GetKeyDown(int keyID) {

	if (Application->input->GetKey(keyID) == KEY_DOWN)
	{
		return true;
	}

	return false;
}

bool SharpBinder::GetKeyUp(int keyID) {

	if (Application->input->GetKey(keyID) == KEY_UP)
	{
		return true;
	}

	return false;
}

bool SharpBinder::GetMouseButton(int buttonID) {

	if (Application->input->GetMouseButton(buttonID) == KEY_REPEAT)
	{
		return true;
	}

	return false;
}

bool SharpBinder::GetMouseButtonDown(int buttonID) {

	if (Application->input->GetMouseButton(buttonID) == KEY_DOWN)
	{
		return true;
	}

	return false;
}

bool SharpBinder::GetMouseButtonUp(int buttonID) {

	if (Application->input->GetMouseButton(buttonID) == KEY_UP)
	{
		return true;
	}

	return false;
}

int SharpBinder::GetAxis(MonoString* axisName) {

	char* C_name = mono_string_to_utf8(axisName);

	return Application->input->GetAxis(C_name);

}

glm::vec3 SharpBinder::GetMousePosition() {

	glm::vec3 pos;
	pos.x = Application->input->GetMouseX();
	pos.y = Application->input->GetMouseY();
	pos.z = Application->input->GetMouseZ();

	return pos;
}

MonoObject* SharpBinder::GetSharpComponent(MonoObject* ref, MonoString* comoponent_name) {

	char* C_name = mono_string_to_utf8(comoponent_name);
	auto GO = ConvertFromSharp(ref);

	/* TODO change this to a dictionary , string component type */
	std::string componentName = std::string(C_name);

	if (componentName == "HawkEngine.Transform") {
		return GO->GetTransform()->GetSharp();
	}
	// Add other components

}

// Estoy bastante seguro de que necesito un transformador de Transform (cs) a TransformComponent (cpp)

//void SharpBinder::RegisterTransformBindings()
//{
//	mono_add_internal_call("HawkEngine.Transform::SetPosition", (const void*)&SharpBinder::SetPosition);
//	mono_add_internal_call("HawkEngine.Transform::GetPosition", (const void*)&SharpBinder::GetPosition);
//	mono_add_internal_call("HawkEngine.Transform::SetRotation", (const void*)&SharpBinder::SetRotation);
//	mono_add_internal_call("HawkEngine.Transform::SetRotationQuat", (const void*)&SharpBinder::SetRotationQuat);
//	mono_add_internal_call("HawkEngine.Transform::Rotate", (const void*)&SharpBinder::Rotate);
//	mono_add_internal_call("HawkEngine.Transform::RotateLocal", (const void*)&SharpBinder::RotateLocal);
//	mono_add_internal_call("HawkEngine.Transform::LookAt", (const void*)&SharpBinder::LookAt);
//	mono_add_internal_call("HawkEngine.Transform::SetScale", (const void*)&SharpBinder::SetScale);
//	mono_add_internal_call("HawkEngine.Transform::Scale", (const void*)&SharpBinder::Scale);
//	mono_add_internal_call("HawkEngine.Transform::TranslateLocal", (const void*)&SharpBinder::TranslateLocal);
//	mono_add_internal_call("HawkEngine.Transform::AlignToGlobalUp", (const void*)&SharpBinder::AlignToGlobalUp);
//	mono_add_internal_call("HawkEngine.Transform::SetForward", (const void*)&SharpBinder::SetForward);
//}
//
void SharpBinder::SetPosition(MonoObject* transformRef, float x, float y, float z)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->SetPosition(glm::vec3(x, y, z));
}

void SharpBinder::GetPosition(MonoObject* transformRef, glm::vec3* position)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) *position = transform->GetPosition();
}

void SharpBinder::SetRotation(MonoObject* transformRef, float x, float y, float z)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->SetRotation(glm::vec3(x, y, z));
}

void SharpBinder::SetRotationQuat(MonoObject* transformRef, glm::quat* rotation)
{
	/*Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->SetRotation(*rotation);*/
}

void SharpBinder::Rotate(MonoObject* transformRef, float radians, glm::vec3* axis)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->Rotate(radians, *axis);
}

void SharpBinder::RotateLocal(MonoObject* transformRef, float radians, glm::vec3* axis)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->RotateLocal(radians, *axis);
}

void SharpBinder::LookAt(MonoObject* transformRef, glm::vec3* target)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->LookAt(*target);
}

void SharpBinder::SetScale(MonoObject* transformRef, float x, float y, float z)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->SetScale(glm::vec3(x, y, z));
}

void SharpBinder::Scale(MonoObject* transformRef, float x, float y, float z)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->Scale(glm::vec3(x, y, z));
}

void SharpBinder::TranslateLocal(MonoObject* transformRef, float x, float y, float z)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->TranslateLocal(glm::vec3(x, y, z));
}

void SharpBinder::AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->AlignToGlobalUp(*worldUp);
}

void SharpBinder::SetForward(MonoObject* transformRef, glm::vec3* forward)
{
	Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
	if (transform) transform->SetForward(*forward);
}
