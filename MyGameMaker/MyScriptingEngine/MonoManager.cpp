#include <Windows.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>

#include "EngineBinds.h"
#include "MonoManager.h"

std::string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    size_t lastSlash = fullPath.find_last_of("\\/");
    return fullPath.substr(0, lastSlash);
}

MonoManager& MonoManager::GetInstance() {
    static MonoManager instance;
    return instance;
}

void MonoManager::InitMono() 
{
    mono_set_assemblies_path("Mono/lib");

    MonoDomain* rootDomain = mono_jit_init("HawkJITRuntime");
    m_RootDomain = rootDomain;
    if (!m_RootDomain) {
        // Handle error
        return;
    }

    char domainName[] = "MyGameApp";
    m_AppDomain = mono_domain_create_appdomain(domainName, nullptr);
    mono_domain_set(m_AppDomain, true);
}

void MonoManager::Initialize()
{
    InitMono();

    std::string corePath = getExecutablePath() + "\\..\\..\\External\\Mono\\lib\\MyGameCore.dll";
    if (!LoadCoreAssembly(corePath)) {
        // Handle error
        return;
    }

    std::string appPath = getExecutablePath() + "\\..\\..\\Script\\obj\\Debug\\Script.dll";
    if (!LoadAppAssembly(appPath)) {
        // Handle error
        return;
    }
}

bool MonoManager::LoadCoreAssembly(const std::filesystem::path& filepath) {
    m_CoreAssemblyPath = filepath;
    m_CoreAssembly = mono_domain_assembly_open(m_AppDomain, filepath.string().c_str());
    if (!m_CoreAssembly) return false;

    m_CoreAssemblyImage = mono_assembly_get_image(m_CoreAssembly);
    return m_CoreAssemblyImage != nullptr;
}

bool MonoManager::LoadAppAssembly(const std::filesystem::path& filepath) {
    m_AppAssemblyPath = filepath;
    m_AppAssembly = mono_domain_assembly_open(m_AppDomain, filepath.string().c_str());
    if (!m_AppAssembly) return false;

    m_AppAssemblyImage = mono_assembly_get_image(m_AppAssembly);
    return m_AppAssemblyImage != nullptr;
}

void MonoManager::ReloadAssembly() {
    mono_domain_set(mono_get_root_domain(), false);
    mono_domain_unload(m_AppDomain);

    InitMono();
    LoadCoreAssembly(m_CoreAssemblyPath);
    LoadAppAssembly(m_AppAssemblyPath);
}

void MonoManager::ShutdownMono() {
    if (m_AppDomain) {
        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(m_AppDomain);
        m_AppDomain = nullptr;
    }

    if (m_RootDomain) {
        mono_jit_cleanup(m_RootDomain);
        m_RootDomain = nullptr;
    }
}

void MonoManager::Shutdown() {
    ShutdownMono();
}

MonoClass* MonoManager::GetClass(const std::string& namespaceName, const std::string& className) const {
    return mono_class_from_name(m_AppAssemblyImage, namespaceName.c_str(), className.c_str());
}