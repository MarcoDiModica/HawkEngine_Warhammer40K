#include "EngineBinds.h"
#include "MonoManager.h"
#include "ComponentMapper.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/Root.h" //QUITAR: CAMBIAR POR ROOT DEL ENGINE

#include <mono/metadata/debug-helpers.h>

// GameObject
MonoObject* EngineBinds::CreateGameObjectSharp(MonoString* name) {
    char* C_name = mono_string_to_utf8(name);
    std::shared_ptr<GameObject> obj = Application->root->CreateCameraObject(std::string(C_name)); //SUSTITUIR POR ROOT DEL ENGINE

    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);

    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.GameObject:.ctor(string,uintptr)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    // assign to C#object its ptr to C++ object
    uintptr_t goPtr = reinterpret_cast<uintptr_t>(obj.get());

    void* args[2];
    args[0] = mono_string_new(MonoManager::GetInstance().GetDomain(), obj->GetName().c_str());
    args[1] = &goPtr;

    mono_runtime_invoke(method, monoObject, args, NULL);

    obj->CsharpReference = monoObject; // store ref to C#ref to call lifecycle functions

    return monoObject;
}

MonoString* EngineBinds::GameObjectGetName(MonoObject* sharpRef) {
    std::string name = ConvertFromSharp(sharpRef)->GetName();
    return mono_string_new(MonoManager::GetInstance().GetDomain(), name.c_str());
}

GameObject* EngineBinds::ConvertFromSharp(MonoObject* sharpObj) {
    if (sharpObj == nullptr) {
        return nullptr;
    }

    uintptr_t Cptr;
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    mono_field_get_value(sharpObj, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
    return reinterpret_cast<GameObject*>(Cptr);
}

void EngineBinds::GameObjectAddChild(MonoObject* parent, MonoObject* child) {
    if (!parent || !child) {
        return;
    }

    GameObject* _parent = ConvertFromSharp(parent);
    GameObject* _child = ConvertFromSharp(child);

    _parent->AddChild(_child);
}

void EngineBinds::Destroy(MonoObject* object_to_destroy) {
    if (object_to_destroy == nullptr) {
        return;
    }

    uintptr_t Cptr;
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    mono_field_get_value(object_to_destroy, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
    GameObject* actor = reinterpret_cast<GameObject*>(Cptr);
    Application->root->RemoveGameObject(actor); //SUSTITUIR POR ROOT DEL ENGINE
}

MonoObject* EngineBinds::GetSharpComponent(MonoObject* ref, MonoString* comoponent_name)
{
    char* C_name = mono_string_to_utf8(comoponent_name);
    auto GO = ConvertFromSharp(ref);

    /* TODO change this to a dictionary , string component type */
    std::string componentName = std::string(C_name);

    if (componentName == "HawkEngine.Transform") {
        return GO->GetTransform()->GetSharp();
    }
    // Add other components
    return nullptr;
}

// Input
bool EngineBinds::GetKey(int keyID) {
    return Application->input->GetKey(keyID) == KEY_REPEAT;
}

bool EngineBinds::GetKeyDown(int keyID) {
    return Application->input->GetKey(keyID) == KEY_DOWN;
}

bool EngineBinds::GetKeyUp(int keyID) {
    return Application->input->GetKey(keyID) == KEY_UP;
}

bool EngineBinds::GetMouseButton(int buttonID) {
    return Application->input->GetMouseButton(buttonID) == KEY_REPEAT;
}

bool EngineBinds::GetMouseButtonDown(int buttonID) {
    return Application->input->GetMouseButton(buttonID) == KEY_DOWN;
}

bool EngineBinds::GetMouseButtonUp(int buttonID) {
    return Application->input->GetMouseButton(buttonID) == KEY_UP;
}

int EngineBinds::GetAxis(MonoString* axisName) {
    char* C_name = mono_string_to_utf8(axisName);
    return Application->input->GetAxis(C_name);
}

glm::vec3 EngineBinds::GetMousePosition() {
    glm::vec3 pos;
    pos.x = Application->input->GetMouseX();
    pos.y = Application->input->GetMouseY();
    pos.z = Application->input->GetMouseZ();
    return pos;
}

// Transform
void EngineBinds::SetPosition(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetPosition(glm::vec3(x, y, z));
}

void EngineBinds::GetPosition(MonoObject* transformRef, glm::vec3* position) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) *position = transform->GetPosition();
}

void EngineBinds::SetRotation(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetRotation(glm::vec3(x, y, z));
}

void EngineBinds::SetRotationQuat(MonoObject* transformRef, glm::quat* rotation) {
    /*Transform_Component* transform = ConvertFromSharp<Transform_Component>(transformRef);
    if (transform) transform->SetRotation(*rotation);*/
}

void EngineBinds::Rotate(MonoObject* transformRef, float radians, glm::vec3* axis) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->Rotate(radians, *axis);
}

void EngineBinds::RotateLocal(MonoObject* transformRef, float radians, glm::vec3* axis) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->RotateLocal(radians, *axis);
}

void EngineBinds::LookAt(MonoObject* transformRef, glm::vec3* target) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->LookAt(*target);
}

void EngineBinds::SetScale(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetScale(glm::vec3(x, y, z));
}

void EngineBinds::Scale(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->Scale(glm::vec3(x, y, z));
}

void EngineBinds::TranslateLocal(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->TranslateLocal(glm::vec3(x, y, z));
}

void EngineBinds::AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->AlignToGlobalUp(*worldUp);
}

void EngineBinds::SetForward(MonoObject* transformRef, glm::vec3* forward) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetForward(*forward);
}

void EngineBinds::BindEngine() {
    // GameObject
    mono_add_internal_call("HawkEngine.EngineCalls::CreateGameObject", (const void*)CreateGameObjectSharp);
    mono_add_internal_call("HawkEngine.GameObject::GetName", (const void*)GameObjectGetName);
    mono_add_internal_call("HawkEngine.GameObject::AddChild", (const void*)GameObjectAddChild);
    mono_add_internal_call("HawkEngine.EngineCalls::Destroy", (const void*)Destroy);
    mono_add_internal_call("HawkEngine.GameObject::TryGetComponent", (const void*)GetSharpComponent);

    // Input
    mono_add_internal_call("HawkEngine.Input::GetKey", (const void*)GetKey);
    mono_add_internal_call("HawkEngine.Input::GetKeyDown", (const void*)GetKeyDown);
    mono_add_internal_call("HawkEngine.Input::GetKeyUp", (const void*)GetKeyUp);
    mono_add_internal_call("HawkEngine.Input::GetMouseButton", (const void*)GetMouseButton);
    mono_add_internal_call("HawkEngine.Input::GetMouseButtonDown", (const void*)GetMouseButtonDown);
    mono_add_internal_call("HawkEngine.Input::GetMouseButtonUp", (const void*)GetMouseButtonUp);
    mono_add_internal_call("HawkEngine.Input::GetAxis", (const void*)GetAxis);
    mono_add_internal_call("HawkEngine.Input::GetMousePosition", (const void*)GetMousePosition);

    // Transform
    mono_add_internal_call("HawkEngine.Transform::SetPosition", (const void*)&EngineBinds::SetPosition);
    mono_add_internal_call("HawkEngine.Transform::GetPosition", (const void*)&EngineBinds::GetPosition);
    mono_add_internal_call("HawkEngine.Transform::SetRotation", (const void*)&EngineBinds::SetRotation);
    mono_add_internal_call("HawkEngine.Transform::SetRotationQuat", (const void*)&EngineBinds::SetRotationQuat);
    mono_add_internal_call("HawkEngine.Transform::Rotate", (const void*)&EngineBinds::Rotate);
    mono_add_internal_call("HawkEngine.Transform::RotateLocal", (const void*)&EngineBinds::RotateLocal);
    mono_add_internal_call("HawkEngine.Transform::LookAt", (const void*)&EngineBinds::LookAt);
    mono_add_internal_call("HawkEngine.Transform::SetScale", (const void*)&EngineBinds::SetScale);
    mono_add_internal_call("HawkEngine.Transform::Scale", (const void*)&EngineBinds::Scale);
    mono_add_internal_call("HawkEngine.Transform::TranslateLocal", (const void*)&EngineBinds::TranslateLocal);
    mono_add_internal_call("HawkEngine.Transform::AlignToGlobalUp", (const void*)&EngineBinds::AlignToGlobalUp);
    mono_add_internal_call("HawkEngine.Transform::SetForward", (const void*)&EngineBinds::SetForward);
}

template <class T>
T* EngineBinds::ConvertFromSharpComponent(MonoObject* sharpComp) {
    if (sharpComp == nullptr) {
        return nullptr;
    }

    auto& mapper = MonoManager::GetInstance().GetMapper();
    std::string typeName = mapper.GetMappedName(std::type_index(typeid(T)));

    uintptr_t Cptr;
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", typeName.c_str());

    std::cerr << "Buscando clase: " << typeName << std::endl;

    if (!klass) {
        throw std::runtime_error("Clase no encontrada en el assembly de C#: " + typeName);
    }

    MonoClassField* field = mono_class_get_field_from_name(klass, "CplusplusInstance");

    if (!field) {
        throw std::runtime_error("Campo CplusplusInstance no encontrado en la clase: " + typeName);
    }

    mono_field_get_value(sharpComp, field, &Cptr);
    return reinterpret_cast<T*>(Cptr);
}