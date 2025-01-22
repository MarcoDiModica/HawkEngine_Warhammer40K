#pragma once
#ifndef MONO_MANAGER_H
#define MONO_MANAGER_H

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <vector>
#include "ComponentMapper.h"
#include <typeindex>
#include <stdexcept>

class MonoManager {
public:
    static MonoManager& GetInstance();

    MonoManager(const MonoManager&) = delete;
    MonoManager& operator=(const MonoManager&) = delete;

    ~MonoManager();

    void Initialize();
    void Shutdown();

    MonoDomain* GetDomain() const { return domain; }
    MonoAssembly* GetAssembly() const { return assembly; }
    MonoImage* GetImage() const { return image; }
    MonoClass* GetClass(const std::string& namespaceName, const std::string& className) const;
    const ComponentMapper& GetMapper() const { return mapper; }


    template <typename T>
    T* GetMappedObject(MonoObject* sharpObject) const;

    std::vector<MonoClass*> GetUserClasses() const { return user_classes; }

private:
    MonoManager();

    MonoDomain* domain;
    MonoAssembly* assembly;
    MonoImage* image;
    ComponentMapper mapper;
    std::vector<MonoClass*> user_classes;
};

template <typename T>
T* MonoManager::GetMappedObject(MonoObject* sharpObject) const {
    if (sharpObject == nullptr) {
        return nullptr;
    }

    std::string typeName = mapper.GetMappedName(std::type_index(typeid(T)));

    uintptr_t Cptr;
    MonoClass* klass = GetClass("HawkEngine", typeName.c_str());

    if (!klass) {
        throw std::runtime_error("NO class C#: " + typeName);
    }

    MonoClassField* field = mono_class_get_field_from_name(klass, "CplusplusInstance");

    if (!field) {
        throw std::runtime_error("CplusplusInstance not found in: " + typeName);
    }

    mono_field_get_value(sharpObject, field, &Cptr);
    return reinterpret_cast<T*>(Cptr);
}

#endif // MONO_MANAGER_H