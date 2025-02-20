#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>

#include "EngineBinds.h"
#include "MonoManager.h"

#include "../MyGameEditor/Log.h"

std::string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    size_t lastSlash = fullPath.find_last_of("\\/");
    return fullPath.substr(0, lastSlash);
}

static char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

static MonoAssembly* LoadMonoAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    delete[] fileData;

    return assembly;
}

static void PrintAssemblyTypes(MonoAssembly* assembly)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        printf("%s.%s\n", nameSpace, name);
    }
}

MonoManager& MonoManager::GetInstance() {
    static MonoManager instance;
    return instance;
}

void MonoManager::Initialize()
{
    s_Data = new ScriptEngineData();

    InitMono();

    bool coreStatus = LoadCoreAssembly("../Script/bin/Script.dll");
    if (!coreStatus)
    {
		return;
	}

    //PrintAssemblyTypes(s_Data->CoreAssembly);

    bool appStatus = LoadAppAssembly("../Script/bin/Script.dll");
    if (!appStatus)
    {
        return;
    }

    //PrintAssemblyTypes(s_Data->AppAssembly);

    EngineBinds::BindEngine();
    LoadScriptClasses();
}

void MonoManager::Shutdown() 
{
    ShutdownMono();
    delete s_Data;
}

void MonoManager::InitMono() 
{
    mono_set_assemblies_path("../External/Mono/lib");

    MonoDomain* rootDomain = mono_jit_init("HawkJITRuntime");
    s_Data->RootDomain = rootDomain;
    if (!s_Data->RootDomain) {

        return;
    }

    mono_thread_set_main(mono_thread_current());
}

void MonoManager::ShutdownMono() {
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(s_Data->AppDomain);
    s_Data->AppDomain = nullptr;

    mono_jit_cleanup(s_Data->RootDomain);
    s_Data->RootDomain = nullptr;
}

bool MonoManager::LoadCoreAssembly(const std::filesystem::path& filepath) 
{
    char appDomainName[] = "HawkJITCoreDomain";
    s_Data->AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
    mono_domain_set(s_Data->AppDomain, true);
    mono_thread_attach(s_Data->AppDomain);
    
    s_Data->CoreAssemblyPath = filepath;
    s_Data->CoreAssembly = LoadMonoAssembly(filepath.string());
    if (!s_Data->CoreAssembly) return false;

    s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
    return true;
}

bool MonoManager::LoadAppAssembly(const std::filesystem::path& filepath) 
{
    s_Data->AppAssemblyPath = filepath;
	s_Data->AppAssembly = LoadMonoAssembly(filepath.string());
	if (!s_Data->AppAssembly) return false;

	s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	return true;
}

void MonoManager::LoadScriptClasses()
{
    m_UserClasses.clear();

    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass* monoBehaviourClass = mono_class_from_name(s_Data->CoreAssemblyImage, "", "MonoBehaviour");

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

        MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);

        if (monoClass == monoBehaviourClass)
            continue;

        bool isMonoBehaviour = mono_class_is_subclass_of(monoClass, monoBehaviourClass, false);
        if (!isMonoBehaviour)
            continue;

        if (!mono_class_is_enum(monoClass)) {
            m_UserClasses.push_back(monoClass);
            LOG(LogType::LOG_INFO, "Found script class: %s%s%s",
                nameSpace[0] ? nameSpace : "",
                nameSpace[0] ? "." : "",
                className);
        }
    }
}

void MonoManager::ReloadAssembly() {
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(s_Data->AppDomain);

    LoadCoreAssembly(s_Data->CoreAssemblyPath);
    LoadAppAssembly(s_Data->AppAssemblyPath);

    LoadScriptClasses();

    EngineBinds::BindEngine();
}

MonoClass* MonoManager::GetClass(const std::string& namespaceName, const std::string& className) const
{
    MonoClass* result = mono_class_from_name(s_Data->AppAssemblyImage, namespaceName.c_str(), className.c_str());
    if (!result) {
        result = mono_class_from_name(s_Data->CoreAssemblyImage, namespaceName.c_str(), className.c_str());
    }
    return result;
}