#include "EngineBinds.h"
#include "MonoManager.h"
#include "ComponentMapper.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/SceneManager.h"
#include "../MyGameEngine/InputEngine.h"
#include "ScriptComponent.h"
#include <mono/metadata/debug-helpers.h>


// GameObject

MonoObject* EngineBinds::GetGameObject(MonoObject* ref) {

    return GetScriptOwner(ref)->GetSharp();

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

MonoObject* EngineBinds::CreateGameObjectSharp(MonoString* name, GameObject* Cgo) {
    char* C_name = mono_string_to_utf8(name);

    if (Cgo == nullptr) {
        auto go = SceneManagement->CreateGameObject(C_name); 
        Cgo = go.get();
    }


    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);

    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.GameObject:.ctor(string,uintptr)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    // assign to C#object its ptr to C++ object
    uintptr_t goPtr = reinterpret_cast<uintptr_t>(Cgo);

    void* args[2];
    args[0] = mono_string_new(MonoManager::GetInstance().GetDomain(), Cgo->GetName().c_str());
    args[1] = &goPtr;

    mono_runtime_invoke(method, monoObject, args, NULL);

    Cgo->CsharpReference = monoObject; // store ref to C#ref to call lifecycle functions

    return monoObject;
}

GameObject* EngineBinds::GetScriptOwner(MonoObject* ref) {
    if (ref == nullptr) {
        return nullptr;
    }

    uintptr_t Cptr;
    MonoClass* klass = MonoManager::GetInstance().GetClass("", "MonoBehaviour");
    mono_field_get_value(ref, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
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

	SceneManagement->RemoveGameObject(ConvertFromSharp(object_to_destroy));

    //uintptr_t Cptr;
    //MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    //mono_field_get_value(object_to_destroy, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
    //GameObject* actor = reinterpret_cast<GameObject*>(Cptr);
    //SceneManagement->RemoveGameObject(actor); //SUSTITUIR POR ROOT DEL ENGINE
}

MonoObject* EngineBinds::GetSharpComponent(MonoObject* ref, MonoString* component_name)
{
    char* C_name = mono_string_to_utf8(component_name);
    auto GO = ConvertFromSharp(ref);

    std::string componentName = std::string(C_name);

    for (const auto& scriptComponent : GO->scriptComponents) {
        if (scriptComponent->GetTypeName() == componentName) {
            return scriptComponent->GetSharpObject();
        }
    }

    if (componentName == "HawkEngine.Transform") {
        return GO->GetTransform()->GetSharp();
	}
	else if (componentName == "HawkEngine.MeshRenderer") {
		return GO->GetComponent<MeshRenderer>()->GetSharp();
	}
	else if (componentName == "HawkEngine.Camera") {
		return GO->GetComponent<CameraComponent>()->GetSharp();
	}


	
    // Add other components
    return nullptr;
}

MonoObject* EngineBinds::AddSharpComponent(MonoObject* ref, int component) {

    Component* _component = nullptr;
    auto go = ConvertFromSharp(ref);

    switch (component) {
    case 0: _component = static_cast<Component*>( go->AddComponent<Transform_Component>()); 
        break;
    case 1: _component = static_cast<Component*>(go->AddComponent<MeshRenderer>());
        break;
    case 2: _component = static_cast<Component*>(go->AddComponent<CameraComponent>());
        break;
   }

    // loop through all the scripts and grant them unique ids
    for (auto it = MonoManager::GetInstance().scriptIDs.begin(); it != MonoManager::GetInstance().scriptIDs.end(); ++it) {

        if (component == it->second) {

            auto script = go->AddComponent<ScriptComponent>();
            script->LoadScript(it->first);
        }

    }

    return _component->GetSharp();

}


void EngineBinds::SetName(MonoObject* ref, MonoString* sharpName) {

    char* C_name = mono_string_to_utf8(sharpName);
    ConvertFromSharp(ref)->SetName(std::string(C_name));
}

// Input
bool EngineBinds::GetKey(int keyID) {
    return InputManagement->GetKey(keyID) == KEY_REPEAT;
}

bool EngineBinds::GetKeyDown(int keyID) {
    return InputManagement->GetKey(keyID) == KEY_DOWN;
}

bool EngineBinds::GetKeyUp(int keyID) {
    return InputManagement->GetKey(keyID) == KEY_UP;
}

bool EngineBinds::GetMouseButton(int buttonID) {
    return InputManagement->GetMouseButton(buttonID) == KEY_REPEAT;
}

bool EngineBinds::GetMouseButtonDown(int buttonID) {
    return InputManagement->GetMouseButton(buttonID) == KEY_DOWN;
}

bool EngineBinds::GetMouseButtonUp(int buttonID) {
    return InputManagement->GetMouseButton(buttonID) == KEY_UP;
}

int EngineBinds::GetAxis(MonoString* axisName) {
    char* C_name = mono_string_to_utf8(axisName);
    return InputManagement->GetAxis(C_name);
}

// Controller
bool EngineBinds::GetControllerButton(int buttonID) {
    return InputManagement->controller_buttons[buttonID] == KEY_REPEAT;
}

bool EngineBinds::GetControllerButtonDown(int buttonID) {
    return InputManagement->controller_buttons[buttonID] == KEY_DOWN;
}

bool EngineBinds::GetControllerButtonUp(int buttonID) {
    return InputManagement->controller_buttons[buttonID] == KEY_UP;
}

float EngineBinds::GetJoystickAxis(int gamepadIndex, int axis) {
    if (axis == 0) {
        return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTX] / 32767.0f);
    }
	else if (axis == 1) {
		return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTY] / 32767.0f);
	}
	else if (axis == 2) {
		return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTX] / 32767.0f);
	}
	else if (axis == 3) {
		return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTY] / 32767.0f);
	}
	else if (axis == 4) {
		return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERLEFT] / 32767.0f);
	}
	else if (axis == 5) {
		return (InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] / 32767.0f);
	}
	else {
		return 0.0f;
	}
}

glm::vec3 EngineBinds::GetMousePosition() {
    glm::vec3 pos;
    pos.x = InputManagement->GetMouseX();
    pos.y = InputManagement->GetMouseY();
    pos.z = InputManagement->GetMouseZ();
    return pos;
}

// Transform
void EngineBinds::SetPosition(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetPosition(glm::vec3(x, y, z));
}

void EngineBinds::SetLocalPosition(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetLocalPosition(glm::vec3(x, y, z));
}

Vector3 EngineBinds::GetPosition(MonoObject* transformRef) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    glm::dvec3 p = transform->GetPosition();
    return Vector3 {(float) p.x,(float)p.y,(float)p.z };
}

Vector3 EngineBinds::GetLocalPosition(MonoObject* transformRef) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    glm::dvec3 p = transform->GetLocalPosition();
    return Vector3{ (float)p.x,(float)p.y,(float)p.z };
}

void EngineBinds::SetRotation(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetRotation(glm::vec3(x, y, z));
}

Vector3 EngineBinds::GetEulerAngles(MonoObject* transformRef) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    
    auto v = transform->GetRotation();
    return Vector3{(float) v.x,(float)v.y,(float)v.z };
}

void EngineBinds::SetRotationQuat(MonoObject* transformRef, glm::quat* rotation) {
    Transform_Component* transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetRotationQuat(*rotation);
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

Vector3 EngineBinds::GetForward(MonoObject* transformRef) {

    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    auto forward = transform->GetForward();
    return Vector3{ (float) forward.x,(float) forward.y , (float) forward.z };
}

// Camera Class functions

void EngineBinds::SetCameraFieldOfView(MonoObject* cameraRef, float fov) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
    camera->fov = fov;

}

void EngineBinds::SetCameraNearClipPlane(MonoObject* cameraRef, float nearClipPlane) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
    camera->zNear = nearClipPlane;

}

void EngineBinds::SetCameraFarClipPlane(MonoObject* cameraRef, float farClipPlane) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
    camera->zFar = farClipPlane;

}

void EngineBinds::SetCameraAspectRatio(MonoObject* cameraRef, float aspectRatio) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
    camera->aspect = aspectRatio;

}

void EngineBinds::SetCameraOrthographicSize(MonoObject* cameraRef, float orthographicSize) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
    camera->orthoSize = orthographicSize;

}

void EngineBinds::SetCameraProjectionType(MonoObject* cameraRef, int projectionType) {

    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);

    if (projectionType == 0) {
        camera->projectionType = ProjectionType::Perspective;
    }
    else {
        camera->projectionType = ProjectionType::Orthographic;
    }

}

// MeshRenderer
void EngineBinds::SetMesh(MonoObject* meshRendererRef, MonoObject* meshRef)
{
    if (!meshRendererRef || !meshRef) return;

    MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
	Mesh* mesh = ConvertFromSharpComponent<Mesh>(meshRef);

    if (meshRenderer && mesh)
    {
        meshRenderer->SetMesh(std::shared_ptr<Mesh>(mesh));
    }
}

void EngineBinds::SetCubeMesh(MonoObject* meshRendererRef)
{
    if (!meshRendererRef) return;

	MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
	if (!meshRenderer) return;

	meshRenderer->SetMesh(Mesh::CreateCube());
}

MonoObject* EngineBinds::GetMesh(MonoObject* meshRendererRef)
{
    if (!meshRendererRef) return nullptr;

	MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
    if (!meshRendererRef) return nullptr;

    std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();
	if (!mesh) return nullptr;

	//return Mono::CreateSharpObjectFromCPlusPlus(mesh.get());

}

void EngineBinds::SetMaterial(MonoObject* meshRendererRef, MonoObject* materialRef)
{
	if (!meshRendererRef || !materialRef) return;

	MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
	Material* material = ConvertFromSharpComponent<Material>(materialRef);

	if (meshRenderer && material)
	{
		meshRenderer->SetMaterial(std::shared_ptr<Material>(material));
	}
}

MonoObject* EngineBinds::GetMaterial(MonoObject* meshRendererRef)
{
	if (!meshRendererRef) return nullptr;

	MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
	if (!meshRendererRef) return nullptr;

	std::shared_ptr<Material> material = meshRenderer->GetMaterial();
	if (!material) return nullptr;

	//return Mono::CreateSharpObjectFromCPlusPlus(material.get());
}

void EngineBinds::SetColor(MonoObject* meshRendererRef, glm::vec3* color)
{
    if (!meshRendererRef || !color) return;

    MeshRenderer* meshRenderer = ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
    if (meshRenderer) {
        meshRenderer->SetColor(*color);
    }
}
	

void EngineBinds::BindEngine() {

    mono_add_internal_call("MonoBehaviour::GetGameObject", (const void*)GetGameObject);
    // GameObject
    mono_add_internal_call("HawkEngine.Engineson::CreateGameObject", (const void*)CreateGameObjectSharp);
    mono_add_internal_call("HawkEngine.GameObject::GetName", (const void*)GameObjectGetName);
    mono_add_internal_call("HawkEngine.GameObject::SetName", (const void*) SetName );
    mono_add_internal_call("HawkEngine.GameObject::AddChild", (const void*)GameObjectAddChild);
    mono_add_internal_call("HawkEngine.Engineson::Destroy", (const void*)Destroy);
    mono_add_internal_call("HawkEngine.GameObject::TryGetComponent", (const void*)GetSharpComponent);
    mono_add_internal_call("HawkEngine.GameObject::TryAddComponent", (const void*)AddSharpComponent);

    // Input
    mono_add_internal_call("HawkEngine.Input::GetKey", (const void*)GetKey);
    mono_add_internal_call("HawkEngine.Input::GetKeyDown", (const void*)GetKeyDown);
    mono_add_internal_call("HawkEngine.Input::GetKeyUp", (const void*)GetKeyUp);
    mono_add_internal_call("HawkEngine.Input::GetMouseButton", (const void*)GetMouseButton);
    mono_add_internal_call("HawkEngine.Input::GetMouseButtonDown", (const void*)GetMouseButtonDown);
    mono_add_internal_call("HawkEngine.Input::GetMouseButtonUp", (const void*)GetMouseButtonUp);
    mono_add_internal_call("HawkEngine.Input::GetAxis", (const void*)GetAxis);
    mono_add_internal_call("HawkEngine.Input::GetMousePosition", (const void*)GetMousePosition);

	// Controller
	mono_add_internal_call("HawkEngine.Input::GetControllerButton", (const void*)GetControllerButton);
	mono_add_internal_call("HawkEngine.Input::GetControllerButtonDown", (const void*)GetControllerButtonDown);
	mono_add_internal_call("HawkEngine.Input::GetControllerButtonUp", (const void*)GetControllerButtonUp);
	mono_add_internal_call("HawkEngine.Input::GetJoystickAxis", (const void*)GetJoystickAxis);

    // Transform
    mono_add_internal_call("HawkEngine.Transform::SetPosition", (const void*)&EngineBinds::SetPosition);
    mono_add_internal_call("HawkEngine.Transform::GetPosition", (const void*)&EngineBinds::GetPosition);
    mono_add_internal_call("HawkEngine.Transform::SetLocalPosition", (const void*)&EngineBinds::SetLocalPosition);
    mono_add_internal_call("HawkEngine.Transform::GetLocalPosition", (const void*)&EngineBinds::GetLocalPosition);
    mono_add_internal_call("HawkEngine.Transform::SetRotation", (const void*)&EngineBinds::SetRotation);
    mono_add_internal_call("HawkEngine.Transform::GetEulerAngles", (const void*)&EngineBinds::GetEulerAngles);
    mono_add_internal_call("HawkEngine.Transform::SetRotationQuat", (const void*)&EngineBinds::SetRotationQuat);
    mono_add_internal_call("HawkEngine.Transform::Rotate", (const void*)&EngineBinds::Rotate);
    mono_add_internal_call("HawkEngine.Transform::RotateLocal", (const void*)&EngineBinds::RotateLocal);
    mono_add_internal_call("HawkEngine.Transform::LookAt", (const void*)&EngineBinds::LookAt);
    mono_add_internal_call("HawkEngine.Transform::SetScale", (const void*)&EngineBinds::SetScale);
    mono_add_internal_call("HawkEngine.Transform::Scale", (const void*)&EngineBinds::Scale);
    mono_add_internal_call("HawkEngine.Transform::TranslateLocal", (const void*)&EngineBinds::TranslateLocal);
    mono_add_internal_call("HawkEngine.Transform::AlignToGlobalUp", (const void*)&EngineBinds::AlignToGlobalUp);
    mono_add_internal_call("HawkEngine.Transform::SetForward", (const void*)&EngineBinds::SetForward);
    mono_add_internal_call("HawkEngine.Transform::GetForward", (const void*)&EngineBinds::GetForward);

    // Camera
    mono_add_internal_call("HawkEngine.Camera::SetCameraFieldOfView", (const void*)&EngineBinds::SetCameraFieldOfView);
    mono_add_internal_call("HawkEngine.Camera::SetCameraNearClipPlane", (const void*)&EngineBinds::SetCameraNearClipPlane);
    mono_add_internal_call("HawkEngine.Camera::SetCameraFarClipPlane", (const void*)&EngineBinds::SetCameraFarClipPlane);
    mono_add_internal_call("HawkEngine.Camera::SetCameraAspectRatio", (const void*)&EngineBinds::SetCameraAspectRatio);
    mono_add_internal_call("HawkEngine.Camera::SetCameraOrthographicSize", (const void*)&EngineBinds::SetCameraOrthographicSize);
    mono_add_internal_call("HawkEngine.Camera::SetCameraProjectionType", (const void*)&EngineBinds::SetCameraProjectionType);

    // MeshRenderer
    mono_add_internal_call("HawkEngine.MeshRenderer::SetMesh", (const void*)&EngineBinds::SetMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetCubeMesh", (const void*)&EngineBinds::SetCubeMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::GetMesh", (const void*)&EngineBinds::GetMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetMaterial", (const void*)&EngineBinds::SetMaterial);
    mono_add_internal_call("HawkEngine.MeshRenderer::GetMaterial", (const void*)&EngineBinds::GetMaterial);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetColor", (const void*)&EngineBinds::SetColor);
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

    //std::cerr << "Looking for class : " << typeName << std::endl;

    if (!klass) {
        throw std::runtime_error("Class not found in the C#: " + typeName);
    }

    MonoClassField* field = mono_class_get_field_from_name(klass, "CplusplusInstance");

    if (!field) {
        throw std::runtime_error("field CplusplusInstance not found: " + typeName);
    }

    mono_field_get_value(sharpComp, field, &Cptr);
    return reinterpret_cast<T*>(Cptr);
}