#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>

#include "ComponentMapper.h"

struct ScriptEngineData
{
    MonoDomain* RootDomain = nullptr;
    MonoDomain* AppDomain = nullptr;

    MonoAssembly* CoreAssembly = nullptr;
    MonoImage* CoreAssemblyImage = nullptr;

    MonoAssembly* AppAssembly = nullptr;
    MonoImage* AppAssemblyImage = nullptr;

    std::filesystem::path CoreAssemblyPath;
    std::filesystem::path AppAssemblyPath;
};

class MonoManager {
public:
    MonoManager() = default;
    static MonoManager& GetInstance();
    MonoManager(const MonoManager&) = delete;
    MonoManager& operator=(const MonoManager&) = delete;

    void Initialize();
    void Shutdown();
    void ReloadAssembly();

    MonoDomain* GetDomain() const { return s_Data->AppDomain; }
    MonoDomain* GetRootDomain() const { return s_Data->RootDomain; }
    MonoAssembly* GetAssembly() const { return s_Data->AppAssembly; }
    MonoAssembly* GetCoreAssembly() const { return s_Data->CoreAssembly; }
    MonoImage* GetImage() const { return s_Data->AppAssemblyImage; }
    MonoImage* GetCoreImage() const { return s_Data->CoreAssemblyImage; }
    MonoClass* GetClass(const std::string& namespaceName, const std::string& className) const;
    const ComponentMapper& GetMapper() const { return m_Mapper; }
    std::vector<MonoClass*> GetUserClasses() const { return m_UserClasses; }

    const std::unordered_map<std::string, int>& GetScriptIDs() const { return m_ScriptIDs; }
    bool HasScriptID(const std::string& scriptName) const { return m_ScriptIDs.contains(scriptName); }
    void AddScriptID(const std::string& scriptName) { m_ScriptIDs.emplace(scriptName, --m_UserScriptID); }
    int GetScriptID(const std::string& scriptName) const { return m_ScriptIDs.at(scriptName); }

private:
    bool LoadCoreAssembly(const std::filesystem::path& filepath);
    bool LoadAppAssembly(const std::filesystem::path& filepath);

    void LoadScriptClasses();
    
    void InitMono();
    void ShutdownMono();

    ScriptEngineData* s_Data = nullptr;

    ComponentMapper m_Mapper;
    std::vector<MonoClass*> m_UserClasses;

    std::unordered_map<std::string, int> m_ScriptIDs;
    int m_UserScriptID = 0;
};