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

	AddUnloadingDelay(400);

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

void MonoManager::CreateNewScript(std::string scriptName) {
	std::string scriptPath = getExecutablePath() + R"(\..\..\Script\)" + scriptName + ".cs";

	std::ifstream checkFile(scriptPath);
	if (checkFile.is_open()) {
		LOG(LogType::LOG_WARNING, "Script already exists: %s", scriptPath.c_str());
		checkFile.close();
		return;
	}

	std::string scriptTemplate =
		R"(using System;
using System.Collections.Generic;
using HawkEngine;

public class )" + scriptName + R"( : MonoBehaviour
{
	// Variables públicas que se mostrarán en el inspector
	public float value = 3.0f;
	public bool isActive = true;

	// Variables privadas
	private Transform transform;

	// Inicialización al cargar el script
	public override void Start()
	{
		transform = GetComponent<Transform>();
		Engineson.print("Script " + gameObject.name + " started");
	}

	// Actualización cada frame
	public override void Update(float deltaTime)
	{
		if (isActive)
		{
			// Lógica de actualización
		}
	}

	// Llamado cuando ocurre una colisión
	public override void OnCollisionEnter(GameObject other)
	{
		Engineson.print($"Collision with {other.name}");
	}

	// Llamado cuando otra entidad entra en un trigger
	public override void OnTriggerEnter(GameObject other)
	{
		Engineson.print($"Trigger with {other.name}");
	}

	// Métodos personalizados
	public void CustomMethod()
	{
		// Tu código aquí
	}
}
		)";

	std::ofstream newScriptFile(scriptPath);
	if (newScriptFile.is_open()) {
		newScriptFile << scriptTemplate;
		newScriptFile.close();
	}
	else {
		LOG(LogType::LOG_ERROR, "Error while creating the new script: %s", scriptPath.c_str());
		return;
	}

	AddScriptToProject(scriptName);

	AddUnloadingDelay(200);

	ForceRecompileScripts();
}

void MonoManager::AddScriptToProject(const std::string& scriptName) {
	std::string csprojPath = getExecutablePath() + R"(\..\..\Script\C#Assembly.csproj)";

	std::ifstream checkFile(csprojPath);
	if (!checkFile.is_open()) {
		LOG(LogType::LOG_ERROR, "Not found .csproj: %s", csprojPath.c_str());
		return;
	}

	std::stringstream buffer;
	buffer << checkFile.rdbuf();
	std::string content = buffer.str();
	checkFile.close();

	std::string searchString = "<Compile Include=\"" + scriptName + ".cs\"";
	if (content.find(searchString) != std::string::npos) {
		LOG(LogType::LOG_INFO, "Alredy included in the project: %s", scriptName.c_str());
		return;
	}

	size_t insertPos = content.find("</ItemGroup>");
	if (insertPos == std::string::npos) {
		insertPos = content.find("<ItemGroup>");
		if (insertPos == std::string::npos) {
			LOG(LogType::LOG_ERROR, "Cant find where to include the script in the .csproj");
			return;
		}

		insertPos = content.find('>', insertPos) + 1;
	}

	std::string newLine = "    <Compile Include=\"" + scriptName + ".cs\" />\n  ";

	content.insert(insertPos, newLine);

	std::ofstream outFile(csprojPath);
	if (!outFile.is_open()) {
		LOG(LogType::LOG_ERROR, "Cant open the .csproj for writing: %s", csprojPath.c_str());
		return;
	}

	outFile << content;
	outFile.close();
}

void MonoManager::RemoveScriptFromProject(const std::string& scriptName)
{
	std::string csprojPath = getExecutablePath() + R"(\..\..\Script\C#Assembly.csproj)";

	std::ifstream checkFile(csprojPath);
	if (!checkFile.is_open()) {
		LOG(LogType::LOG_ERROR, "Not found .csproj: %s", csprojPath.c_str());
		return;
	}

	std::stringstream buffer;
	buffer << checkFile.rdbuf();
	std::string content = buffer.str();
	checkFile.close();

	std::string searchString = "<Compile Include=\"" + scriptName + ".cs\"";
	size_t pos = content.find(searchString);
	if (pos == std::string::npos) {
		LOG(LogType::LOG_INFO, "Not included in the project: %s", scriptName.c_str());
		return;
	}

	size_t start = content.rfind("<Compile Include=", pos);
	size_t end = content.find("/>", pos) + 2;

	content.erase(start, end - start);

	std::ofstream outFile(csprojPath);
	if (!outFile.is_open()) {
		LOG(LogType::LOG_ERROR, "Cant open the .csproj for writing: %s", csprojPath.c_str());
		return;
	}

	outFile << content;
	outFile.close();

	std::string scriptPath = getExecutablePath() + R"(\..\..\Script\)" + scriptName + ".cs";
	if (std::filesystem::exists(scriptPath)) {
		std::filesystem::remove(scriptPath);
	}

	AddUnloadingDelay(200);

	ForceRecompileScripts();
}
