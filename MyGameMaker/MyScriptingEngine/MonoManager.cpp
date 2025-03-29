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
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
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
	Shutdown();
}

void MonoManager::Initialize() {
	std::string path = getExecutablePath() + "\\..\\..\\External\\Mono";
	mono_set_dirs(std::string(path + "\\lib").c_str(),
		std::string(path + "\\etc").c_str());

	domain = mono_jit_init("MyGameDomain");
	if (!domain) {
		std::cerr << "Error initializing Mono" << std::endl;
		return;
	}

	assemblyPath = std::string(getExecutablePath() + R"(\..\..\Script\obj\Script.dll)");
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

	EngineBinds::BindEngine();

	mono_add_internal_call("HawkEngine.Engineson::print", (const void*)HandleConsoleOutput);

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
}

void MonoManager::Shutdown() {
	if (domain) {
		mono_jit_cleanup(domain);
		domain = nullptr;
		assembly = nullptr;
		image = nullptr;
	}
}

MonoClass* MonoManager::GetClass(const std::string& namespaceName, const std::string& className) const {
	return mono_class_from_name(image, namespaceName.c_str(), className.c_str());
}

void MonoManager::ReloadAssembly(const std::string& assemblyPath) {
	LOG(LogType::LOG_INFO, "Recargando assembly: %s", assemblyPath.c_str());

	// Cerrar el assembly actual si existe
	if (assembly) {
		mono_assembly_close(assembly);
		assembly = nullptr;
		image = nullptr;
	}

	// Cargar el nuevo assembly
	assembly = mono_domain_assembly_open(domain, assemblyPath.c_str());
	if (!assembly) {
		LOG(LogType::LOG_ERROR, "Error al cargar el assembly: %s", assemblyPath.c_str());
		return;
	}

	// Obtener la imagen del assembly
	image = mono_assembly_get_image(assembly);
	if (!image) {
		LOG(LogType::LOG_ERROR, "Error al obtener la imagen del assembly");
		return;
	}

	// Cargar las clases de usuario
	user_classes.clear();
	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);

	MonoClass* klass = nullptr;

	for (int i = rows - 1; i > 0; --i) {
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		if (name[0] != '<') {
			const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			klass = mono_class_from_name(image, name_space, name);

			if (klass != nullptr && strcmp(mono_class_get_namespace(klass), "Script") != 0) {
				if (!mono_class_is_enum(klass)) {
					user_classes.push_back(klass);
				}
			}
		}
	}

	LOG(LogType::LOG_INFO, "Assembly recargado exitosamente");
}

void MonoManager::EnableHotReloading() {
	if (!hotReloadEnabled) {
		LOG(LogType::LOG_INFO, "Activando hot-reloading de scripts...");

		std::string scriptFolder = getExecutablePath() + "\\..\\..\\Script";
		std::string outputAssemblyDir = getExecutablePath() + "\\..\\..\\Script\\obj";

		// Inicializar el hot reloader
		ScriptHotReloader::GetInstance().Initialize(scriptFolder, outputAssemblyDir);

		// Registrar callback con la nueva firma que incluye la ruta del assembly
		ScriptHotReloader::GetInstance().RegisterOnReloadCallback(
			[this](const std::string& newAssemblyPath) {
				this->OnScriptsRecompiled(newAssemblyPath);
			});

		hotReloadEnabled = true;
		LOG(LogType::LOG_INFO, "Hot-reloading de scripts activado exitosamente");
	}
}

void MonoManager::DisableHotReloading() {
	hotReloadEnabled = false;
}

void MonoManager::OnScriptsRecompiled(const std::string& newAssemblyPath) {
	LOG(LogType::LOG_INFO, "Scripts recompilados, recargando assembly: %s", newAssemblyPath.c_str());

	// Verificar que el archivo existe
	if (!std::filesystem::exists(newAssemblyPath)) {
		LOG(LogType::LOG_ERROR, "El archivo del assembly no existe: %s", newAssemblyPath.c_str());
		return;
	}

	// Recargar el assembly
	ReloadAssembly(newAssemblyPath);
}