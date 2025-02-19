//#pragma once
//
//#include <memory>
//
//#include <mono/metadata/object.h>
//#include <glm/vec3.hpp>
//#include <glm/gtc/quaternion.hpp>
//
//class GameObject;
//class Material;
//class Mesh;
//
//#define ADD_INTERNAL_CALL(method, name) mono_add_internal_call(name, (const void*)method)
//
//struct Vector3 {
//    float X, Y, Z;
//};
//
//namespace EngineBinds
//{
//    void BindEngine();
//
//    namespace GameObjectImplementation
//	{
//        MonoObject* CreateGameObjectSharp(MonoString* name, GameObject* Cgo = nullptr);
//        MonoObject* GetGameObject(MonoObject* ref);
//        GameObject* ConvertFromSharp(MonoObject* sharpObj);
//        GameObject* GetScriptOwner(MonoObject* ref);
//        void GameObjectAddChild(MonoObject* parent, MonoObject* child);
//        void Destroy(MonoObject* object_to_destroy);
//        MonoString* GameObjectGetName(MonoObject* sharpRef);
//        void SetName(MonoObject* ref, MonoString* sharpName);
//        MonoObject* GetSharpComponent(MonoObject* ref, MonoString* component_name);
//        MonoObject* AddSharpComponent(MonoObject* ref, int component);
//        template <class T>
//        T* ConvertFromSharpComponent(MonoObject* sharpComp);
//	}
//
//    namespace InputImplementation 
//    {
//        bool GetKey(int keyID);
//        bool GetKeyDown(int keyID);
//        bool GetKeyUp(int keyID);
//        bool GetMouseButton(int buttonID);
//        bool GetMouseButtonDown(int buttonID);
//        bool GetMouseButtonUp(int buttonID);
//        int GetAxis(MonoString* axisName);
//        glm::vec3 GetMousePosition();
//    }
//
//    namespace TransformImplementation
//    {
//        void SetPosition(MonoObject* transformRef, float x, float y, float z);
//        void SetLocalPosition(MonoObject* transformRef, float x, float y, float z);
//        Vector3 GetPosition(MonoObject* transformRef);
//        Vector3 GetLocalPosition(MonoObject* transformRef);
//        void SetRotation(MonoObject* transformRef, float x, float y, float z);
//        void SetRotationQuat(MonoObject* transformRef, glm::quat* rotation);
//        Vector3 GetEulerAngles(MonoObject* transformRef);
//        void Rotate(MonoObject* transformRef, float radians, glm::vec3* axis);
//        void RotateLocal(MonoObject* transformRef, float radians, glm::vec3* axis);
//        void LookAt(MonoObject* transformRef, glm::vec3* target);
//        void SetScale(MonoObject* transformRef, float x, float y, float z);
//        void Scale(MonoObject* transformRef, float x, float y, float z);
//        void TranslateLocal(MonoObject* transformRef, float x, float y, float z);
//        void AlignToGlobalUp(MonoObject* transformRef, glm::vec3* worldUp);
//        Vector3 GetForward(MonoObject* transformRef);
//        void SetForward(MonoObject* transformRef, glm::vec3* forward);
//    }
//
//    namespace CameraImplementation 
//    {
//        void SetFieldOfView(MonoObject* cameraRef, float fov);
//        void SetNearClipPlane(MonoObject* cameraRef, float nearClipPlane);
//        void SetFarClipPlane(MonoObject* cameraRef, float farClipPlane);
//        void SetAspectRatio(MonoObject* cameraRef, float aspectRatio);
//        void SetOrthographicSize(MonoObject* cameraRef, float orthographicSize);
//        void SetProjectionType(MonoObject* cameraRef, int projectionType);
//    }
//
//    namespace MeshRendererImplementation
//    {
//        void SetMesh(MonoObject* meshRendererRef, MonoObject* meshRef);
//        MonoObject* GetMesh(MonoObject* meshRendererRef);
//        void SetMaterial(MonoObject* meshRendererRef, MonoObject* materialRef);
//        MonoObject* GetMaterial(MonoObject* meshRendererRef);
//        void SetColor(MonoObject* meshRendererRef, glm::vec3* color);
//        void GetColor(MonoObject* meshRendererRef, glm::vec3* color);
//        void Render(MonoObject* meshRendererRef);
//    }
//}
//
//namespace EnigneBinds
//{
//    namespace GameObjectImplementation
//	{
//		template <class T>
//        T* ConvertFromSharpComponent(MonoObject* sharpComp) {
//            if (sharpComp == nullptr) {
//                return nullptr;
//            }
//
//            auto& mapper = MonoManager::GetInstance().GetMapper();
//            std::string typeName = mapper.GetMappedName(std::type_index(typeid(T)));
//
//            uintptr_t Cptr;
//            MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", typeName.c_str());
//
//            std::cerr << "Looking for class : " << typeName << std::endl;
//
//            if (!klass) {
//                throw std::runtime_error("Class not found in the C#: " + typeName);
//            }
//
//            MonoClassField* field = mono_class_get_field_from_name(klass, "CplusplusInstance");
//
//            if (!field) {
//                throw std::runtime_error("field CplusplusInstance not found: " + typeName);
//            }
//
//            mono_field_get_value(sharpComp, field, &Cptr);
//            return reinterpret_cast<T*>(Cptr);
//        }
//	}
//}