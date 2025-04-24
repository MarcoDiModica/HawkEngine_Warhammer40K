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
#include "../MyGameEngine/Scene.h"
#include "../MyPhysicsEngine/BoxColliderComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "ScriptComponent.h"
#include <mono/metadata/debug-helpers.h>
#include <MyPhysicsEngine/RigidBodyComponent.h>
#include "../MyGameEditor/App.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UIButtonComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyGameEngine/Tweening.h"

#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "../MyParticlesEngine/ParticleFX.h"
#include <MyPhysicsEngine/MeshColliderComponent.h>
#include <MyPhysicsEngine/CapsuleColliderComponent.h>
#include <MyGameEngine/PrefabManager.h>

// GameObject

MonoObject* EngineBinds::GetGameObject(MonoObject* ref) {

    return GetScriptOwner(ref)->GetSharp();

}

MonoString* EngineBinds::GameObjectGetName(MonoObject* sharpRef) {
    std::string name = ConvertFromSharp(sharpRef)->GetName();
    return mono_string_new(MonoManager::GetInstance().GetDomain(), name.c_str());
}

MonoString* EngineBinds::GameObjectGetTag(MonoObject* sharpRef) {
    std::string name = ConvertFromSharp(sharpRef)->GetTag();
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

    GameObject* go = ConvertFromSharp(object_to_destroy);
    Application->root->RemoveGameObject(go);
}

MonoObject* EngineBinds::GetSharpComponent(MonoObject* ref, MonoString* component_name)
{
    char* C_name = mono_string_to_utf8(component_name);
    auto GO = ConvertFromSharp(ref);

    std::string componentName = std::string(C_name);

    for (const auto& scriptComponent : GO->scriptComponents)
    {
        MonoObject* monoScript = scriptComponent->GetSharpObject();
        if (!monoScript) continue;

        MonoClass* scriptClass = mono_object_get_class(monoScript);

        MonoClass* targetClass = mono_class_from_name_case(
            MonoManager::GetInstance().GetImage(),
            "", // Namespace vacío (se puede mejorar)
            C_name
        );

        if (!targetClass) continue;

        if (mono_class_is_subclass_of(scriptClass, targetClass, true)) {
            return monoScript;
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
    else if (componentName == "HawkEngine.Collider") {
        if (auto boxCollider = GO->GetComponent<BoxColliderComponent>()) {
            return boxCollider->GetSharp();
        }
        else if (auto capsuleCollider = GO->GetComponent<CapsuleColliderComponent>()) {
            return capsuleCollider->GetSharp();
        }
        else if (auto meshCollider = GO->GetComponent<MeshColliderComponent>()) {
            return meshCollider->GetSharp();
        }
	}
    else if (componentName == "HawkEngine.BoxCollider") {
		return GO->GetComponent<BoxColliderComponent>()->GetSharp();
	}
    else if (componentName == "HawkEngine.CapsuleCollider") {
		return GO->GetComponent<CapsuleColliderComponent>()->GetSharp();
	}
    else if (componentName == "HawkEngine.MeshCollider") {
		return GO->GetComponent<MeshColliderComponent>()->GetSharp();
	}
    else if (componentName == "HawkEngine.Rigidbody") {
		return GO->GetComponent<RigidbodyComponent>()->GetSharp();
	}
    else if (componentName == "HawkEngine.Audio") {
        return GO->GetComponent<SoundComponent>()->GetSharp();
    }
    else if (componentName == "HawkEngine.UIImage") {
        return GO->GetComponent<UIImageComponent>()->GetSharp();
    }
	else if (componentName == "HawkEngine.SkeletalAnimation") {
		return GO->GetComponent<SkeletalAnimationComponent>()->GetSharp();
	}
	else if (componentName == "HawkEngine.UIButton") {
		return GO->GetComponent<UIButtonComponent>()->GetSharp();
	}
	else if (componentName == "HawkEngine.UICanvas") {
		return GO->GetComponent<UICanvasComponent>()->GetSharp();
	}
    else if (componentName == "HawkEngine.UITransform") {
        return GO->GetComponent<UITransformComponent>()->GetSharp();
    }
	else if (componentName == "HawkEngine.ScriptComponent") {
		return GO->GetComponent<ScriptComponent>()->GetSharp();
	}
	else if (componentName == "HawkEngine.ParticleFX") {
		return GO->GetComponent<ParticleFX>()->GetSharp();
	}


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
    case 3: _component = static_cast<Component*>(go->AddComponent<BoxColliderComponent>(Application->physicsModule));
		break;
    case 4: _component = static_cast<Component*>(go->AddComponent<RigidbodyComponent>(Application->physicsModule));
        break;
    case 5: _component = static_cast<Component*>(go->AddComponent<SoundComponent>());
        break;
    case 6: _component = static_cast<Component*>(go->AddComponent<UIImageComponent>());
		break;
	case 7: _component = static_cast<Component*>(go->AddComponent<UIButtonComponent>());
		break;
	case 8: _component = static_cast<Component*>(go->AddComponent<UICanvasComponent>());
		break; 
    case 9: _component = static_cast<Component*>(go->AddComponent<SkeletalAnimationComponent>());
        break;
	case 10: _component = static_cast<Component*>(go->AddComponent<UITransformComponent>());
		break;
    case 11: _component = static_cast<Component*>(go->AddComponent<MeshColliderComponent>(Application->physicsModule));
        break; 
    case 12: _component = static_cast<Component*>(go->AddComponent<CapsuleColliderComponent>(Application->physicsModule));
        break;
	case 13: _component = static_cast<Component*>(go->AddComponent<ParticleFX>());
		break;
    default:
        //for (const auto& [className, id] : MonoManager::GetInstance().scriptIDs) {
        //    if (component == id) {
        //        MonoClass* klass = MonoManager::GetInstance().GetClass("", className.c_str());
        //        if (!klass) return nullptr;


        //        //Commented for the moment but in case is abstract or interface  should give an error 
        //       /* if (mono_class_is_interface(klass) || mono_class_is_abstract(klass)) {
        //            Logger::LogError("[AddSharpComponent] Cannot instantiate abstract/interface script: " + className);
        //            return nullptr;
        //        }*/

        //        auto script = go->AddComponent<ScriptComponent>();
        //        script->LoadScript(className);
        //        return script->GetSharpObject();
        //    }
        //}
        break;
    
	}

    return _component->GetSharp();

}

void EngineBinds::AddScript(MonoObject* ref, MonoString* scriptName) {
	char* C_name = mono_string_to_utf8(scriptName);
	auto go = ConvertFromSharp(ref);
	go->AddComponent<ScriptComponent>()->LoadScript(C_name);
}

void EngineBinds::SetActive(MonoObject* ref, bool active) {
	ConvertFromSharp(ref)->SetActive(active);
}

bool EngineBinds::GameObjectIsActive(MonoObject* ref) {
	return ConvertFromSharp(ref)->IsActive();
}


void EngineBinds::SetName(MonoObject* ref, MonoString* sharpName) {

    char* C_name = mono_string_to_utf8(sharpName);
    ConvertFromSharp(ref)->SetName(std::string(C_name));
}
void EngineBinds::SetTag(MonoObject* ref, MonoString* sharpName) {

    char* C_name = mono_string_to_utf8(sharpName);
    ConvertFromSharp(ref)->SetTag(std::string(C_name));
}

void EngineBinds::GameObjectSetActive(MonoObject* ref, bool active) {
    ConvertFromSharp(ref)->SetActive(active);
}


MonoString* EngineBinds::GetTag(MonoObject* ref) {
	return mono_string_new(MonoManager::GetInstance().GetDomain(), ConvertFromSharp(ref)->GetTag().c_str());
}



MonoObject* EngineBinds::GetGameObjectByName(MonoString* name)
{
    char* C_name = mono_string_to_utf8(name);
	GameObject* go = SceneManagement->FindGOByName(std::string(C_name)).get();
    return go ? go->GetSharp() : nullptr;
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

float EngineBinds::GetControllerAxis(int gamepadIndex, int axis) {
	const float DEADZONE = 0.2f;

	float rawValue = 0.0f;

	switch (axis) {
	case 0:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTX] / 32767.0f;
		break;
	case 1:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_LEFTY] / 32767.0f;
		break;
	case 2:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTX] / 32767.0f;
		break;
	case 3:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_RIGHTY] / 32767.0f;
		break;
	case 4:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERLEFT] / 32767.0f;
		break;
	case 5:
		rawValue = InputManagement->gamepads[gamepadIndex].axes[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] / 32767.0f;
		break;
	}

	float absValue = std::abs(rawValue);
	if (absValue < DEADZONE) {
		return 0.0f;
	}

	float normalizedValue = (absValue - DEADZONE) / (1.0f - DEADZONE);
	return (rawValue > 0) ? normalizedValue : -normalizedValue;
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
    if (transform)
        transform->SetRotation(glm::radians(glm::vec3(x, y, z)));
}

Vector3 EngineBinds::GetEulerAngles(MonoObject* transformRef) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    
	glm::vec3 euler = transform->GetEulerAngles();
	return Vector3{ (float)euler.x, (float)euler.y, (float)euler.z };
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

void EngineBinds::TranslateLocal(MonoObject* transformRef, float x, float y, float z) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->TranslateLocal(glm::vec3(x, y, z));
}

void EngineBinds::AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->AlignToGlobalUp(*worldUp);
}

void EngineBinds::SetForward(MonoObject* transformRef, glm::vec3* newFwd) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetForward(*newFwd);
}

Vector3 EngineBinds::GetForward(MonoObject* transformRef) {

    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    auto forward = transform->GetForward();
    return Vector3{ (float) forward.x,(float) forward.y , (float) forward.z };
}

void EngineBinds::SetRight(MonoObject* transformRef, glm::vec3* newRight) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetRight(*newRight);
}

Vector3 EngineBinds::GetRight(MonoObject* transformRef) {

    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    auto right = transform->GetRight();
    return Vector3{ (float) right.x,(float) right.y , (float) right.z };
}

void EngineBinds::SetUp(MonoObject* transformRef, glm::vec3* newUp) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) transform->SetUp(*newUp);
}

Vector3 EngineBinds::GetUp(MonoObject* transformRef) {

    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    auto up = transform->GetUp();
    return Vector3{ (float) up.x,(float) up.y , (float) up.z };
}

Vector3 EngineBinds::GetLocalScale(MonoObject* transformRef) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    auto scale = transform->GetLocalScale();
    return Vector3{ (float)scale.x, (float)scale.y, (float)scale.z };
}
void EngineBinds::SetLocalScale(MonoObject* transformRef, Vector3 scale) {
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    transform->SetLocalScale(glm::dvec3(scale.X, scale.Y, scale.Z));
}

// Camera Class functions

void EngineBinds::SetCameraFieldOfView(MonoObject* cameraRef, double fov) {

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

void EngineBinds::SetFollowTarget(MonoObject* cameraRef, MonoObject* target, glm::vec3* offset, float distance, bool followX, bool followY, bool followZ, float smoothness)
{
    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
	GameObject* targetGO = ConvertFromSharp(target);
	camera->FollowTarget(targetGO, distance, *offset, smoothness, followX, followY, followZ);
}

void EngineBinds::SetDistance(MonoObject* cameraRef, float distance)
{
    CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
	camera->SetDistance(distance);
}

void EngineBinds::SetOffset(MonoObject* cameraRef, glm::vec3* offset)
{
	CameraComponent* camera = ConvertFromSharpComponent<CameraComponent>(cameraRef);
	camera->SetOffset(*offset);
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
	
// Physics Collider
void EngineBinds::SetTrigger(MonoObject* colliderRef, bool trigger) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SetTrigger(trigger);
    }
}

bool EngineBinds::IsTrigger(MonoObject* colliderRef)
{
    if (!colliderRef) return false;

    BaseColliderComponent* collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
	if (collider) {
		return collider->IsTrigger();
	}

    return false;
}

glm::vec3 EngineBinds::GetColliderPosition(MonoObject* colliderRef) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    return collider ? collider->GetColliderPos() : glm::vec3(0.0f);
}

void EngineBinds::SetColliderPosition(MonoObject* colliderRef, glm::vec3* position) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SetColliderPos(*position);
    }
}


MonoObject* GetMonoObjectFromGameObjectHelper(GameObject* gameObject) {
	if (!gameObject) return nullptr;

	MonoClass* gameObjectClass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
	if (!gameObjectClass) {
		return nullptr;
	}

	MonoObject* monoGameObject = mono_object_new(mono_domain_get(), gameObjectClass);
	if (!monoGameObject) {
		return nullptr;
	}

	MonoClassField* nativePtrField = mono_class_get_field_from_name(gameObjectClass, "CplusplusInstance");
	if (!nativePtrField) {
		return nullptr;
	}

	uintptr_t nativePtr = reinterpret_cast<uintptr_t>(gameObject);
	mono_field_set_value(monoGameObject, nativePtrField, &nativePtr);

	return monoGameObject;
}

// mirarse esta funcion rarilla
MonoArray* EngineBinds::OverlapSphere(glm::vec3* position, float radius, MonoString* tag) {
    if (!Application || !Application->physicsModule) {
        return nullptr;
    }

    std::string tagStr = mono_string_to_utf8(tag);
    auto overlappingObjects = Application->physicsModule->OverlapSphere(*position, radius, tagStr);

    MonoDomain* domain = mono_domain_get();
    MonoClass* gameObjectClass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    if (!gameObjectClass) return nullptr;

    MonoArray* monoArray = mono_array_new(domain, gameObjectClass, overlappingObjects.size());

    for (size_t i = 0; i < overlappingObjects.size(); i++) {
        MonoObject* monoGameObject = GetMonoObjectFromGameObjectHelper(overlappingObjects[i]);
        if (monoGameObject) {
            mono_array_set(monoArray, MonoObject*, i, monoGameObject);
        }
    }

    return monoArray;
}


glm::quat EngineBinds::GetColliderRotation(MonoObject* colliderRef) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    return collider ? collider->GetColliderRotation() : glm::quat();
}

void EngineBinds::SetColliderRotation(MonoObject* colliderRef, glm::quat* rotation) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SetColliderRotation(*rotation);
    }
}

glm::vec3 EngineBinds::GetColliderSize(MonoObject* colliderRef) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    return collider ? collider->GetSize() : glm::vec3(0.0f);
}

void EngineBinds::SetColliderSize(MonoObject* colliderRef, glm::vec3* sizeFactor) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SetSize(*sizeFactor);
    }
}

void EngineBinds::SetColliderActive(MonoObject* colliderRef, bool active) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SetActive(active);
    }
}

void EngineBinds::SnapColliderToPosition(MonoObject* colliderRef) {
    auto collider = ConvertFromSharpComponent<BaseColliderComponent>(colliderRef);
    if (collider) {
        collider->SnapToPosition();
    }
}


//Rigidbody
void EngineBinds::SetVelocity(MonoObject* rigidbodyRef, glm::vec3* velocity) {
    
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        auto btRigidBody = rigidbody->GetRigidBody();
        btRigidBody->setLinearVelocity(btVector3(velocity->x, velocity->y, velocity->z));
    }
}

glm::vec3 EngineBinds::GetVelocity(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        auto btRigidBody = rigidbody->GetRigidBody();
        auto velocity = btRigidBody->getLinearVelocity();
        return glm::vec3(velocity.getX(), velocity.getY(), velocity.getZ());
    }
    return glm::vec3(0.0f);
}

void EngineBinds::AddForce(MonoObject* rigidbodyRef, glm::vec3* force) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->AddForce(*force);
    }
}

void EngineBinds::SetMass(MonoObject* rigidbodyRef, float mass) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->SetMass(mass);
    }
}

float EngineBinds::GetMass(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    return rigidbody ? rigidbody->GetMass() : 0.0f;
}

void EngineBinds::SetFriction(MonoObject* rigidbodyRef, float friction) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->SetFriction(friction);
    }
}

float EngineBinds::GetFriction(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    return rigidbody ? rigidbody->GetFriction() : 0.0f;
}

void EngineBinds::SetGravity(MonoObject* rigidbodyRef, glm::vec3* gravity) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->SetGravity(*gravity);
    }
}

glm::vec3 EngineBinds::GetGravity(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    return rigidbody ? rigidbody->GetGravity() : glm::vec3(0.0f);
}

void EngineBinds::SetDamping(MonoObject* rigidbodyRef, float linearDamping, float angularDamping) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->SetDamping(linearDamping, angularDamping);
    }
}

glm::vec2 EngineBinds::GetDamping(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    return rigidbody ? rigidbody->GetDamping() : glm::vec2(0.0f);
}

void EngineBinds::SetKinematic(MonoObject* rigidbodyRef, bool isKinematic) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->SetKinematic(isKinematic);
    }
}

bool EngineBinds::IsKinematic(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    return rigidbody ? rigidbody->IsKinematic() : false;
}

void EngineBinds::EnableContinuousCollision(MonoObject* rigidbodyRef) {
    auto rigidbody = ConvertFromSharpComponent<RigidbodyComponent>(rigidbodyRef);
    if (rigidbody) {
        rigidbody->EnableContinuousCollision();
    }
}

// Raycast
MonoObject* EngineBinds::Raycast(glm::vec3* origin, glm::vec3* direction, float maxDistance, glm::vec3& hitPoint, glm::vec3& normal, float& distance)
{
    btVector3 from;
	from.setValue(origin->x, origin->y, origin->z);

	btVector3 to;
	to.setValue(direction->x, direction->y, direction->z);

    btVector3 hitPos;
    hitPos.setValue(hitPoint.x, hitPoint.y, hitPoint.z);

    btVector3 hitNormal;
    hitNormal.setValue(normal.x, normal.y, normal.z);

	GameObject* hitObject;
	hitObject = Application->physicsModule->Raycast(from, to, maxDistance, hitPos, hitNormal, distance);

	
	hitPoint = glm::vec3(hitPos.getX(), hitPos.getY(), hitPos.getZ());
	normal = glm::vec3(hitNormal.getX(), hitNormal.getY(), hitNormal.getZ());

	if (hitObject) {
		return GetMonoObjectFromGameObjectHelper(hitObject);
	}

    return nullptr;
}


void EngineBinds::Play(MonoObject* audioRef, bool loop /*= false*/)
{
    auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->Play(loop);
	}

}

void EngineBinds::Stop(MonoObject* audioRef)
{
    auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->Stop();
	}
}

void EngineBinds::Pause(MonoObject* audioRef)
{
	auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->Pause();
	}
}

void EngineBinds::Resume(MonoObject* audioRef)
{
	auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->Resume();
	}
}

void EngineBinds::SetVolume(MonoObject* audioRef, float volume)
{
	auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->SetVolume(volume);
	}
}

float EngineBinds::GetVolume(MonoObject* audioRef)
{
    auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
    return sound ? sound->GetVolume() : 0.0f;
}

void EngineBinds::LoadAudioClip(MonoObject* audioRef, MonoString* path)
{
	char* C_path = mono_string_to_utf8(path);
	auto sound = ConvertFromSharpComponent<SoundComponent>(audioRef);
	if (sound) {
		sound->LoadAudio(C_path, true);
	}
}

void EngineBinds::SetTexture(MonoObject* uiImageRef, MonoString* path)
{
	char* C_path = mono_string_to_utf8(path);
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetTexture(C_path);
	}
}

void EngineBinds::SetImageEnabled(MonoObject* uiImageRef, bool enabled) {
    auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
    if (uiImage) {
        uiImage->SetEnabled(enabled);
    }
}

void EngineBinds::SetImageHasAnimation(MonoObject* uiImageRef, bool hasAnimation) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetUseAnimation(hasAnimation);
	}
}

void EngineBinds::SetImageAnimationSpeed(MonoObject* uiImageRef, float speed) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetAnimSpeed(speed);
	}
}

void EngineBinds::SetImageAnimationIndexLimit(MonoObject* uiImageRef, int indexLimit) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetAnimationIndexLimit(indexLimit);
	}
}

void EngineBinds::SetImageAnimation(MonoObject* uiImageRef, int index) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
        if (index == 0) 
        {
            uiImage->SetAnimationIndex(0);
        }
        else 
        {
            uiImage->SetAnimationIndex(uiImage->GetAnimationIndexLimit());
        }

		uiImage->SetAnimationNum(index);
	}
}

void EngineBinds::SetImageSpriteSize(MonoObject* uiImageRef, float width, float height) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetSpriteSize(vec2(width, height));
	}
}

void EngineBinds::SetImageAnimIndex(MonoObject* uiImageRef, int index) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->SetAnimationIndex(index);
	}
}

void EngineBinds::PlayStopAnimation(MonoObject* uiImageRef, bool play) {
	auto uiImage = ConvertFromSharpComponent<UIImageComponent>(uiImageRef);
	if (uiImage) {
		uiImage->PlayStopAnimation(play);
	}
}

int EngineBinds::GetState(MonoObject* uiButtonRef)
{
	auto uiButton = ConvertFromSharpComponent<UIButtonComponent>(uiButtonRef);
	if (uiButton) {
		return (int)uiButton->GetState();
	}
}

void EngineBinds::SetUIScale(MonoObject* uiTransformRef, glm::vec3* scale)
{
	auto uiTransform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
	if (uiTransform) {
		uiTransform->SetSacale(*scale);
	}
}

void EngineBinds::SetAnimationSpeed(MonoObject* animationRef, float speed)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->SetAnimationSpeed(speed);
	}
}

void EngineBinds::SetLoop(MonoObject* animationRef, bool loop)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->SetLoop(loop);
	}
}

float EngineBinds::GetAnimationSpeed(MonoObject* animationRef)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	return animation ? animation->GetAnimationSpeed() : 0.0f;
}

void EngineBinds::SetAnimation(MonoObject* animationRef, int index)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->PlayIndexAnimation(index);
	}
}

int EngineBinds::GetAnimationIndex(MonoObject* animationRef)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
    return animation ? animation->GetAnimationIndex() : 0;
}

float EngineBinds::GetAnimationTime(MonoObject* animationRef)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	return animation ? animation->GetAnimationTime() : 0.0f;
}

float EngineBinds::GetAnimationLength(MonoObject* animationRef)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	return animation ? animation->GetAnimationDuration() : 0.0f;
}

void EngineBinds::SetAnimationPlayTime(MonoObject* animationRef, float time)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->GetAnimator()->SetCurrentMTime(time);
	}
}

void EngineBinds::SetAnimationPlayState(MonoObject* animationRef, bool play)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->SetAnimationPlayState(play);
	}
}

bool EngineBinds::GetAnimationPlayState(MonoObject* animationRef)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	return animation ? animation->GetAnimationPlayState() : false;
}

void EngineBinds::TransitionAnimations(MonoObject* animationRef, int oldAnim, int newAnim, float timeToAnim)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->TransitionAnimations(oldAnim, newAnim, timeToAnim);
	}
}

void EngineBinds::PlayAnimOnce(MonoObject* animationRef, int index, float timeToTransition)
{
	auto animation = ConvertFromSharpComponent<SkeletalAnimationComponent>(animationRef);
	if (animation) {
		animation->PlayAnimOnce(index, timeToTransition);
	}
}

//Tween
// tienes que referenciar al componente que vas a usar o al gameobject que vas a usar, en el caso de que sea
// un gameobject se usara el transform del gameobject
void EngineBinds::DOMove(MonoObject* transformRef, glm::vec3* targetPosition, float duration, Modes mode) {
	auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
	if (transform) {
		GameObject* object = transform->GetOwner();
		Tweening::Move(object, *targetPosition, duration, mode);
	}
}

void EngineBinds::DOMoveX(MonoObject* transformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::MoveX(object, targetPosition, duration, mode);
    }
}

void EngineBinds::DOMoveY(MonoObject* transformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
		GameObject* object = transform->GetOwner();
		Tweening::MoveY(object, targetPosition, duration, mode);
	}
	
}

void EngineBinds::DOMoveZ(MonoObject* transformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
	    Tweening::MoveZ(object, targetPosition, duration, mode);
	}
}   

void EngineBinds::DORotate(MonoObject* transformRef, glm::vec3* targetRotation, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::Rotate(object, *targetRotation, duration, mode);
    }
}

void EngineBinds::DORotateX(MonoObject* transformRef, float targetRotation, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::RotateX(object, targetRotation, duration, mode);
    }
}

void EngineBinds::DORotateY(MonoObject* transformRef, float targetRotation, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::RotateY(object, targetRotation, duration, mode);
    }
}

void EngineBinds::DORotateZ(MonoObject* transformRef, float targetRotation, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::RotateZ(object, targetRotation, duration, mode);
    }
}

void EngineBinds::DOScale(MonoObject* transformRef, glm::vec3* targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::Scale(object, *targetScale, duration, mode);
    }
}

void EngineBinds::DOScaleX(MonoObject* transformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::ScaleX(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOScaleY(MonoObject* transformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::ScaleY(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOScaleZ(MonoObject* transformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<Transform_Component>(transformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::ScaleZ(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOMoveUI(MonoObject* uiTransformRef, glm::vec3* targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIMove(object, *targetPosition, duration, mode);
    }
}

void EngineBinds::DOMoveXUI(MonoObject* uiTransformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIMoveX(object, targetPosition, duration, mode);
    }
}

void EngineBinds::DOMoveYUI(MonoObject* uiTransformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIMoveY(object, targetPosition, duration, mode);
    }
}

void EngineBinds::DOMoveZUI(MonoObject* uiTransformRef, float targetPosition, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIMoveZ(object, targetPosition, duration, mode);
    }
}

void EngineBinds::DOScaleUI(MonoObject* uiTransformRef, glm::vec3* targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
	    Tweening::UIScale(object, *targetScale, duration, mode);
	}
}

void EngineBinds::DOScaleXUI(MonoObject* uiTransformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIScaleX(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOScaleYUI(MonoObject* uiTransformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIScaleY(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOScaleZUI(MonoObject* uiTransformRef, float targetScale, float duration, Modes mode)
{
    auto transform = ConvertFromSharpComponent<UITransformComponent>(uiTransformRef);
    if (transform) {
        GameObject* object = transform->GetOwner();
        Tweening::UIScaleZ(object, targetScale, duration, mode);
    }
}

void EngineBinds::DOColor(glm::vec4 *color, const glm::vec4 startColor, const glm::vec4 targetColor, float duration, Modes mode)
{
	Tweening::TweenColor(color, startColor, targetColor, duration, mode);
}

void EngineBinds::DOValue(float* value, float start, float target, float duration, Modes mode)
{
    Tweening::TweenValue(value, start, target, duration, mode);
}

void EngineBinds::CleanAllTweens()
{
    Tweening::CleanAllTweens();
}

void EngineBinds::DOVec3(glm::vec3* value, glm::vec3 start, glm::vec3 target, float duration, Modes mode)
{
	Tweening::TweenVec3(value, start, target, duration, mode);
}
    
bool EngineBinds::LoadScene(MonoString* sceneName)
{
    char* C_sceneName = mono_string_to_utf8(sceneName);
    if (Application->scene_serializer->DeSerialize(std::string(C_sceneName)))
    {
        Application->physicsModule->linkPhysicsToScene = false;
		Application->hasChangedScene = true;
		return true;
    }
	return false;
}

void EngineBinds::SetScenePlay()
{
	SceneManagement->currentScene->sceneState = Scene::SceneState::PLAY;
	SceneManagement->Awake();
	SceneManagement->currentScene->Start();
}

void EngineBinds::ApplyPreset(MonoObject* particleRef, int particleType)
{
	auto particle = ConvertFromSharpComponent<ParticleFX>(particleRef);

	if (particle) {
		particle->ApplyPreset(particleType);
	}
}

void EngineBinds::SetOneShot(MonoObject* particleRef, bool oneShot)
{
	auto particle = ConvertFromSharpComponent<ParticleFX>(particleRef);
	if (particle) {
		particle->SetOneShot(oneShot);
	}
}

void EngineBinds::PlayParticle(MonoObject* particleRef)
{
	auto particle = ConvertFromSharpComponent<ParticleFX>(particleRef);
	if (particle) {
		particle->Play();
	}
}

void EngineBinds::StopParticle(MonoObject* particleRef)
{
	auto particle = ConvertFromSharpComponent<ParticleFX>(particleRef);
	if (particle) {
		particle->Pause();
	}
}

void EngineBinds::EmitBurst(MonoObject* particleRef, int burstCount)
{
	auto particle = ConvertFromSharpComponent<ParticleFX>(particleRef);
	if (particle) {
		particle->EmitBurst(burstCount);
	}
}
MonoObject* EngineBinds::InstantiatePrefab(MonoObject* prefabObj, MonoObject* parentTransformObj, bool worldPositionStays)  
{  
   if (!prefabObj) return nullptr;  
 
   MonoClass* prefabClass = mono_object_get_class(prefabObj);  
   MonoClassField* pathField = mono_class_get_field_from_name(prefabClass, "path");  

   MonoString* pathString = nullptr;  
   mono_field_get_value(prefabObj, pathField, &pathString);  

   if (!pathString) return nullptr;  

   char* cStr = mono_string_to_utf8(pathString);  
   std::string prefabPath(cStr);  
   mono_free(cStr);  

   std::shared_ptr<GameObject> newGO = PrefabManager::LoadPrefab(prefabPath);

   if (!newGO) return nullptr;  

   // Handle parenting  
   if (parentTransformObj) {
       MonoClass* transformClass = mono_object_get_class(parentTransformObj);
       MonoClassField* cppInstanceField = mono_class_get_field_from_name(transformClass, "CplusplusInstance");

       if (cppInstanceField) {
           uintptr_t cppInstance = 0;
           mono_field_get_value(parentTransformObj, cppInstanceField, &cppInstance);
           Transform_Component* parentTransform = reinterpret_cast<Transform_Component*>(cppInstance);

           if (parentTransform) {
               GameObject* parentGO = parentTransform->GetOwner();
               if (parentGO) {
                   if (worldPositionStays)
                       Application->root->ParentGameObjectPreserve(*newGO, *parentGO);
                   else
                       Application->root->ParentGameObject(*newGO, *parentGO);
               }
           }
       }
   }

   return MonoManager::GetInstance().CreateGameObjectReference(newGO.get());  
}

void EngineBinds::BindEngine() {

    // GameObject
	mono_add_internal_call("MonoBehaviour::GetGameObject", (const void*)GetGameObject);
    mono_add_internal_call("MonoBehaviour::Instantiate", (const void*)InstantiatePrefab);
    mono_add_internal_call("HawkEngine.Engineson::CreateGameObject", (const void*)CreateGameObjectSharp);
    mono_add_internal_call("HawkEngine.GameObject::GetName", (const void*)GameObjectGetName);
    mono_add_internal_call("HawkEngine.GameObject::GetTag", (const void*)GameObjectGetTag);
    mono_add_internal_call("HawkEngine.GameObject::SetName", (const void*) SetName );
	mono_add_internal_call("HawkEngine.GameObject::GetTag", (const void*)GetTag);
	mono_add_internal_call("HawkEngine.GameObject::SetTag", (const void*)SetTag);
    mono_add_internal_call("HawkEngine.GameObject::AddChild", (const void*)GameObjectAddChild);
    mono_add_internal_call("HawkEngine.Engineson::Destroy", (const void*)Destroy);
    mono_add_internal_call("HawkEngine.GameObject::TryGetComponent", (const void*)GetSharpComponent);
    mono_add_internal_call("HawkEngine.GameObject::TryAddComponent", (const void*)AddSharpComponent);
    mono_add_internal_call("HawkEngine.GameObject::Find", (const void*)GetGameObjectByName);
    mono_add_internal_call("HawkEngine.GameObject::AddScript", (const void*)AddScript);
    mono_add_internal_call("HawkEngine.GameObject::SetActive", (const void*)GameObjectSetActive);
    mono_add_internal_call("HawkEngine.GameObject::IsActive", (const void*)GameObjectIsActive);



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
	mono_add_internal_call("HawkEngine.Input::GetControllerAxis", (const void*)GetControllerAxis);

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
    mono_add_internal_call("HawkEngine.Transform::TranslateLocal", (const void*)&EngineBinds::TranslateLocal);
    mono_add_internal_call("HawkEngine.Transform::AlignToGlobalUp", (const void*)&EngineBinds::AlignToGlobalUp);
    mono_add_internal_call("HawkEngine.Transform::SetForward", (const void*)&EngineBinds::SetForward);
    mono_add_internal_call("HawkEngine.Transform::GetForward", (const void*)&EngineBinds::GetForward);
    mono_add_internal_call("HawkEngine.Transform::GetUp", (const void*)&EngineBinds::SetUp);
    mono_add_internal_call("HawkEngine.Transform::GetUp", (const void*)&EngineBinds::GetUp);
    mono_add_internal_call("HawkEngine.Transform::GetRight", (const void*)&EngineBinds::SetRight);
    mono_add_internal_call("HawkEngine.Transform::GetRight", (const void*)&EngineBinds::GetRight);
    mono_add_internal_call("HawkEngine.Transform::SetLocalScale", (const void*)&EngineBinds::SetLocalScale);
    mono_add_internal_call("HawkEngine.Transform::GetLocalScale", (const void*)&EngineBinds::GetLocalScale);

    // Camera
    mono_add_internal_call("HawkEngine.Camera::SetCameraFieldOfView", (const void*)&EngineBinds::SetCameraFieldOfView);
    mono_add_internal_call("HawkEngine.Camera::SetCameraNearClipPlane", (const void*)&EngineBinds::SetCameraNearClipPlane);
    mono_add_internal_call("HawkEngine.Camera::SetCameraFarClipPlane", (const void*)&EngineBinds::SetCameraFarClipPlane);
    mono_add_internal_call("HawkEngine.Camera::SetCameraAspectRatio", (const void*)&EngineBinds::SetCameraAspectRatio);
    mono_add_internal_call("HawkEngine.Camera::SetCameraOrthographicSize", (const void*)&EngineBinds::SetCameraOrthographicSize);
    mono_add_internal_call("HawkEngine.Camera::SetCameraProjectionType", (const void*)&EngineBinds::SetCameraProjectionType);
    mono_add_internal_call("HawkEngine.Camera::SetFollowTarget", (const void*)&EngineBinds::SetFollowTarget);
    mono_add_internal_call("HawkEngine.Camera::SetDistance", (const void*)&EngineBinds::SetDistance);
    mono_add_internal_call("HawkEngine.Camera::SetOffset", (const void*)&EngineBinds::SetOffset);

    // MeshRenderer
    mono_add_internal_call("HawkEngine.MeshRenderer::SetMesh", (const void*)&EngineBinds::SetMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetCubeMesh", (const void*)&EngineBinds::SetCubeMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::GetMesh", (const void*)&EngineBinds::GetMesh);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetMaterial", (const void*)&EngineBinds::SetMaterial);
    mono_add_internal_call("HawkEngine.MeshRenderer::GetMaterial", (const void*)&EngineBinds::GetMaterial);
    mono_add_internal_call("HawkEngine.MeshRenderer::SetColor", (const void*)&EngineBinds::SetColor);


    //Physics
    mono_add_internal_call("HawkEngine.Physics::OverlapSphere", (const void*)&EngineBinds::OverlapSphere);

    // Physics Collider
    mono_add_internal_call("HawkEngine.Collider::SetTrigger", (const void*)&EngineBinds::SetTrigger);
    mono_add_internal_call("HawkEngine.Collider::IsTrigger", (const void*)&EngineBinds::IsTrigger);
    mono_add_internal_call("HawkEngine.Collider::GetPosition", (const void*)&EngineBinds::GetColliderPosition);
    mono_add_internal_call("HawkEngine.Collider::SetPosition", (const void*)&EngineBinds::SetColliderPosition);
    mono_add_internal_call("HawkEngine.Collider::GetRotation", (const void*)&EngineBinds::GetColliderRotation);
    mono_add_internal_call("HawkEngine.Collider::SetRotation", (const void*)&EngineBinds::SetColliderRotation);
    mono_add_internal_call("HawkEngine.Collider::GetSize", (const void*)&EngineBinds::GetColliderSize);
    mono_add_internal_call("HawkEngine.Collider::SetSize", (const void*)&EngineBinds::SetColliderSize);
    mono_add_internal_call("HawkEngine.Collider::SetActive", (const void*)&EngineBinds::SetColliderActive);
    mono_add_internal_call("HawkEngine.Collider::SnapToPosition", (const void*)&EngineBinds::SnapColliderToPosition);
    mono_add_internal_call("HawkEngine.Collider::OverlapSphere", (const void*)&EngineBinds::OverlapSphere);

    // Physics Rigidbody
    mono_add_internal_call("HawkEngine.Rigidbody::SetVelocity", (const void*)&EngineBinds::SetVelocity);
    mono_add_internal_call("HawkEngine.Rigidbody::GetVelocity", (const void*)&EngineBinds::GetVelocity);
    mono_add_internal_call("HawkEngine.Rigidbody::AddForce", (const void*)&EngineBinds::AddForce);
    mono_add_internal_call("HawkEngine.Rigidbody::SetMass", (const void*)&EngineBinds::SetMass);
    mono_add_internal_call("HawkEngine.Rigidbody::GetMass", (const void*)&EngineBinds::GetMass);
    mono_add_internal_call("HawkEngine.Rigidbody::SetFriction", (const void*)&EngineBinds::SetFriction);
    mono_add_internal_call("HawkEngine.Rigidbody::GetFriction", (const void*)&EngineBinds::GetFriction);
    mono_add_internal_call("HawkEngine.Rigidbody::SetGravity", (const void*)&EngineBinds::SetGravity);
    mono_add_internal_call("HawkEngine.Rigidbody::GetGravity", (const void*)&EngineBinds::GetGravity);
    mono_add_internal_call("HawkEngine.Rigidbody::SetDamping", (const void*)&EngineBinds::SetDamping);
    mono_add_internal_call("HawkEngine.Rigidbody::GetDamping", (const void*)&EngineBinds::GetDamping);
    mono_add_internal_call("HawkEngine.Rigidbody::SetKinematic", (const void*)&EngineBinds::SetKinematic);
    mono_add_internal_call("HawkEngine.Rigidbody::IsKinematic", (const void*)&EngineBinds::IsKinematic);
    mono_add_internal_call("HawkEngine.Rigidbody::EnableContinuousCollision", (const void*)&EngineBinds::EnableContinuousCollision);

	// Raycast
	mono_add_internal_call("HawkEngine.RayCast::Raycast", (const void*)&EngineBinds::Raycast);

    // Audio
    mono_add_internal_call("HawkEngine.Audio::Play", (const void*)&EngineBinds::Play);
    mono_add_internal_call("HawkEngine.Audio::Stop", (const void*)&EngineBinds::Stop);
    mono_add_internal_call("HawkEngine.Audio::Pause", (const void*)&EngineBinds::Pause);
    mono_add_internal_call("HawkEngine.Audio::Resume", (const void*)&EngineBinds::Resume);
    mono_add_internal_call("HawkEngine.Audio::SetVolume", (const void*)&EngineBinds::SetVolume);
    mono_add_internal_call("HawkEngine.Audio::GetVolume", (const void*)&EngineBinds::GetVolume);
	mono_add_internal_call("HawkEngine.Audio::LoadAudio", (const void*)&EngineBinds::LoadAudioClip);

    // UI Image
    mono_add_internal_call("HawkEngine.UIImage::SetImage", (const void*)&EngineBinds::SetTexture);
	mono_add_internal_call("HawkEngine.UIImage::SetImageEnabled", (const void*)&EngineBinds::SetImageEnabled);
	mono_add_internal_call("HawkEngine.UIImage::SetImageHasAnimation", (const void*)&EngineBinds::SetImageHasAnimation);
    mono_add_internal_call("HawkEngine.UIImage::SetImageAnimationSpeed", (const void*)&EngineBinds::SetImageAnimationSpeed);
	mono_add_internal_call("HawkEngine.UIImage::SetImageAnimationIndexLimit", (const void*)&EngineBinds::SetImageAnimationIndexLimit);
	mono_add_internal_call("HawkEngine.UIImage::SetImageAnimation", (const void*)&EngineBinds::SetImageAnimation);
	mono_add_internal_call("HawkEngine.UIImage::SetImageSpriteSize", (const void*)&EngineBinds::SetImageSpriteSize);
	mono_add_internal_call("HawkEngine.UIImage::SetImageAnimIndex", (const void*)&EngineBinds::SetImageAnimIndex);
	mono_add_internal_call("HawkEngine.UIImage::PlayStopAnimation", (const void*)&EngineBinds::PlayStopAnimation);

	// UI Button
	mono_add_internal_call("HawkEngine.UIButton::GetState", (const void*)&EngineBinds::GetState);

	// UI Transform
	mono_add_internal_call("HawkEngine.UITransform::SetScaleUI", (const void*)&EngineBinds::SetUIScale);
  
    // SkeletalAnimation
	mono_add_internal_call("HawkEngine.SkeletalAnimation::SetAnimationSpeed", (const void*)&EngineBinds::SetAnimationSpeed);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::GetAnimationSpeed", (const void*)&EngineBinds::GetAnimationSpeed);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::SetAnimation", (const void*)&EngineBinds::SetAnimation);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::GetAnimationIndex", (const void*)&EngineBinds::GetAnimationIndex);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::GetAnimationTime", (const void*)&EngineBinds::GetAnimationTime);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::GetAnimationLength", (const void*)&EngineBinds::GetAnimationLength);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::SetAnimationPlayTime", (const void*)&EngineBinds::SetAnimationPlayTime);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::SetAnimationPlayState", (const void*)&EngineBinds::SetAnimationPlayState);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::GetAnimationPlayState", (const void*)&EngineBinds::GetAnimationPlayState);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::TransitionAnimations", (const void*)&EngineBinds::TransitionAnimations);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::SetLoop", (const void*)&EngineBinds::SetLoop);
	mono_add_internal_call("HawkEngine.SkeletalAnimation::PlayAnimOnce", (const void*)&EngineBinds::PlayAnimOnce);

	// Tween
    
	mono_add_internal_call("HawkEngine.Transform::DOMove", (const void*)&EngineBinds::DOMove);
	mono_add_internal_call("HawkEngine.Transform::DOMoveX", (const void*)&EngineBinds::DOMoveX);
	mono_add_internal_call("HawkEngine.Transform::DOMoveY", (const void*)&EngineBinds::DOMoveY);
	mono_add_internal_call("HawkEngine.Transform::DOMoveZ", (const void*)&EngineBinds::DOMoveZ);
	mono_add_internal_call("HawkEngine.Transform::DORotate", (const void*)&EngineBinds::DORotate);
	mono_add_internal_call("HawkEngine.Transform::DORotateX", (const void*)&EngineBinds::DORotateX);
	mono_add_internal_call("HawkEngine.Transform::DORotateY", (const void*)&EngineBinds::DORotateY);
	mono_add_internal_call("HawkEngine.Transform::DORotateZ", (const void*)&EngineBinds::DORotateZ);
	mono_add_internal_call("HawkEngine.Transform::DOScale", (const void*)&EngineBinds::DOScale);
	mono_add_internal_call("HawkEngine.Transform::DOScaleX", (const void*)&EngineBinds::DOScaleX);
	mono_add_internal_call("HawkEngine.Transform::DOScaleY", (const void*)&EngineBinds::DOScaleY);
	mono_add_internal_call("HawkEngine.Transform::DOScaleZ", (const void*)&EngineBinds::DOScaleZ);
	mono_add_internal_call("HawkEngine.UITransform::DOMoveUI", (const void*)&EngineBinds::DOMoveUI);
	mono_add_internal_call("HawkEngine.UITransform::DOMoveXUI", (const void*)&EngineBinds::DOMoveXUI);
    mono_add_internal_call("HawkEngine.UITransform::DOMoveYUI", (const void*)&EngineBinds::DOMoveYUI);
    mono_add_internal_call("HawkEngine.UITransform::DOMoveZUI", (const void*)&EngineBinds::DOMoveZUI);
    mono_add_internal_call("HawkEngine.UITransform::DOScaleUI", (const void*)&EngineBinds::DOScaleUI);
	mono_add_internal_call("HawkEngine.UITransform::DOScaleXUI", (const void*)&EngineBinds::DOScaleXUI);
    mono_add_internal_call("HawkEngine.UITransform::DOScaleYUI", (const void*)&EngineBinds::DOScaleYUI);
    mono_add_internal_call("HawkEngine.UITransform::DOScaleZUI", (const void*)&EngineBinds::DOScaleZUI);
    mono_add_internal_call("HawkEngine.Tweening::DOVector4", (const void*)&EngineBinds::DOColor);
	mono_add_internal_call("HawkEngine.Tweening::DOValue", (const void*)&EngineBinds::DOValue);
	mono_add_internal_call("HawkEngine.Tweening::DOVector3", (const void*)&EngineBinds::DOVec3);
    mono_add_internal_call("HawkEngine.Tweening::CleanTweens", (const void*)&EngineBinds::CleanAllTweens);

	// Scene
	mono_add_internal_call("HawkEngine.SceneManager::LoadSceneInternal", (const void*)&EngineBinds::LoadScene);
	mono_add_internal_call("HawkEngine.SceneManager::SetSceneToPlay", (const void*)&EngineBinds::SetScenePlay);

	// VFX (particles)
	mono_add_internal_call("HawkEngine.ParticleFX::ApplyPreset", (const void*)&EngineBinds::ApplyPreset);
    mono_add_internal_call("HawkEngine.ParticleFX::SetOneShot", (const void*)&EngineBinds::SetOneShot);
	mono_add_internal_call("HawkEngine.ParticleFX::Play", (const void*)&EngineBinds::PlayParticle);
	mono_add_internal_call("HawkEngine.ParticleFX::Stop", (const void*)&EngineBinds::StopParticle);
	mono_add_internal_call("HawkEngine.ParticleFX::EmitBurst", (const void*)&EngineBinds::EmitBurst);
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