#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <mono/jit/jit.h>

#include "ComponentMapper.h"

class MonoManager {
public:
    MonoManager() = default;
    static MonoManager& GetInstance();
    MonoManager(const MonoManager&) = delete;
    MonoManager& operator=(const MonoManager&) = delete;

    void Initialize();
    void Shutdown();
    void ReloadAssembly();

    MonoDomain* GetDomain() const { return m_AppDomain; }
    MonoAssembly* GetAssembly() const { return m_AppAssembly; }
    MonoImage* GetImage() const { return m_AppAssemblyImage; }
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
    void InitMono();
    void ShutdownMono();

    MonoDomain* m_RootDomain = nullptr;
    MonoDomain* m_AppDomain = nullptr;
    MonoAssembly* m_CoreAssembly = nullptr;
    MonoImage* m_CoreAssemblyImage = nullptr;
    MonoAssembly* m_AppAssembly = nullptr;
    MonoImage* m_AppAssemblyImage = nullptr;
    std::filesystem::path m_CoreAssemblyPath;
    std::filesystem::path m_AppAssemblyPath;

    ComponentMapper m_Mapper;
    std::vector<MonoClass*> m_UserClasses;

    std::unordered_map<std::string, int> m_ScriptIDs;
    int m_UserScriptID = 0;
};