#include "MonoManager.h"
#include "EngineBinds.h"
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>
#include <mono/metadata/reflection.h>
#include <iostream>
#include <Windows.h>
#include "../MyGameEditor/Log.h"
#include <mono/metadata/class.h>


// Este metodo hay que moverlo a engineBinds o ScriptingBinds, segun convenga
void HandleConsoleOutput(MonoString* message) /*C# strings are parse by mnono as MonoString */
{
    if (message == nullptr)
        return;

    char* msg = mono_string_to_utf8(message);
    LOG(LogType::LOG_C_SHARP, msg);

    mono_free(msg);
}

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

MonoManager::MonoManager() : domain(nullptr), assembly(nullptr), image(nullptr) {}

MonoManager::~MonoManager() {
}

void MonoManager::Initialize() {
    // Initialize Mono
    std::string path = getExecutablePath() + "\\..\\..\\External\\Mono";
    mono_set_dirs(std::string(path + "\\lib").c_str(),
        std::string(path + "\\etc").c_str());

    domain = mono_jit_init("JITRuntime");
    if (!domain) {
        std::cerr << "Error initializing Mono" << std::endl;
        return;
    }

    // Load the assembly
    std::string assemblyPath = std::string(getExecutablePath() + "\\..\\..\\Script\\obj\\Debug\\Script.dll").c_str();
    assembly = mono_domain_assembly_open(domain, assemblyPath.c_str());
    if (!assembly) {
        std::cerr << "Error loading assembly: " << assemblyPath << std::endl;
        return;
    }

    image = mono_assembly_get_image(assembly);
    if (!image) {
        std::cerr << "Error getting image from assembly" << std::endl;
        return;
    }

    // Link engine methods
    EngineBinds::BindEngine();

    // Set up internal calls
    mono_add_internal_call("HawkEngine.Engineson::print", (const void*)HandleConsoleOutput);

    // Load user classes
    const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int rows = mono_table_info_get_rows(table_info);

    MonoClass* klass = nullptr;

    user_classes.clear();
    for (int i = rows - 1; i > 0; --i)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        if (name[0] != '<')
        {
            const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            klass = mono_class_from_name(image, name_space, name);

            if (klass != nullptr && strcmp(mono_class_get_namespace(klass), "Script") != 0)
            {
                if (!mono_class_is_enum(klass)) {
                    user_classes.push_back(klass);
                }
                //LOG("%s", mono_class_get_name(_class));
            }
        }
    }
}

void MonoManager::Shutdown()
{
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(domain);
    domain = nullptr;
}

MonoClass* MonoManager::GetClass(const std::string& namespaceName, const std::string& className) const {
    return mono_class_from_name(image, namespaceName.c_str(), className.c_str());
}

void MonoManager::ReloadAssembly() {
    // Deshabilitamos el dominio actual, pero no lo descargamos.
    mono_domain_set(mono_get_root_domain(), false);

    // Volver a cargar el ensamblado
    std::string assemblyPath = std::string(getExecutablePath() + "\\..\\..\\Script\\obj\\Debug\\Script.dll").c_str();
    assembly = mono_domain_assembly_open(domain, assemblyPath.c_str());
    if (!assembly) {
		LOG(LogType::LOG_ERROR, "Error loading assembly: %s", assemblyPath.c_str());
        return;
    }

    image = mono_assembly_get_image(assembly);
    if (!image) {
		LOG(LogType::LOG_ERROR, "Error getting image from assembly");
        return;
    }

    // Vuelve a vincular las funciones internas
    EngineBinds::BindEngine();
    mono_add_internal_call("HawkEngine.Engineson::print", (const void*)HandleConsoleOutput);

    // Recargar las clases de usuario
    const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int rows = mono_table_info_get_rows(table_info);

    MonoClass* klass = nullptr;

    user_classes.clear();
    for (int i = rows - 1; i > 0; --i)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        if (name[0] != '<')
        {
            const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            klass = mono_class_from_name(image, name_space, name);

            if (klass != nullptr && strcmp(mono_class_get_namespace(klass), "Script") != 0)
            {
                if (!mono_class_is_enum(klass)) {
                    user_classes.push_back(klass);
                }
            }
        }
    }

    // Vuelve a activar el dominio
    mono_domain_set(domain, true);
	LOG(LogType::LOG_INFO, "Assembly reloaded");
}
