#include "MonoManager.h"
#include "EngineBinds.h"
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/exception.h>
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include "../MyGameEditor/Log.h"
#include <mono/metadata/class.h>

#include "../MyGameEditor/App.h"

MonoDomain* rootDomain = nullptr;
MonoDomain* scriptDomain = nullptr;

void HandleConsoleOutput(MonoString* message)
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

	rootDomain = mono_jit_init("RootDomain");
	if (!rootDomain) {
		LOG(LogType::LOG_ERROR, "Error inicializando Mono Root Domain");
		return;
	}

	CreateScriptDomain();
}

void MonoManager::CreateScriptDomain() {
	if (scriptDomain) {
		LOG(LogType::LOG_ERROR, "Script domain already exists, should be unloaded first");
		return;
	}

	scriptDomain = mono_domain_create_appdomain((char*)"ScriptDomain", nullptr);
	if (!scriptDomain) {
		LOG(LogType::LOG_ERROR, "Error creating script domain");
		return;
	}

	mono_domain_set(scriptDomain, false);

	domain = scriptDomain;

	assemblyPath = std::string(getExecutablePath() + R"(\..\..\Script\obj\Script.dll)");
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

	EngineBinds::BindEngine();

	mono_add_internal_call("HawkEngine.Engineson::print", (const void*)HandleConsoleOutput);

	LoadUserClasses();
}

void MonoManager::LoadUserClasses() {
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
	if (scriptDomain) {
		UnloadScriptDomain();
	}

	if (rootDomain) {
		mono_jit_cleanup(rootDomain);
		rootDomain = nullptr;
	}
}

void MonoManager::UnloadScriptDomain() {
	if (!scriptDomain) {
		return;
	}

	user_classes.clear();
	image = nullptr;
	assembly = nullptr;

	mono_domain_set(rootDomain, false);

	mono_gc_collect(mono_gc_max_generation());

	mono_domain_unload(scriptDomain);
	scriptDomain = nullptr;
	domain = nullptr;
}

MonoClass* MonoManager::GetClass(const std::string& namespaceName, const std::string& className) const {
	if (!image) {
		LOG(LogType::LOG_ERROR, "No image loaded");
		return nullptr;
	}
	return mono_class_from_name(image, namespaceName.c_str(), className.c_str());
}

void MonoManager::ReloadAssembly(const std::string& newAssemblyPath) {
	assemblyPath = newAssemblyPath;

	UnloadScriptDomain();

	AddUnloadingDelay(200);

	CreateScriptDomain();

	NotifyScriptComponentsToRefresh();
}

void MonoManager::RefreshScriptComponentsRecursive(std::shared_ptr<GameObject> gameObject)
{
	if (!gameObject->scriptComponents.empty()) {
		for (auto& script : gameObject->scriptComponents) {
			script->ResetErrorState();
			script->RefreshScriptInstance();
		}
	}

	for (auto& child : gameObject->GetChildren()) {
		RefreshScriptComponentsRecursive(child);
	}
}

void MonoManager::NotifyScriptComponentsToRefresh() {
	// Aquí implementa la lógica para notificar a todos los ScriptComponent
	// que deben recrear sus instancias de objetos Mono
	// Esto depende de cómo estés rastreando esos componentes

	//hacer refresh de todos los script components de la escena
	for (auto& go : Application->root->GetActiveScene()->children()) {
		RefreshScriptComponentsRecursive(go);
	}
}

void MonoManager::EnableHotReloading() {
	if (!hotReloadEnabled) {
		std::string scriptFolder = getExecutablePath() + "\\..\\..\\Script";
		std::string outputAssemblyDir = getExecutablePath() + "\\..\\..\\Script\\obj";

		ScriptHotReloader::GetInstance().Initialize(scriptFolder, outputAssemblyDir);

		ScriptHotReloader::GetInstance().RegisterOnReloadCallback(
			[this](const std::string& newAssemblyPath) {
				this->OnScriptsRecompiled(newAssemblyPath);
			});

		hotReloadEnabled = true;
	}
}

void MonoManager::DisableHotReloading() {
	hotReloadEnabled = false;
}

void MonoManager::OnScriptsRecompiled(const std::string& newAssemblyPath) {
	if (!std::filesystem::exists(newAssemblyPath)) {
		LOG(LogType::LOG_ERROR, "El archivo del assembly no existe: %s", newAssemblyPath.c_str());
		return;
	}

	std::string errorLogPath = assemblyPath + "\\build_errors.txt";
	if (std::filesystem::exists(errorLogPath)) {
		std::ifstream errorFile(errorLogPath);
		if (errorFile.is_open()) {
			std::string line;
			std::stringstream errorLog;
			bool hasErrors = false;

			while (std::getline(errorFile, line)) {
				if (line.find("error") != std::string::npos) {
					hasErrors = true;
				}
				errorLog << line << "\n";
			}

			if (hasErrors) {
				LOG(LogType::LOG_ERROR, "Compilation errors found:\n%s", errorLog.str().c_str());
				return;
			}
		}
	}

	ReloadAssembly(newAssemblyPath);
}

void MonoManager::AddUnloadingDelay(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}