#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include "../MyGameEditor/Log.h"

#pragma warning(disable: 4996)

using ReloadCallbackType = std::function<void(const std::string&)>;

class ScriptHotReloader {
public:
	static ScriptHotReloader& GetInstance() {
		static ScriptHotReloader instance;
		return instance;
	}

	ScriptHotReloader(const ScriptHotReloader&) = delete;
	ScriptHotReloader& operator=(const ScriptHotReloader&) = delete;

	void Initialize(const std::string& scriptFolder, const std::string& outputAssemblyDir) {
		m_ScriptFolder = scriptFolder;
		m_OutputAssemblyDir = outputAssemblyDir;
		m_LastCompilationTime = std::filesystem::file_time_type();
		m_IsCompiling = false;
		m_CompilationCooldown = false;

		LOG(LogType::LOG_INFO, "Inicializando ScriptHotReloader con proyecto existente");
		LOG(LogType::LOG_INFO, "Carpeta de scripts: %s", m_ScriptFolder.c_str());
		LOG(LogType::LOG_INFO, "Directorio de salida: %s", m_OutputAssemblyDir.c_str());

		m_ProjectFile = FindCsprojFile(m_ScriptFolder);
		if (m_ProjectFile.empty()) {
			LOG(LogType::LOG_ERROR, "No se encontró un archivo .csproj en la carpeta de scripts");
		}
		else {
			LOG(LogType::LOG_INFO, "Se usará el proyecto existente: %s", m_ProjectFile.c_str());
		}

		RefreshScriptTimestamps();

		LOG(LogType::LOG_INFO, "ScriptHotReloader inicializado con éxito");
	}

	void RegisterOnReloadCallback(ReloadCallbackType callback) {
		m_OnReloadCallbacks.push_back(callback);
		LOG(LogType::LOG_INFO, "Callback de recarga registrado");
	}

	bool CheckForChanges() {
		if (m_IsCompiling || m_CompilationCooldown) {
			return false;
		}

		bool scriptsModified = false;
		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				auto lastWriteTime = std::filesystem::last_write_time(entry.path());

				auto it = m_ScriptTimestamps.find(entry.path().string());
				if (it == m_ScriptTimestamps.end() || it->second < lastWriteTime) {
					scriptsModified = true;
					LOG(LogType::LOG_INFO, "Script modificado: %s", entry.path().filename().string().c_str());
					break;
				}
			}
		}

		if (scriptsModified) {
			LOG(LogType::LOG_INFO, "Se detectaron cambios en scripts, compilando...");

			m_IsCompiling = true;

			bool result = CompileExistingProject();

			m_CompilationCooldown = true;
			m_LastCompilationSuccess = result;

			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(5));
				m_CompilationCooldown = false;
				m_IsCompiling = false;
				LOG(LogType::LOG_INFO, "Cooldown de compilación finalizado, listo para detectar nuevos cambios");
				}).detach();

			return result;
		}

		return false;
	}

	void RefreshScriptTimestamps() {
		m_ScriptTimestamps.clear();

		int count = 0;
		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				m_ScriptTimestamps[entry.path().string()] = std::filesystem::last_write_time(entry.path());
				count++;
			}
		}

		LOG(LogType::LOG_INFO, "Timestamps actualizados para %d scripts", count);
	}

	void Update() {
		CheckForChanges();
	}

private:
	ScriptHotReloader() : m_IsCompiling(false), m_CompilationCooldown(false), m_LastCompilationSuccess(true) {}
	~ScriptHotReloader() {}

	std::string FindCsprojFile(const std::string& folder) {
		for (const auto& entry : std::filesystem::directory_iterator(folder)) {
			if (entry.path().extension() == ".csproj") {
				return entry.path().string();
			}
		}
		return "";
	}

	bool CompileExistingProject() {
		if (m_ProjectFile.empty()) {
			LOG(LogType::LOG_ERROR, "No se ha encontrado un archivo de proyecto (.csproj)");
			return false;
		}

		LOG(LogType::LOG_INFO, "Compilando proyecto existente: %s", m_ProjectFile.c_str());

		std::string dotnetPath = "C:\\Program Files\\dotnet\\dotnet.exe";
		if (!std::filesystem::exists(dotnetPath)) {
			if (std::filesystem::exists("C:\\Program Files (x86)\\dotnet\\dotnet.exe")) {
				dotnetPath = "C:\\Program Files (x86)\\dotnet\\dotnet.exe";
			}
			else {
				// Intentar encontrar en PATH
				const char* pathEnv = getenv("PATH");
				if (pathEnv) {
					std::string path = pathEnv;
					std::stringstream ss(path);
					std::string item;
					while (std::getline(ss, item, ';')) {
						std::string testPath = item + "\\dotnet.exe";
						if (std::filesystem::exists(testPath)) {
							dotnetPath = testPath;
							break;
						}
					}
				}
			}
		}

		if (!std::filesystem::exists(dotnetPath)) {
			LOG(LogType::LOG_ERROR, "No se pudo encontrar dotnet.exe");
			return false;
		}

		LOG(LogType::LOG_INFO, "Usando dotnet CLI: %s", dotnetPath.c_str());

		std::string batchFile = m_ScriptFolder + "\\build.bat";
		std::ofstream batch(batchFile);
		if (!batch.is_open()) {
			LOG(LogType::LOG_ERROR, "No se pudo crear el archivo batch");
			return false;
		}

		batch << "@echo off" << std::endl;
		batch << "echo Compilando proyecto..." << std::endl;
		batch << "cd /d \"" << m_ScriptFolder << "\"" << std::endl;
		batch << "\"" << dotnetPath << "\" build \"" << m_ProjectFile << "\" -c Release -v quiet" << std::endl;
		batch << "echo Código de salida: %ERRORLEVEL% > build_result.txt" << std::endl;
		batch.close();

		LOG(LogType::LOG_INFO, "Ejecutando batch de compilación...");
		std::string command = "cmd /c \"" + batchFile + "\" > \"" + m_ScriptFolder + "\\build_output.txt\" 2>&1";
		int result = system(command.c_str());
		LOG(LogType::LOG_INFO, "Resultado directo del system(): %d", result);

		int buildResult = -1;
		try {
			std::ifstream resultFile(m_ScriptFolder + "\\build_result.txt");
			if (resultFile.is_open()) {
				std::string line;
				if (std::getline(resultFile, line)) {
					size_t pos = line.find_last_of(":");
					if (pos != std::string::npos && pos + 1 < line.length()) {
						std::string codeStr = line.substr(pos + 1);
						codeStr.erase(0, codeStr.find_first_not_of(" \t"));
						buildResult = std::stoi(codeStr);
					}
				}
				resultFile.close();
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "Error al leer el resultado de la compilación");
		}

		try {
			std::ifstream outputFile(m_ScriptFolder + "\\build_output.txt");
			if (outputFile.is_open()) {
				std::stringstream buffer;
				buffer << outputFile.rdbuf();
				std::string output = buffer.str();
				outputFile.close();
				//LOG(LogType::LOG_INFO, "Salida completa de la compilación: %s", output.c_str());
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "Error al leer la salida de la compilación");
		}

		if (buildResult != 0) {
			LOG(LogType::LOG_ERROR, "Error al compilar proyecto. Código: %d", buildResult);
			return false;
		}

		std::string assemblyPath = FindGeneratedAssembly();
		if (assemblyPath.empty()) {
			LOG(LogType::LOG_ERROR, "No se encontró el assembly generado después de la compilación");
			return false;
		}

		LOG(LogType::LOG_INFO, "Proyecto compilado exitosamente. Assembly: %s", assemblyPath.c_str());

		m_LastCompilationTime = std::filesystem::last_write_time(assemblyPath);
		RefreshScriptTimestamps();

		for (auto& callback : m_OnReloadCallbacks) {
			try {
				callback(assemblyPath);
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "Error en callback de recarga: %s", e.what());
			}
		}

		return true;
	}

	std::string FindGeneratedAssembly() {
		std::string assemblyPath = m_OutputAssemblyDir + "\\Script.dll";
		if (std::filesystem::exists(assemblyPath)) {
			return assemblyPath;
		}

		std::vector<std::string> possiblePaths = {
			m_ScriptFolder + "\\bin\\Debug\\Script.dll",
			m_ScriptFolder + "\\bin\\Release\\Script.dll",
			m_ScriptFolder + "\\bin\\Debug\\netstandard2.0\\Script.dll",
			m_ScriptFolder + "\\bin\\Release\\netstandard2.0\\Script.dll",
			m_ScriptFolder + "\\obj\\Debug\\Script.dll",
			m_ScriptFolder + "\\obj\\Release\\Script.dll",
			m_ScriptFolder + "\\obj\\Debug\\netstandard2.0\\Script.dll",
			m_ScriptFolder + "\\obj\\Release\\netstandard2.0\\Script.dll"
		};

		for (const auto& path : possiblePaths) {
			if (std::filesystem::exists(path)) {
				return path;
			}
		}

		std::string latestDll;
		auto latestTime = std::filesystem::file_time_type::min();

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".dll") {
				auto writeTime = std::filesystem::last_write_time(entry.path());
				if (writeTime > latestTime) {
					latestTime = writeTime;
					latestDll = entry.path().string();
				}
			}
		}

		return latestDll;
	}

	std::string m_ScriptFolder;
	std::string m_OutputAssemblyDir;
	std::string m_ProjectFile;
	std::unordered_map<std::string, std::filesystem::file_time_type> m_ScriptTimestamps;
	std::filesystem::file_time_type m_LastCompilationTime;
	std::vector<ReloadCallbackType> m_OnReloadCallbacks;
	bool m_IsCompiling;
	bool m_CompilationCooldown;
	bool m_LastCompilationSuccess;
};