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
#include <unordered_set>

#include "../MyGameEditor/Log.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"

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

		//LOG(LogType::LOG_INFO, "Script Folder: %s", m_ScriptFolder.c_str());
		//LOG(LogType::LOG_INFO, "Output Folder: %s", m_OutputAssemblyDir.c_str());

		m_ProjectFile = FindCsprojFile(m_ScriptFolder);
		if (m_ProjectFile.empty()) {
			LOG(LogType::LOG_ERROR, "File .csproj not found");
		}

		RefreshScriptTimestamps();

		m_IsCompiling = true;
		bool initialCompilationResult = CompileExistingProject();
		m_IsCompiling = false;
		m_LastCompilationSuccess = initialCompilationResult;

		if (!initialCompilationResult) {
			LOG(LogType::LOG_ERROR, "Initial compilation failed. Please solve the errors or ask Hawk Developers :)");
		}
	}

	void RegisterOnReloadCallback(ReloadCallbackType callback) {
		m_OnReloadCallbacks.push_back(callback);
	}

	bool CheckForChanges() {
		if (m_IsCompiling || m_CompilationCooldown) {
			return false;
		}

		if (!m_LastCompilationSuccess) {
			LOG(LogType::LOG_INFO, "Compilation Failed. Please fix it :0");
			return false;
		}

		if (!IsEngineInForeground()) {
			return false;
		}

		bool scriptsModified = false;
		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				auto lastWriteTime = std::filesystem::last_write_time(entry.path());

				auto it = m_ScriptTimestamps.find(entry.path().string());
				if (it == m_ScriptTimestamps.end() || it->second < lastWriteTime) {
					scriptsModified = true;
					break;
				}
			}
		}

		if (scriptsModified) {
			m_IsCompiling = true;

			bool result = CompileExistingProject();

			m_CompilationCooldown = true;
			m_LastCompilationSuccess = result;

			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(5));
				m_CompilationCooldown = false;
				m_IsCompiling = false;
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
	}

	void Update() {
		CheckForChanges();
	}

	void TryDeleteFile(const std::string& filePath) {
		try {
			if (std::filesystem::exists(filePath)) {
				std::filesystem::remove(filePath);
			}
		}
		catch (...) {
		}
	}

	bool ForceRecompile() {
		if (m_IsCompiling || m_CompilationCooldown) {
			LOG(LogType::LOG_INFO, "Compilation already in progress or cooling down. Please wait.");
			return false;
		}

		LOG(LogType::LOG_INFO, "Forcing recompilation of scripts...");
		m_IsCompiling = true;

		bool result = CompileExistingProject();

		m_CompilationCooldown = true;
		m_LastCompilationSuccess = result;

		std::thread([this]() {
			std::this_thread::sleep_for(std::chrono::seconds(5));
			m_CompilationCooldown = false;
			m_IsCompiling = false;
			}).detach();

		if (result) {
			LOG(LogType::LOG_INFO, "Forced recompilation succeeded.");
		}
		else {
			LOG(LogType::LOG_ERROR, "Forced recompilation failed.");
		}

		return result;
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

	bool IsEngineInForeground() {
		return Application->window->IsForeground();
	}

	bool CompileExistingProject() {
		if (m_ProjectFile.empty()) {
			LOG(LogType::LOG_ERROR, "File (.csproj) not found!");
			return false;
		}

		std::string dotnetPath = "C:\\Program Files\\dotnet\\dotnet.exe";
		if (!std::filesystem::exists(dotnetPath)) {
			if (std::filesystem::exists("C:\\Program Files (x86)\\dotnet\\dotnet.exe")) {
				dotnetPath = "C:\\Program Files (x86)\\dotnet\\dotnet.exe";
			}
			else {
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
			LOG(LogType::LOG_ERROR, "Can't find dotnet.exe pls download .NET Framework in Visual Installer or contact Hawk Developers :0");
			return false;
		}

		TryDeleteFile(m_ScriptFolder + "\\build_output.txt");
		TryDeleteFile(m_ScriptFolder + "\\build_result.txt");
		TryDeleteFile(m_ScriptFolder + "\\build_warnings.txt");

		std::string batchFile = m_ScriptFolder + "\\build.bat";
		std::ofstream batch(batchFile);
		if (!batch.is_open()) {
			LOG(LogType::LOG_ERROR, "Couldn't create batch file");
			return false;
		}

		batch << "@echo off" << std::endl;
		batch << "echo Compiling project please raise your hands" << std::endl;
		batch << "cd /d \"" << m_ScriptFolder << "\"" << std::endl;
		batch << "\"" << dotnetPath << "\" build \"" << m_ProjectFile << "\" -c Release > build_output.txt 2>&1" << std::endl;
		batch << "echo Código de salida: %ERRORLEVEL% > build_result.txt" << std::endl;

		batch << "findstr /C:\"warning\" build_output.txt > build_warnings.txt 2>nul" << std::endl;
		batch << "findstr /C:\"error\" build_output.txt > build_errors.txt 2>nul" << std::endl;

		batch.close();

		std::string command = "cmd /c \"" + batchFile + "\"";
		int result = system(command.c_str());

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
			LOG(LogType::LOG_ERROR, "Error reading compilation results");
		}

		DisplayCompilationOutput();

		bool hasErrors = false;
		try {
			std::ifstream errorsFile(m_ScriptFolder + "\\build_errors.txt");
			if (errorsFile.is_open() && errorsFile.peek() != std::ifstream::traits_type::eof()) {
				hasErrors = true;
				errorsFile.close();
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "Error checking for compilation errors");
		}

		if (buildResult != 0 || hasErrors) {
			LOG(LogType::LOG_ERROR, "Error compiling the project. Code: %d. Fix it or contact Hawk Developers.", buildResult);
			return false;
		}

		std::string assemblyPath = FindGeneratedAssembly();
		if (assemblyPath.empty()) {
			LOG(LogType::LOG_ERROR, "Assembly not found");
			return false;
		}

		m_LastCompilationTime = std::filesystem::last_write_time(assemblyPath);
		RefreshScriptTimestamps();

		for (auto& callback : m_OnReloadCallbacks) {
			try {
				callback(assemblyPath);
			}
			catch (const std::exception& e) {
				LOG(LogType::LOG_ERROR, "Reload callback error: %s", e.what());
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

	std::string CleanCompilerMessage(const std::string& message) {
		size_t lastSlash = message.find_last_of("\\/", message.find('('));
		if (lastSlash == std::string::npos) return message;

		size_t filenameStart = lastSlash + 1;
		size_t filenameEnd = message.find('(', filenameStart);
		if (filenameEnd == std::string::npos) return message;

		std::string filename = message.substr(filenameStart, filenameEnd - filenameStart);

		size_t lineInfoStart = filenameEnd;
		size_t lineInfoEnd = message.find(')', lineInfoStart);
		if (lineInfoEnd == std::string::npos) return message;

		std::string lineInfo = message.substr(lineInfoStart, lineInfoEnd - lineInfoStart + 1);

		size_t messageStart = lineInfoEnd + 2; 
		size_t projectBracketStart = message.find(" [", messageStart);

		std::string errorMessage;
		if (projectBracketStart != std::string::npos) {
			errorMessage = message.substr(messageStart, projectBracketStart - messageStart);
		}
		else {
			errorMessage = message.substr(messageStart);
		}

		return filename + lineInfo + ": " + errorMessage;
	}

	void DisplayCompilationOutput() {
		try {
			std::ifstream warningsFile(m_ScriptFolder + "\\build_warnings.txt");
			if (warningsFile.is_open()) {
				std::string line;
				std::unordered_set<std::string> uniqueWarnings;

				bool hasWarnings = false;
				while (std::getline(warningsFile, line)) {
					if (!line.empty()) {
						hasWarnings = true;
						std::string cleanedWarning = CleanCompilerMessage(line);
						if (uniqueWarnings.find(cleanedWarning) == uniqueWarnings.end()) {
							LOG(LogType::LOG_C_SHARP_WARNING, "C# Warning: %s", cleanedWarning.c_str());
							uniqueWarnings.insert(cleanedWarning);
						}
					}
				}

				if (hasWarnings) {
					LOG(LogType::LOG_C_SHARP_WARNING, "--- End of warnings ---");
				}

				warningsFile.close();
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "Error reading compilation warnings");
		}

		try {
			std::ifstream errorsFile(m_ScriptFolder + "\\build_errors.txt");
			if (errorsFile.is_open()) {
				std::string line;
				std::unordered_set<std::string> uniqueErrors;

				bool hasErrors = false;
				while (std::getline(errorsFile, line)) {
					if (!line.empty()) {
						hasErrors = true;
						std::string cleanedError = CleanCompilerMessage(line);
						if (uniqueErrors.find(cleanedError) == uniqueErrors.end()) {
							LOG(LogType::LOG_ERROR, "C# Error: %s", cleanedError.c_str());
							uniqueErrors.insert(cleanedError);
						}
					}
				}

				if (hasErrors) {
					LOG(LogType::LOG_ERROR, "--- End of errors ---");
				}

				errorsFile.close();
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "Error reading compilation errors");
		}
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