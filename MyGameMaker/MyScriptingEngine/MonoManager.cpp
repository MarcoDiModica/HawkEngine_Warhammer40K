#include "MonoManager.h"

#include <iostream>
#include <fstream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

struct ScriptEngineData {
	MonoDomain* RootDomain = nullptr;
	MonoDomain* AppDomain = nullptr;

    MonoAssembly* CoreAssembly = nullptr;
};

static ScriptEngineData* s_ScriptEngineData = nullptr;

void MonoManager::Init() {
	s_ScriptEngineData = new ScriptEngineData();
	
	InitMono();
}

void MonoManager::Shutdown() {
	delete s_ScriptEngineData;
}

char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        std::cout << "Failed to open file: " << filepath << std::endl;
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        std::cout << "File is empty: " << filepath << std::endl;
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        std::cout << "Failed to open image: " << errorMessage << std::endl;
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

void MonoManager::InitMono() {
	mono_set_assemblies_path("../External/Mono/lib");

	MonoDomain* rootDomain = mono_jit_init("HawkJITRuntime");
	if (!rootDomain) {
		std::cout << "Failed to initialize Mono runtime" << std::endl;
		return;
	}

	s_ScriptEngineData->RootDomain = rootDomain;

	char appDomainName[] = "HawkScriptRuntime";
	s_ScriptEngineData->AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
	mono_domain_set(s_ScriptEngineData->AppDomain, true);

    // move this maybe
    s_ScriptEngineData->CoreAssembly = LoadCSharpAssembly("../Script/bin/Hawk-ScriptCore.dll");
    if (!s_ScriptEngineData->CoreAssembly)
	{
		std::cout << "Failed to load core assembly" << std::endl;
		return;
	}
    PrintAssemblyTypes(s_ScriptEngineData->CoreAssembly);
}

void MonoManager::ShutdownMono() {
	mono_jit_cleanup(s_ScriptEngineData->RootDomain);
}

// he comentado cosas de la anteror implementacion de mono. sobre todo en GameObject, los componenetes 
// y script component, tambien he comentado el EngineBinds