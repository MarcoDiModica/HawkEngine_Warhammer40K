#include <iostream>
#include <fstream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>

#include "MonoManager.h"
#include "ComponentMapper.h"
#include "EngineBinds.h"

#include "../MyGameEditor/Log.h"

namespace Utils
{
    static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
    {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream)
        {
            LOG(LogType::LOG_ERROR, "Failed to open file: %s", filepath.string().c_str());
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = end - stream.tellg();

        if (size == 0)
        {
            LOG(LogType::LOG_ERROR, "File is empty: %s", filepath.string().c_str());
            return nullptr;
        }

        char* buffer = new char[size];
        stream.read((char*)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }
    
    static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
    {
        uint32_t fileSize = 0;
        char* fileData = ReadBytes(assemblyPath, &fileSize);

        MonoImageOpenStatus status;
        MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK)
        {
            const char* errorMessage = mono_image_strerror(status);
            LOG(LogType::LOG_ERROR, "Failed to open image: %s", errorMessage);
            return nullptr;
        }

        std::string assemblyName = assemblyPath.string();
        MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyName.c_str(), &status, 0);
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

            LOG(LogType::LOG_INFO, "Type: %s.%s", nameSpace, name);
        }
    }
}

struct ScriptEngineData 
{
	MonoDomain* RootDomain = nullptr;
	MonoDomain* AppDomain = nullptr;

    MonoAssembly* CoreAssembly = nullptr;
    MonoImage* CoreAssemblyImage = nullptr;

    MonoAssembly* AppAssembly = nullptr;
    MonoImage* AppAssemblyImage = nullptr;

    std::filesystem::path CoreAssemblyFilepath;
    std::filesystem::path AppAssemblyFilepath;

    bool AssemblyReloadPending = false;
};

static ScriptEngineData* s_ScriptEngineData = nullptr;

void MonoManager::Init()
{
	s_ScriptEngineData = new ScriptEngineData();
	
	InitMono();
    //EngineBinds::BindEngine();

    bool status = LoadAssembly("../Script/bin/Hawk-ScriptCore.dll");
    if (!status)
	{
		LOG(LogType::LOG_ERROR, "Failed to load CORE assembly");
		return;
	}

    //LoadAssemblyClasses();

    //register components
}

void MonoManager::Shutdown()
{
	ShutdownMono();
    delete s_ScriptEngineData;
}



void MonoManager::InitMono() 
{
	mono_set_assemblies_path("Mono/lib");

	MonoDomain* rootDomain = mono_jit_init("HawkJITRuntime");
	if (!rootDomain)
    {
		LOG(LogType::LOG_ERROR, "Failed to initialize mono runtime");
		return;
	}

	s_ScriptEngineData->RootDomain = rootDomain;

    mono_thread_set_main(mono_thread_current());
}

void MonoManager::ShutdownMono()
{
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(s_ScriptEngineData->AppDomain);
    s_ScriptEngineData->AppDomain = nullptr;

    mono_jit_cleanup(s_ScriptEngineData->RootDomain);
    s_ScriptEngineData->RootDomain = nullptr;
}

bool MonoManager::LoadAssembly(const std::filesystem::path& filepath)
{
    char appDomainName[] = "HawkScriptRuntime";
    s_ScriptEngineData->AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
    mono_domain_set(s_ScriptEngineData->AppDomain, true);

    s_ScriptEngineData->CoreAssemblyFilepath = filepath;
    s_ScriptEngineData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
    if (!s_ScriptEngineData->CoreAssembly)
    {
        LOG(LogType::LOG_ERROR, "Failed to load assembly: %s", filepath.string().c_str());
        return false;
    }

    s_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->CoreAssembly);

    return true;
}

bool MonoManager::LoadAppAssembly(const std::filesystem::path& filepath)
{
	s_ScriptEngineData->AppAssemblyFilepath = filepath;
	s_ScriptEngineData->AppAssembly = Utils::LoadMonoAssembly(filepath);
	if (!s_ScriptEngineData->AppAssembly)
	{
		LOG(LogType::LOG_ERROR, "Failed to load assembly: %s", filepath.string().c_str());
		return false;
	}

	s_ScriptEngineData->AppAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->AppAssembly);

	return true;
}

void MonoManager::ReloadAssembly()
{
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(s_ScriptEngineData->AppDomain);

    LoadAssembly(s_ScriptEngineData->CoreAssemblyFilepath);
    LoadAppAssembly(s_ScriptEngineData->AppAssemblyFilepath);
    //LoadAssemblyClasses();

    //EngineBinds::BindEngine();
}

// he comentado cosas de la anteror implementacion de mono. sobre todo en GameObject, los componenetes 
// y script component, tambien he comentado el EngineBinds completo