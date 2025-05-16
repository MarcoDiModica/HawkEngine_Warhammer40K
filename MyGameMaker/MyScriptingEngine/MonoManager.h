#pragma once
#ifndef MONO_MANAGER_H
#define MONO_MANAGER_H

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <vector>
#include <map>
#include "ComponentMapper.h"
#include <typeindex>
#include <stdexcept>
#include "ScriptHotReloader.h"
#include "ScriptComponent.h"

class MonoManager {
public:
	static MonoManager& GetInstance();

	MonoManager(const MonoManager&) = delete;
	MonoManager& operator=(const MonoManager&) = delete;

	~MonoManager();

	void Initialize();
	void Shutdown();

	void CreateScriptDomain();
	void UnloadScriptDomain();
	void LoadUserClasses();
	void NotifyScriptComponentsToRefresh();

	MonoDomain* GetDomain() const { return domain; }
	MonoAssembly* GetAssembly() const { return assembly; }
	MonoImage* GetImage() const { return image; }
	MonoClass* GetClass(const std::string& namespaceName, const std::string& className) const;
	const ComponentMapper& GetMapper() const { return mapper; }

	void EnableHotReloading();
	void DisableHotReloading();
	bool IsHotReloadingEnabled() const { return hotReloadEnabled; }
	void OnScriptsRecompiled(const std::string& newAssemblyPath);
	void ReloadAssembly(const std::string& assemblyPath);

	void RefreshScriptComponentsRecursive(std::shared_ptr<GameObject> gameObject);
	template <typename T>
	T* GetMappedObject(MonoObject* sharpObject) const;

	std::vector<MonoClass*> GetUserClasses() const { return user_classes; }

	std::map<std::string, int> scriptIDs;

	int GetNewScriptClassID() {
		userScriptID--;
		return userScriptID;
	}

	bool ForceRecompileScripts() {
		if (!hotReloadEnabled) {
			LOG(LogType::LOG_ERROR, "Hot reloading is not enabled. Enable it before forcing recompilation.");
			return false;
		}
		return ScriptHotReloader::GetInstance().ForceRecompile();
	}

	void AddUnloadingDelay(int milliseconds = 200);

	void CreateNewScript(std::string scriptName);
	void AddScriptToProject(const std::string& scriptName);
	void RemoveScriptFromProject(const std::string& scriptName);

	MonoObject* CreateGameObjectReference(GameObject* nativeGO);

	MonoObject* CreatePrefabReference(const std::string& path);

	std::vector<std::string> scriptNames;
private:
	MonoManager();
	MonoImage* currentImage = nullptr;

	// Keep track of threads we've attached
	std::vector<MonoThread*> attachedThreads;
	MonoDomain* domain;
	MonoAssembly* assembly;
	MonoImage* image;
	ComponentMapper mapper;
	std::vector<MonoClass*> user_classes;

	bool hotReloadEnabled = false; 
	std::string assemblyPath;

	int userScriptID = 0;
};

template <typename T>
T* MonoManager::GetMappedObject(MonoObject* sharpObject) const {
	if (sharpObject == nullptr) {
		return nullptr;
	}

	std::string typeName = mapper.GetMappedName(std::type_index(typeid(T)));

	uintptr_t Cptr;
	MonoClass* klass = GetClass("HawkEngine", typeName);

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