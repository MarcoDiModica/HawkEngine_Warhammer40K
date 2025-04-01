#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <chrono>

#include "../MyGameEditor/Log.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"

#pragma warning(disable: 4996)

using ReloadCallbackType = std::function<void(const std::string&)>;

class ScriptHotReloader {
public:
	static ScriptHotReloader& GetInstance();

	ScriptHotReloader(const ScriptHotReloader&) = delete;
	ScriptHotReloader& operator=(const ScriptHotReloader&) = delete;

	void Initialize(const std::string& scriptFolder, const std::string& outputAssemblyDir);
	void RegisterOnReloadCallback(ReloadCallbackType callback);
	bool CheckForChanges();
	void RefreshScriptTimestamps();
	void Update();
	void TryDeleteFile(const std::string& filePath);
	bool ForceRecompile();

private:
	ScriptHotReloader();
	~ScriptHotReloader();

	std::string FindCsprojFile(const std::string& folder);
	bool IsEngineInForeground();
	bool CompileExistingProject();
	std::string FindGeneratedAssembly();
	std::string CleanCompilerMessage(const std::string& message);
	void DisplayCompilationOutput();
	std::string GenerateVersionedFilename(const std::string& baseFilename);
	void CleanupOldVersions(int keepCount);

	bool FindWorkingDotnet();
	bool TestDotnetCompilation(const std::string& dotnetPath);
	void SaveWorkingDotnetPath(const std::string& dotnetPath);
	std::string LoadWorkingDotnetPath();

	std::string m_ScriptFolder;
	std::string m_OutputAssemblyDir;
	std::string m_StagingDirectory;
	std::string m_ProjectFile;
	std::unordered_map<std::string, std::filesystem::file_time_type> m_ScriptTimestamps;
	std::filesystem::file_time_type m_LastCompilationTime;
	std::vector<ReloadCallbackType> m_OnReloadCallbacks;
	bool m_IsCompiling;
	bool m_CompilationCooldown;
	bool m_LastCompilationSuccess;
	std::string m_DotnetPath;
};