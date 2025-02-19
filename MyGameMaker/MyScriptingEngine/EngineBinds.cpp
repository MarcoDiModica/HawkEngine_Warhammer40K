//#include <mono/metadata/debug-helpers.h>
//
//#include "EngineBinds.h"
//#include "MonoManager.h"
//#include "ComponentMapper.h"
//#include "ScriptComponent.h"
//
//#include "../MyGameEngine/GameObject.h"
//#include "../MyGameEngine/TransformComponent.h"
//#include "../MyGameEngine/MeshRendererComponent.h"
//#include "../MyGameEngine/CameraComponent.h"
//#include "../MyGameEngine/SceneManager.h"
//#include "../MyGameEngine/InputEngine.h"
//
//#define ADD_INTERNAL_CALL(method, name) mono_add_internal_call(name, (const void*)method)
//
//namespace EngineBinds
//{
//    namespace GameObjectImplementation
//    {
//        MonoObject* GetGameObject(MonoObject* ref) {
//            return GetScriptOwner(ref)->GetSharp();
//        }
//
//        MonoString* GameObjectGetName(MonoObject* sharpRef) {
//            std::string name = ConvertFromSharp(sharpRef)->GetName();
//            return mono_string_new(MonoManager::GetInstance().GetDomain(), name.c_str());
//        }
//
//        GameObject* ConvertFromSharp(MonoObject* sharpObj) {
//            if (!sharpObj) return nullptr;
//
//            uintptr_t Cptr;
//            MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
//            mono_field_get_value(sharpObj, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
//            return reinterpret_cast<GameObject*>(Cptr);
//        }
//
//        MonoObject* CreateGameObjectSharp(MonoString* name, GameObject* Cgo) {
//            char* C_name = mono_string_to_utf8(name);
//
//            if (!Cgo) {
//                auto go = SceneManagement->CreateGameObject(C_name);
//                Cgo = go.get();
//            }
//
//            MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
//            MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
//
//            MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.GameObject:.ctor(string,uintptr)", true);
//            MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
//
//            uintptr_t goPtr = reinterpret_cast<uintptr_t>(Cgo);
//            void* args[2] = {
//                mono_string_new(MonoManager::GetInstance().GetDomain(), Cgo->GetName().c_str()),
//                &goPtr
//            };
//
//            mono_runtime_invoke(method, monoObject, args, NULL);
//            Cgo->CsharpReference = monoObject;
//
//            return monoObject;
//        }
//
//        GameObject* GetScriptOwner(MonoObject* ref) {
//            if (!ref) return nullptr;
//
//            uintptr_t Cptr;
//            MonoClass* klass = MonoManager::GetInstance().GetClass("", "MonoBehaviour");
//            mono_field_get_value(ref, mono_class_get_field_from_name(klass, "CplusplusInstance"), &Cptr);
//            return reinterpret_cast<GameObject*>(Cptr);
//        }
//
//        void GameObjectAddChild(MonoObject* parent, MonoObject* child) {
//            if (!parent || !child) return;
//            ConvertFromSharp(parent)->AddChild(ConvertFromSharp(child));
//        }
//
//        void Destroy(MonoObject* object_to_destroy) {
//            if (!object_to_destroy) return;
//            GameObject* actor = ConvertFromSharp(object_to_destroy);
//            SceneManagement->RemoveGameObject(actor);
//        }
//
//        MonoObject* GetSharpComponent(MonoObject* ref, MonoString* component_name) {
//            char* C_name = mono_string_to_utf8(component_name);
//            auto GO = ConvertFromSharp(ref);
//            std::string componentName = std::string(C_name);
//
//            for (const auto& scriptComponent : GO->scriptComponents) {
//                if (scriptComponent->GetTypeName() == componentName) {
//                    return scriptComponent->GetSharpObject();
//                }
//            }
//
//            if (componentName == "HawkEngine.Transform") {
//                return GO->GetTransform()->GetSharp();
//            }
//            else if (componentName == "HawkEngine.MeshRenderer") {
//                return GO->GetComponent<MeshRenderer>()->GetSharp();
//            }
//            else if (componentName == "HawkEngine.Camera") {
//                return GO->GetComponent<CameraComponent>()->GetSharp();
//            }
//
//            return nullptr;
//        }
//
//        MonoObject* AddSharpComponent(MonoObject* ref, int component) {
//            Component* _component = nullptr;
//            auto go = ConvertFromSharp(ref);
//
//            switch (component) {
//            case 0: _component = static_cast<Component*>(go->AddComponent<Transform_Component>()); break;
//            case 1: _component = static_cast<Component*>(go->AddComponent<MeshRenderer>()); break;
//            case 2: _component = static_cast<Component*>(go->AddComponent<CameraComponent>()); break;
//            }
//
//            for (auto it = MonoManager::GetInstance().scriptIDs.begin();
//                it != MonoManager::GetInstance().scriptIDs.end(); ++it) {
//                if (component == it->second) {
//                    auto script = go->AddComponent<ScriptComponent>();
//                    script->LoadScript(it->first);
//                }
//            }
//
//            return _component->GetSharp();
//        }
//
//        void SetName(MonoObject* ref, MonoString* sharpName) {
//            char* C_name = mono_string_to_utf8(sharpName);
//            ConvertFromSharp(ref)->SetName(std::string(C_name));
//        }
//    }
//
//
//    namespace InputImplementation {
//        bool GetKey(int keyID) {
//            return InputManagement->GetKey(keyID) == KEY_REPEAT;
//        }
//
//        bool GetKeyDown(int keyID) {
//            return InputManagement->GetKey(keyID) == KEY_DOWN;
//        }
//
//        bool GetKeyUp(int keyID) {
//            return InputManagement->GetKey(keyID) == KEY_UP;
//        }
//
//        bool GetMouseButton(int buttonID) {
//            return InputManagement->GetMouseButton(buttonID) == KEY_REPEAT;
//        }
//
//        bool GetMouseButtonDown(int buttonID) {
//            return InputManagement->GetMouseButton(buttonID) == KEY_DOWN;
//        }
//
//        bool GetMouseButtonUp(int buttonID) {
//            return InputManagement->GetMouseButton(buttonID) == KEY_UP;
//        }
//
//        int GetAxis(MonoString* axisName) {
//            return InputManagement->GetAxis(mono_string_to_utf8(axisName));
//        }
//
//        glm::vec3 GetMousePosition() {
//            return glm::vec3(
//                InputManagement->GetMouseX(),
//                InputManagement->GetMouseY(),
//                InputManagement->GetMouseZ()
//            );
//        }
//    }
//
//    namespace TransformImplementation 
//    {
//        void SetPosition(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetPosition(glm::vec3(x, y, z));
//        }
//
//        void SetLocalPosition(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetLocalPosition(glm::vec3(x, y, z));
//        }
//
//        Vector3 GetPosition(MonoObject* transformRef) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            glm::dvec3 p = transform->GetPosition();
//            return Vector3{ (float)p.x, (float)p.y, (float)p.z };
//        }
//
//        Vector3 GetLocalPosition(MonoObject* transformRef) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            glm::dvec3 p = transform->GetLocalPosition()[3];
//            return Vector3{ (float)p.x, (float)p.y, (float)p.z };
//        }
//
//        void SetRotation(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetRotation(glm::vec3(x, y, z));
//        }
//
//        Vector3 GetEulerAngles(MonoObject* transformRef) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            auto v = transform->GetRotation();
//            return Vector3{ (float)v.x, (float)v.y, (float)v.z };
//        }
//
//        void SetRotationQuat(MonoObject* transformRef, glm::quat* rotation) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetRotationQuat(*rotation);
//        }
//
//        void Rotate(MonoObject* transformRef, float radians, glm::vec3* axis) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->Rotate(radians, *axis);
//        }
//
//        void RotateLocal(MonoObject* transformRef, float radians, glm::vec3* axis) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->RotateLocal(radians, *axis);
//        }
//
//        void LookAt(MonoObject* transformRef, glm::vec3* target) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->LookAt(*target);
//        }
//
//        void SetScale(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetScale(glm::vec3(x, y, z));
//        }
//
//        void Scale(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->Scale(glm::vec3(x, y, z));
//        }
//
//        void TranslateLocal(MonoObject* transformRef, float x, float y, float z) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->TranslateLocal(glm::vec3(x, y, z));
//        }
//
//        void AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->AlignToGlobalUp(*worldUp);
//        }
//
//        Vector3 GetForward(MonoObject* transformRef) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            auto forward = transform->GetForward();
//            return Vector3{ (float)forward.x, (float)forward.y, (float)forward.z };
//        }
//
//        void SetForward(MonoObject* transformRef, glm::vec3* forward) {
//            auto transform = GameObjectImplementation::ConvertFromSharpComponent<Transform_Component>(transformRef);
//            if (transform) transform->SetForward(*forward);
//        }
//    }
//
//    namespace CameraImplementation 
//    {
//        void SetFieldOfView(MonoObject* cameraRef, float fov) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) camera->fov = fov;
//        }
//
//        void SetNearClipPlane(MonoObject* cameraRef, float nearClipPlane) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) camera->zNear = nearClipPlane;
//        }
//
//        void SetFarClipPlane(MonoObject* cameraRef, float farClipPlane) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) camera->zFar = farClipPlane;
//        }
//
//        void SetAspectRatio(MonoObject* cameraRef, float aspectRatio) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) camera->aspect = aspectRatio;
//        }
//
//        void SetOrthographicSize(MonoObject* cameraRef, float orthographicSize) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) camera->orthoSize = orthographicSize;
//        }
//
//        void SetProjectionType(MonoObject* cameraRef, int projectionType) {
//            auto camera = GameObjectImplementation::ConvertFromSharpComponent<CameraComponent>(cameraRef);
//            if (camera) {
//                camera->projectionType = (projectionType == 0) ?
//                    ProjectionType::Perspective : ProjectionType::Orthographic;
//            }
//        }
//    }
//
//    namespace MeshRendererImplementation
//    {
//        void SetMesh(MonoObject* meshRendererRef, MonoObject* meshRef) 
//        {
//            if (!meshRendererRef || !meshRef) return;
//
//            auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//            auto mesh = ConvertFromSharpComponent<Mesh>(meshRef);
//
//            if (meshRenderer && mesh) {
//                meshRenderer->SetMesh(std::shared_ptr<Mesh>(mesh));
//            }
//        }
//
//        MonoObject* GetMesh(MonoObject* meshRendererRef) 
//        {
//            if (!meshRendererRef) return nullptr;
//
//            auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//            if (!meshRenderer) return nullptr;
//
//            auto mesh = meshRenderer->GetMesh();
//            return mesh ? mesh->GetSharp() : nullptr;
//        }
//
//        void SetMaterial(MonoObject* meshRendererRef, MonoObject* materialRef) 
//        {
//            if (!meshRendererRef || !materialRef) return;
//
//            auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//            auto material = ConvertFromSharpComponent<Material>(materialRef);
//
//            if (meshRenderer && material) {
//                meshRenderer->SetMaterial(std::shared_ptr<Material>(material));
//            }
//        }
//
//        MonoObject* GetMaterial(MonoObject* meshRendererRef) 
//        {
//            if (!meshRendererRef) return nullptr;
//
//            auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//            if (!meshRenderer) return nullptr;
//
//            auto material = meshRenderer->GetMaterial();
//            return material ? material->GetSharp() : nullptr;
//        }
//
//        void SetColor(MonoObject* meshRendererRef, glm::vec3* color) {
//            if (!meshRendererRef || !color) return;
//
//            auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//            if (meshRenderer) 
//            {
//                meshRenderer->SetColor(*color);
//            }
//        }
//
//        void GetColor(MonoObject* meshRendererRef, glm::vec3* color) {
//			if (!meshRendererRef || !color) return;
//
//			auto meshRenderer = GameObjectImplementation::ConvertFromSharpComponent<MeshRenderer>(meshRendererRef);
//			if (meshRenderer) 
//			{
//				*color = meshRenderer->GetColor();
//			}
//    }
//}
//
//void EngineBinds::BindEngine() 
//{
//    // GameObject bindings
//    ADD_INTERNAL_CALL(GameObjectImplementation::GetGameObject, "MonoBehaviour::GetGameObject");
//    ADD_INTERNAL_CALL(GameObjectImplementation::CreateGameObjectSharp, "HawkEngine.Engineson::CreateGameObject");
//    ADD_INTERNAL_CALL(GameObjectImplementation::GameObjectGetName, "HawkEngine.GameObject::GetName");
//    ADD_INTERNAL_CALL(GameObjectImplementation::SetName, "HawkEngine.GameObject::SetName");
//    ADD_INTERNAL_CALL(GameObjectImplementation::GameObjectAddChild, "HawkEngine.GameObject::AddChild");
//    ADD_INTERNAL_CALL(GameObjectImplementation::Destroy, "HawkEngine.Engineson::Destroy");
//    ADD_INTERNAL_CALL(GameObjectImplementation::GetSharpComponent, "HawkEngine.GameObject::TryGetComponent");
//    ADD_INTERNAL_CALL(GameObjectImplementation::AddSharpComponent, "HawkEngine.GameObject::TryAddComponent");
//
//    ADD_INTERNAL_CALL(InputImplementation::GetKey, "HawkEngine.Input::GetKey");
//    ADD_INTERNAL_CALL(InputImplementation::GetKeyDown, "HawkEngine.Input::GetKeyDown");
//    ADD_INTERNAL_CALL(InputImplementation::GetKeyUp, "HawkEngine.Input::GetKeyUp");
//    ADD_INTERNAL_CALL(InputImplementation::GetMouseButton, "HawkEngine.Input::GetMouseButton");
//    ADD_INTERNAL_CALL(InputImplementation::GetMouseButtonDown, "HawkEngine.Input::GetMouseButtonDown");
//    ADD_INTERNAL_CALL(InputImplementation::GetMouseButtonUp, "HawkEngine.Input::GetMouseButtonUp");
//    ADD_INTERNAL_CALL(InputImplementation::GetAxis, "HawkEngine.Input::GetAxis");
//    ADD_INTERNAL_CALL(InputImplementation::GetMousePosition, "HawkEngine.Input::GetMousePosition");
//
//    ADD_INTERNAL_CALL(TransformImplementation::SetPosition, "HawkEngine.Transform::SetPosition");
//    ADD_INTERNAL_CALL(TransformImplementation::GetPosition, "HawkEngine.Transform::GetPosition");
//    ADD_INTERNAL_CALL(TransformImplementation::SetLocalPosition, "HawkEngine.Transform::SetLocalPosition");
//    ADD_INTERNAL_CALL(TransformImplementation::GetLocalPosition, "HawkEngine.Transform::GetLocalPosition");
//    ADD_INTERNAL_CALL(TransformImplementation::SetRotation, "HawkEngine.Transform::SetRotation");
//    ADD_INTERNAL_CALL(TransformImplementation::GetEulerAngles, "HawkEngine.Transform::GetEulerAngles");
//    ADD_INTERNAL_CALL(TransformImplementation::SetRotationQuat, "HawkEngine.Transform::SetRotationQuat");
//    ADD_INTERNAL_CALL(TransformImplementation::Rotate, "HawkEngine.Transform::Rotate");
//    ADD_INTERNAL_CALL(TransformImplementation::RotateLocal, "HawkEngine.Transform::RotateLocal");
//    ADD_INTERNAL_CALL(TransformImplementation::LookAt, "HawkEngine.Transform::LookAt");
//    ADD_INTERNAL_CALL(TransformImplementation::SetScale, "HawkEngine.Transform::SetScale");
//    ADD_INTERNAL_CALL(TransformImplementation::Scale, "HawkEngine.Transform::Scale");
//    ADD_INTERNAL_CALL(TransformImplementation::TranslateLocal, "HawkEngine.Transform::TranslateLocal");
//    ADD_INTERNAL_CALL(TransformImplementation::AlignToGlobalUp, "HawkEngine.Transform::AlignToGlobalUp");
//    ADD_INTERNAL_CALL(TransformImplementation::SetForward, "HawkEngine.Transform::SetForward");
//    ADD_INTERNAL_CALL(TransformImplementation::GetForward, "HawkEngine.Transform::GetForward");
//
//    ADD_INTERNAL_CALL(CameraImplementation::SetFieldOfView, "HawkEngine.Camera::SetCameraFieldOfView");
//    ADD_INTERNAL_CALL(CameraImplementation::SetNearClipPlane, "HawkEngine.Camera::SetCameraNearClipPlane");
//    ADD_INTERNAL_CALL(CameraImplementation::SetFarClipPlane, "HawkEngine.Camera::SetCameraFarClipPlane");
//    ADD_INTERNAL_CALL(CameraImplementation::SetAspectRatio, "HawkEngine.Camera::SetCameraAspectRatio");
//    ADD_INTERNAL_CALL(CameraImplementation::SetOrthographicSize, "HawkEngine.Camera::SetCameraOrthographicSize");
//    ADD_INTERNAL_CALL(CameraImplementation::SetProjectionType, "HawkEngine.Camera::SetCameraProjectionType");
//
//    ADD_INTERNAL_CALL(MeshRendererImplementation::SetMesh, "HawkEngine.MeshRenderer::SetMesh");
//    ADD_INTERNAL_CALL(MeshRendererImplementation::GetMesh, "HawkEngine.MeshRenderer::GetMesh");
//    ADD_INTERNAL_CALL(MeshRendererImplementation::SetMaterial, "HawkEngine.MeshRenderer::SetMaterial");
//    ADD_INTERNAL_CALL(MeshRendererImplementation::GetMaterial, "HawkEngine.MeshRenderer::GetMaterial");
//    ADD_INTERNAL_CALL(MeshRendererImplementation::SetColor, "HawkEngine.MeshRenderer::SetColor");
//}