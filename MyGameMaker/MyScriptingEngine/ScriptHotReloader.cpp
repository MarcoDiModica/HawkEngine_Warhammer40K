#include "ScriptHotReloader.h"
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>  
#include <sstream>
#include <thread>
#include <unordered_set>
#include <algorithm>
#include <iomanip>

ScriptHotReloader& ScriptHotReloader::GetInstance() {
	static ScriptHotReloader instance;
	return instance;
}

ScriptHotReloader::ScriptHotReloader() : m_IsCompiling(false), m_CompilationCooldown(false), m_LastCompilationSuccess(true), m_PreferMSBuild(false) {
	m_PreferMSBuild = LoadBuildPreference();
}

ScriptHotReloader::~ScriptHotReloader() {
}

void ScriptHotReloader::Initialize(const std::string& scriptFolder, const std::string& outputAssemblyDir) {
	m_ScriptFolder = scriptFolder;
	m_OutputAssemblyDir = outputAssemblyDir;
	m_StagingDirectory = m_ScriptFolder + "\\staging";
	m_LastCompilationTime = std::filesystem::file_time_type();
	m_IsCompiling = false;
	m_CompilationCooldown = false;

	try {
		if (!std::filesystem::exists(m_StagingDirectory)) {
			std::filesystem::create_directories(m_StagingDirectory);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Failed to create staging directory: %s", e.what());
	}

	m_ProjectFile = FindCsprojFile(m_ScriptFolder);
	if (m_ProjectFile.empty()) {
		LOG(LogType::LOG_ERROR, "File .csproj not found");
		return;
	}

	RefreshScriptTimestamps();

	m_DotnetPath = LoadWorkingDotnetPath();
	m_MSBuildPath = LoadWorkingMSBuildPath();
	m_PreferMSBuild = LoadBuildPreference();

	bool dotnetAvailable = false;
	bool msbuildAvailable = false;

	if (m_PreferMSBuild) {
		if (!m_MSBuildPath.empty() && std::filesystem::exists(m_MSBuildPath)) {
			msbuildAvailable = TestMSBuildCompilation(m_MSBuildPath);
		}

		if (!msbuildAvailable) {
			msbuildAvailable = FindWorkingMSBuild();
		}

		if (!msbuildAvailable) {
			LOG(LogType::LOG_WARNING, "MSBuild not available, falling back to dotnet...");

			if (!m_DotnetPath.empty() && std::filesystem::exists(m_DotnetPath)) {
				dotnetAvailable = TestDotnetCompilation(m_DotnetPath);
			}

			if (!dotnetAvailable) {
				dotnetAvailable = FindWorkingDotnet();
			}
		}
	}
	else {
		if (!m_DotnetPath.empty() && std::filesystem::exists(m_DotnetPath)) {
			dotnetAvailable = TestDotnetCompilation(m_DotnetPath);
		}

		if (!dotnetAvailable) {
			dotnetAvailable = FindWorkingDotnet();
		}

		if (!dotnetAvailable) {
			LOG(LogType::LOG_WARNING, "Dotnet not available, trying MSBuild as fallback...");

			if (!m_MSBuildPath.empty() && std::filesystem::exists(m_MSBuildPath)) {
				msbuildAvailable = TestMSBuildCompilation(m_MSBuildPath);
			}

			if (!msbuildAvailable) {
				msbuildAvailable = FindWorkingMSBuild();
			}
		}
	}

	if (!dotnetAvailable && !msbuildAvailable) {
		LOG(LogType::LOG_ERROR, "Failed to find a working dotnet or MSBuild installation. Please install .NET SDK or Visual Studio.");
		return;
	}

	LOG(LogType::LOG_INFO, "Hot reloading initialization complete. Use ForceRecompile to compile scripts.");
}

void ScriptHotReloader::RegisterOnReloadCallback(ReloadCallbackType callback) {
	m_OnReloadCallbacks.push_back(callback);
}

bool ScriptHotReloader::CheckForChanges() {
	if (m_IsCompiling || m_CompilationCooldown) {
		return false;
	}

	if (!m_LastCompilationSuccess) {
		LOG(LogType::LOG_INFO, "Previous compilation failed. Please fix errors.");
		return false;
	}

	if (!IsEngineInForeground()) {
		return false;
	}

	bool scriptsModified = false;
	std::vector<std::string> modifiedFiles;

	for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
		if (entry.path().extension() == ".cs") {
			auto lastWriteTime = std::filesystem::last_write_time(entry.path());

			auto it = m_ScriptTimestamps.find(entry.path().string());
			if (it == m_ScriptTimestamps.end() || it->second < lastWriteTime) {
				scriptsModified = true;
				modifiedFiles.push_back(entry.path().filename().string());

				m_ScriptTimestamps[entry.path().string()] = lastWriteTime;
			}
		}
	}

	if (scriptsModified) {
		if (Application->root->GetActiveScene()->sceneState == Scene::SceneState::PLAY) {
			LOG(LogType::LOG_ERROR, "Script changes detected, but engine is in play mode. Please stop the game to reload scripts.");
			return false;
		}

		LOG(LogType::LOG_INFO, "Detected changes in %d script(s):", modifiedFiles.size());
		for (const auto& file : modifiedFiles) {
			LOG(LogType::LOG_INFO, "  - %s", file.c_str());
		}

		m_IsCompiling = true;

		if (m_PreferMSBuild && !m_MSBuildPath.empty()) {
			std::string cleanCmd = "\"" + m_MSBuildPath + "\" \"" + m_ProjectFile +
				"\" /t:Clean /p:Configuration=Release /nologo";
			ExecuteSilentProcess(cleanCmd, m_ScriptFolder);
		}
		else if (!m_DotnetPath.empty()) {
			std::string cleanCmd = "\"" + m_DotnetPath + "\" clean \"" + m_ProjectFile + "\"";
			ExecuteSilentProcess(cleanCmd, m_ScriptFolder);
		}

		bool result = false;
		bool dotnetAvailable = !m_DotnetPath.empty() && std::filesystem::exists(m_DotnetPath);
		bool msbuildAvailable = !m_MSBuildPath.empty() && std::filesystem::exists(m_MSBuildPath);

		if (m_PreferMSBuild && msbuildAvailable) {
			result = CompileWithMSBuild();
		}
		else if (!m_PreferMSBuild && dotnetAvailable) {
			result = CompileExistingProject();
		}
		else if (msbuildAvailable) {
			result = CompileWithMSBuild();
		}
		else if (dotnetAvailable) {
			result = CompileExistingProject();
		}
		else {
			LOG(LogType::LOG_ERROR, "No working dotnet or MSBuild available for compilation.");
		}

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

void ScriptHotReloader::RefreshScriptTimestamps() {
	m_ScriptTimestamps.clear();

	int count = 0;
	for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
		if (entry.path().extension() == ".cs") {
			m_ScriptTimestamps[entry.path().string()] = std::filesystem::last_write_time(entry.path());
			count++;
		}
	}
}

void ScriptHotReloader::Update() {
	CheckForChanges();
}

void ScriptHotReloader::TryDeleteFile(const std::string& filePath) {
	try {
		if (std::filesystem::exists(filePath)) {
			std::filesystem::remove(filePath);
		}
	}
	catch (...) {
	}
}

bool ScriptHotReloader::ForceRecompile() {
	if (m_IsCompiling) {
		LOG(LogType::LOG_INFO, "Compilation already in progress. Please wait.");
		return false;
	}

	if (Application->root->GetActiveScene()->sceneState == Scene::SceneState::PLAY) {
		LOG(LogType::LOG_ERROR, "Engine is in play mode. Please stop the game to reload scripts.");
		return false;
	}

	if (m_PreferMSBuild && !m_MSBuildPath.empty()) {
		std::string cleanCmd = "\"" + m_MSBuildPath + "\" \"" + m_ProjectFile +
			"\" /t:Clean /p:Configuration=Release /nologo";
		ExecuteSilentProcess(cleanCmd, m_ScriptFolder);
	}
	else if (!m_DotnetPath.empty()) {
		std::string cleanCmd = "\"" + m_DotnetPath + "\" clean \"" + m_ProjectFile + "\"";
		ExecuteSilentProcess(cleanCmd, m_ScriptFolder);
	}

	RefreshScriptTimestamps();

	m_IsCompiling = true;
	m_CompilationCooldown = false;

	bool result = false;
	bool dotnetAvailable = !m_DotnetPath.empty() && std::filesystem::exists(m_DotnetPath);
	bool msbuildAvailable = !m_MSBuildPath.empty() && std::filesystem::exists(m_MSBuildPath);

	Application->CleanLogs();

	if (m_PreferMSBuild && msbuildAvailable) {
		result = CompileWithMSBuild();
	}
	else if (!m_PreferMSBuild && dotnetAvailable) {
		result = CompileExistingProject();
	}
	else if (msbuildAvailable) {
		result = CompileWithMSBuild();
	}
	else if (dotnetAvailable) {
		result = CompileExistingProject();
	}
	else {
		LOG(LogType::LOG_ERROR, "No working dotnet or MSBuild available for compilation.");
	}

	RefreshScriptTimestamps();

	m_LastCompilationSuccess = result;

	m_CompilationCooldown = true;
	std::thread([this]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		m_CompilationCooldown = false;
		m_IsCompiling = false;
		}).detach();

	if (!result) {
		LOG(LogType::LOG_ERROR, "Forced recompilation failed.");
	}
	else {
		LOG(LogType::LOG_INFO, "Forced recompilation completed successfully.");
	}

	return result;
}

std::string ScriptHotReloader::FindCsprojFile(const std::string& folder) {
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.path().extension() == ".csproj") {
			return entry.path().string();
		}
	}
	return "";
}

bool ScriptHotReloader::IsEngineInForeground() {
	return Application->window->IsForeground();
}

bool ScriptHotReloader::FindWorkingDotnet() {
	std::vector<std::string> standardDotnetPaths;
	standardDotnetPaths.push_back("C:\\Program Files\\dotnet\\dotnet.exe");
	standardDotnetPaths.push_back("C:\\Program Files (x86)\\dotnet\\dotnet.exe");

	std::vector<std::string> existingStandardPaths;
	for (const auto& path : standardDotnetPaths) {
		if (std::filesystem::exists(path)) {
			existingStandardPaths.push_back(path);
		}
	}

	for (const auto& dotnetPath : existingStandardPaths) {
		if (TestDotnetCompilation(dotnetPath)) {
			m_DotnetPath = dotnetPath;
			SaveWorkingDotnetPath(dotnetPath);
			return true;
		}
	}

	std::vector<std::string> potentialVSDotnetPaths;

	const std::vector<std::string> vsCommonPaths = {
		"C:\\Program Files\\Microsoft Visual Studio",
		"C:\\Program Files (x86)\\Microsoft Visual Studio"
	};

	const std::vector<std::string> vsYears = {
		"2022", "2019", "2017", "2015"
	};

	const std::vector<std::string> vsEditions = {
		"Enterprise", "Professional", "Community", "BuildTools"
	};

	for (const auto& basePath : vsCommonPaths) {
		if (!std::filesystem::exists(basePath)) continue;

		try {
			for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
				if (!entry.is_directory()) continue;

				for (const auto& year : vsYears) {
					std::string yearPath = entry.path().string() + "\\" + year;
					if (!std::filesystem::exists(yearPath)) continue;

					for (const auto& edition : vsEditions) {
						std::string editionPath = yearPath + "\\" + edition;
						if (!std::filesystem::exists(editionPath)) continue;

						std::vector<std::string> dotnetLocations = {
							editionPath + "\\MSBuild\\Current\\Bin\\dotnet.exe",
							editionPath + "\\MSBuild\\Current\\Bin\\amd64\\dotnet.exe",
							editionPath + "\\MSBuild\\15.0\\Bin\\dotnet.exe",
							editionPath + "\\MSBuild\\15.0\\Bin\\amd64\\dotnet.exe",
							editionPath + "\\Common7\\IDE\\dotnet.exe",
							editionPath + "\\Common7\\Tools\\dotnet.exe",
							editionPath + "\\dotnet\\dotnet.exe",
							editionPath + "\\dotnet\\net8.0\\runtime\\dotnet.exe",
							editionPath + "\\dotnet\\net7.0\\runtime\\dotnet.exe",
							editionPath + "\\dotnet\\net6.0\\runtime\\dotnet.exe",
							editionPath + "\\dotnet\\net5.0\\runtime\\dotnet.exe",
							editionPath + "\\dotnet\\netcoreapp3.1\\runtime\\dotnet.exe",
							editionPath + "\\SDK\\dotnet.exe"
						};

						try {
							for (auto it = std::filesystem::recursive_directory_iterator(editionPath, std::filesystem::directory_options::skip_permission_denied);
								it != std::filesystem::recursive_directory_iterator(); ++it) {
								if (it.depth() > 4) {
									it.disable_recursion_pending();
									continue;
								}

								if (it->path().filename() == "dotnet.exe") {
									dotnetLocations.push_back(it->path().string());
								}
							}
						}
						catch (const std::filesystem::filesystem_error&) {
						}

						for (const auto& location : dotnetLocations) {
							if (std::filesystem::exists(location)) {
								potentialVSDotnetPaths.push_back(location);
							}
						}
					}
				}
			}
		}
		catch (const std::filesystem::filesystem_error&) {
		}
	}

	const char* pathEnv = getenv("PATH");
	if (pathEnv) {
		std::string path = pathEnv;
		std::stringstream ss(path);
		std::string item;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				std::string testPath = item;
				if (testPath.back() != '\\' && testPath.back() != '/') {
					testPath += '\\';
				}
				testPath += "dotnet.exe";

				if (std::filesystem::exists(testPath) &&
					std::find(potentialVSDotnetPaths.begin(), potentialVSDotnetPaths.end(), testPath) == potentialVSDotnetPaths.end()) {
					potentialVSDotnetPaths.push_back(testPath);
				}
			}
		}
	}

	if (!potentialVSDotnetPaths.empty()) {
		for (const auto& dotnetPath : potentialVSDotnetPaths) {
			if (TestDotnetCompilation(dotnetPath)) {
				m_DotnetPath = dotnetPath;
				SaveWorkingDotnetPath(dotnetPath);
				return true;
			}
		}
	}

	LOG(LogType::LOG_ERROR, "No working dotnet installation found. Will try MSBuild.");
	return false;
}

bool ScriptHotReloader::TestDotnetCompilation(const std::string& dotnetPath) {
	if (!std::filesystem::exists(dotnetPath)) {
		return false;
	}

	std::string versionCommand = "\"" + dotnetPath + "\" --version";
	int exitCode = ExecuteSilentProcess(versionCommand);

	return (exitCode == 0);
}

void ScriptHotReloader::SaveWorkingDotnetPath(const std::string& dotnetPath) {
	try {
		std::string configFile = m_ScriptFolder + "\\dotnet_config.txt";
		std::ofstream file(configFile);
		if (file.is_open()) {
			file << dotnetPath;
			file.close();
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Failed to save working dotnet path configuration");
	}
}

std::string ScriptHotReloader::LoadWorkingDotnetPath() {
	try {
		std::string configFile = m_ScriptFolder + "\\dotnet_config.txt";
		if (std::filesystem::exists(configFile)) {
			std::ifstream file(configFile);
			if (file.is_open()) {
				std::string savedPath;
				std::getline(file, savedPath);
				file.close();

				if (!savedPath.empty() && std::filesystem::exists(savedPath)) {
					return savedPath;
				}
			}
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Failed to load saved dotnet path configuration");
	}

	return "";
}

bool ScriptHotReloader::CompileExistingProject() {
	if (m_ProjectFile.empty()) {
		LOG(LogType::LOG_ERROR, "File (.csproj) not found!");
		return false;
	}

	if (m_DotnetPath.empty() || !std::filesystem::exists(m_DotnetPath)) {
		LOG(LogType::LOG_ERROR, "Valid dotnet installation not found. Please install .NET SDK or check MSBuild option.");
		return false;
	}

	TryDeleteFile(m_ScriptFolder + "\\build_output.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_result.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_warnings.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_errors.txt");
	TryDeleteFile(m_OutputAssemblyDir + "\\Script.dll");
	TryDeleteFile(m_ScriptFolder + "\\bin\\Debug\\Script.dll");
	TryDeleteFile(m_ScriptFolder + "\\bin\\Release\\Script.dll");

	std::string outputFile = m_ScriptFolder + "\\build_output.txt";
	std::string buildCommand = "\"" + m_DotnetPath + "\" build \"" + m_ProjectFile +
		"\" -c Release > \"" + outputFile + "\" 2>&1";

	int exitCode = ExecuteSilentProcess(buildCommand, m_ScriptFolder);

	std::ofstream resultFile(m_ScriptFolder + "\\build_result.txt");
	if (resultFile.is_open()) {
		resultFile << "Código de salida: " << exitCode;
		resultFile.close();
	}

	ExecuteSilentProcess("findstr /C:\"warning\" \"" + outputFile + "\" > \"" +
		m_ScriptFolder + "\\build_warnings.txt\" 2>nul", m_ScriptFolder);
	ExecuteSilentProcess("findstr /C:\"error\" \"" + outputFile + "\" > \"" +
		m_ScriptFolder + "\\build_errors.txt\" 2>nul", m_ScriptFolder);

	int buildResult = -1;
	try {
		std::ifstream resultFileStream(m_ScriptFolder + "\\build_result.txt");
		if (resultFileStream.is_open()) {
			std::string line;
			if (std::getline(resultFileStream, line)) {
				size_t pos = line.find_last_of(":");
				if (pos != std::string::npos && pos + 1 < line.length()) {
					std::string codeStr = line.substr(pos + 1);
					codeStr.erase(0, codeStr.find_first_not_of(" \t"));
					buildResult = std::stoi(codeStr);
				}
			}
			resultFileStream.close();
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
		LOG(LogType::LOG_ERROR, "Error compiling the project with dotnet. Code: %d. Fix it or try MSBuild.", buildResult);
		return false;
	}

	std::string assemblyPath = FindGeneratedAssembly();
	if (assemblyPath.empty()) {
		LOG(LogType::LOG_ERROR, "Assembly not found");
		return false;
	}

	std::string versionedFilename = GenerateVersionedFilename("Script.dll");
	std::string stagedPath = m_StagingDirectory + "\\" + versionedFilename;

	CleanupOldVersions(0);

	try {
		std::filesystem::copy(assemblyPath, stagedPath, std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::filesystem::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Failed to create versioned assembly: %s", e.what());
		stagedPath = assemblyPath;
	}

	m_LastCompilationTime = std::filesystem::last_write_time(assemblyPath);
	RefreshScriptTimestamps();

	for (auto& callback : m_OnReloadCallbacks) {
		try {
			callback(stagedPath);
		}
		catch (const std::exception& e) {
			LOG(LogType::LOG_ERROR, "Reload callback error: %s", e.what());
		}
	}

	return true;
}

std::string ScriptHotReloader::FindGeneratedAssembly() {
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

std::string ScriptHotReloader::CleanCompilerMessage(const std::string& message) {
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

void ScriptHotReloader::DisplayCompilationOutput() {
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

			errorsFile.close();
		}
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Error reading compilation errors");
	}
}

std::string ScriptHotReloader::GenerateVersionedFilename(const std::string& baseFilename) {
	return "currentAssembly.dll";
}

void ScriptHotReloader::CleanupOldVersions(int keepCount) {
	try {
		std::string currentPath = m_StagingDirectory + "\\currentAssembly.dll";
		std::string prev1Path = m_StagingDirectory + "\\previousAssembly1.dll";
		std::string prev2Path = m_StagingDirectory + "\\previousAssembly2.dll";

		if (std::filesystem::exists(prev2Path)) {
			std::filesystem::remove(prev2Path);
		}

		if (std::filesystem::exists(prev1Path)) {
			std::filesystem::rename(prev1Path, prev2Path);
		}

		if (std::filesystem::exists(currentPath)) {
			std::filesystem::rename(currentPath, prev1Path);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Error managing assembly versions: %s", e.what());
	}
}

bool ScriptHotReloader::FindWorkingMSBuild() {
	std::vector<std::string> potentialMSBuildPaths;

	const std::vector<std::string> vsCommonPaths = {
		"C:\\Program Files\\Microsoft Visual Studio",
		"C:\\Program Files (x86)\\Microsoft Visual Studio"
	};

	const std::vector<std::string> vsYears = {
		"2022", "2019", "2017", "2015"
	};

	const std::vector<std::string> vsEditions = {
		"Enterprise", "Professional", "Community", "BuildTools"
	};

	for (const auto& basePath : vsCommonPaths) {
		if (!std::filesystem::exists(basePath)) continue;

		try {
			for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
				if (!entry.is_directory()) continue;

				for (const auto& year : vsYears) {
					std::string yearPath = entry.path().string() + "\\" + year;
					if (!std::filesystem::exists(yearPath)) continue;

					for (const auto& edition : vsEditions) {
						std::string editionPath = yearPath + "\\" + edition;
						if (!std::filesystem::exists(editionPath)) continue;

						std::vector<std::string> msbuildLocations = {
							editionPath + "\\MSBuild\\Current\\Bin\\MSBuild.exe",
							editionPath + "\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe",
							editionPath + "\\MSBuild\\15.0\\Bin\\MSBuild.exe",
							editionPath + "\\MSBuild\\15.0\\Bin\\amd64\\MSBuild.exe"
						};

						try {
							for (auto it = std::filesystem::recursive_directory_iterator(editionPath, std::filesystem::directory_options::skip_permission_denied);
								it != std::filesystem::recursive_directory_iterator(); ++it) {
								if (it.depth() > 4) {
									it.disable_recursion_pending();
									continue;
								}

								if (it->path().filename() == "MSBuild.exe") {
									msbuildLocations.push_back(it->path().string());
								}
							}
						}
						catch (const std::filesystem::filesystem_error&) {
						}

						for (const auto& location : msbuildLocations) {
							if (std::filesystem::exists(location)) {
								potentialMSBuildPaths.push_back(location);
							}
						}
					}
				}
			}
		}
		catch (const std::filesystem::filesystem_error&) {
		}
	}

	const char* pathEnv = getenv("PATH");
	if (pathEnv) {
		std::string path = pathEnv;
		std::stringstream ss(path);
		std::string item;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				std::string testPath = item;
				if (testPath.back() != '\\' && testPath.back() != '/') {
					testPath += '\\';
				}
				testPath += "MSBuild.exe";

				if (std::filesystem::exists(testPath) &&
					std::find(potentialMSBuildPaths.begin(), potentialMSBuildPaths.end(), testPath) == potentialMSBuildPaths.end()) {
					potentialMSBuildPaths.push_back(testPath);
				}
			}
		}
	}

	for (const auto& msbuildPath : potentialMSBuildPaths) {
		if (TestMSBuildCompilation(msbuildPath)) {
			m_MSBuildPath = msbuildPath;
			SaveWorkingMSBuildPath(msbuildPath);
			return true;
		}
	}

	LOG(LogType::LOG_WARNING, "No working MSBuild installation found.");
	return false;
}

bool ScriptHotReloader::TestMSBuildCompilation(const std::string& msbuildPath) {
	if (m_ProjectFile.empty()) {
		LOG(LogType::LOG_ERROR, "No .csproj file found for testing MSBuild!");
		return false;
	}

	std::string testOutputFile = m_ScriptFolder + "\\test_msbuild_output.txt";
	std::string testErrorFile = m_ScriptFolder + "\\test_msbuild_error.txt";
	std::string resultFile = m_ScriptFolder + "\\test_msbuild_result.txt";

	std::string buildCommand = "\"" + msbuildPath + "\" \"" + m_ProjectFile +
		"\" /p:Configuration=Release /t:Rebuild /nologo /verbosity:minimal > \"" +
		testOutputFile + "\" 2> \"" + testErrorFile + "\"";

	int buildExitCode = ExecuteSilentProcess(buildCommand, m_ScriptFolder);

	std::ofstream resultStream(resultFile);
	if (resultStream.is_open()) {
		resultStream << buildExitCode;
		resultStream.close();
	}

	int buildResult = -1;
	try {
		std::ifstream resultFileStream(resultFile);
		if (resultFileStream.is_open()) {
			std::string line;
			if (std::getline(resultFileStream, line) && !line.empty()) {
				buildResult = std::stoi(line);
			}
			resultFileStream.close();
		}
	}
	catch (...) {}

	bool buildHasErrors = false;

	try {
		std::ifstream outputFile(testOutputFile);
		if (outputFile.is_open()) {
			std::string line;
			while (std::getline(outputFile, line)) {
				if (line.find("error") != std::string::npos) {
					buildHasErrors = true;
					break;
				}
			}
			outputFile.close();
		}
	}
	catch (...) {}

	try {
		std::ifstream errorFile(testErrorFile);
		if (errorFile.is_open() && errorFile.peek() != std::ifstream::traits_type::eof()) {
			buildHasErrors = true;
			errorFile.close();
		}
	}
	catch (...) {}

	TryDeleteFile(testOutputFile);
	TryDeleteFile(testErrorFile);
	TryDeleteFile(resultFile);

	if (buildResult != 0 || buildHasErrors) {
		LOG(LogType::LOG_INFO, "MSBuild failed to build the project, result code: %d", buildResult);
		return false;
	}

	return true;
}

void ScriptHotReloader::SaveWorkingMSBuildPath(const std::string& msbuildPath) {
	try {
		std::string configFile = m_ScriptFolder + "\\msbuild_config.txt";
		std::ofstream file(configFile);
		if (file.is_open()) {
			file << msbuildPath;
			file.close();
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Failed to save working MSBuild path configuration");
	}
}

std::string ScriptHotReloader::LoadWorkingMSBuildPath() {
	try {
		std::string configFile = m_ScriptFolder + "\\msbuild_config.txt";
		if (std::filesystem::exists(configFile)) {
			std::ifstream file(configFile);
			if (file.is_open()) {
				std::string savedPath;
				std::getline(file, savedPath);
				file.close();

				if (!savedPath.empty() && std::filesystem::exists(savedPath)) {
					return savedPath;
				}
			}
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Failed to load saved MSBuild path configuration");
	}

	return "";
}

bool ScriptHotReloader::CompileWithMSBuild() {
	if (m_ProjectFile.empty()) {
		LOG(LogType::LOG_ERROR, "File (.csproj) not found!");
		return false;
	}

	if (m_MSBuildPath.empty() || !std::filesystem::exists(m_MSBuildPath)) {
		LOG(LogType::LOG_ERROR, "Valid MSBuild installation not found. Please install Visual Studio.");
		return false;
	}

	LOG(LogType::LOG_INFO, "Compiling with MSBuild: %s", m_MSBuildPath.c_str());

	TryDeleteFile(m_ScriptFolder + "\\build_output.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_result.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_warnings.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_errors.txt");
	TryDeleteFile(m_OutputAssemblyDir + "\\Script.dll");
	TryDeleteFile(m_ScriptFolder + "\\bin\\Debug\\Script.dll");
	TryDeleteFile(m_ScriptFolder + "\\bin\\Release\\Script.dll");

	std::string outputFile = m_ScriptFolder + "\\build_output.txt";
	std::string buildCommand = "\"" + m_MSBuildPath + "\" \"" + m_ProjectFile +
		"\" /p:Configuration=Release /t:Rebuild /nologo /verbosity:minimal > \"" +
		outputFile + "\" 2>&1";

	int exitCode = ExecuteSilentProcess(buildCommand, m_ScriptFolder);

	std::ofstream resultFile(m_ScriptFolder + "\\build_result.txt");
	if (resultFile.is_open()) {
		resultFile << "Código de salida: " << exitCode;
		resultFile.close();
	}

	ExecuteSilentProcess("findstr /C:\"warning\" \"" + outputFile + "\" > \"" +
		m_ScriptFolder + "\\build_warnings.txt\" 2>nul", m_ScriptFolder);
	ExecuteSilentProcess("findstr /C:\"error\" \"" + outputFile + "\" > \"" +
		m_ScriptFolder + "\\build_errors.txt\" 2>nul", m_ScriptFolder);

	int buildResult = -1;
	try {
		std::ifstream resultFileStream(m_ScriptFolder + "\\build_result.txt");
		if (resultFileStream.is_open()) {
			std::string line;
			if (std::getline(resultFileStream, line)) {
				size_t pos = line.find_last_of(":");
				if (pos != std::string::npos && pos + 1 < line.length()) {
					std::string codeStr = line.substr(pos + 1);
					codeStr.erase(0, codeStr.find_first_not_of(" \t"));
					buildResult = std::stoi(codeStr);
				}
			}
			resultFileStream.close();
		}
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Error reading MSBuild compilation results");
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
		LOG(LogType::LOG_ERROR, "Error checking for MSBuild compilation errors");
	}

	if (buildResult != 0 || hasErrors) {
		LOG(LogType::LOG_ERROR, "Error compiling the project with MSBuild. Code: %d. Fix it or try dotnet.", buildResult);
		return false;
	}

	std::string assemblyPath = FindGeneratedAssembly();
	if (assemblyPath.empty()) {
		LOG(LogType::LOG_ERROR, "Assembly not found after MSBuild compilation");
		return false;
	}

	std::string versionedFilename = GenerateVersionedFilename("Script.dll");
	std::string stagedPath = m_StagingDirectory + "\\" + versionedFilename;

	CleanupOldVersions(0);

	try {
		std::filesystem::copy(assemblyPath, stagedPath, std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::filesystem::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Failed to create versioned assembly: %s", e.what());
		stagedPath = assemblyPath;
	}

	m_LastCompilationTime = std::filesystem::last_write_time(assemblyPath);
	RefreshScriptTimestamps();

	for (auto& callback : m_OnReloadCallbacks) {
		try {
			callback(stagedPath);
		}
		catch (const std::exception& e) {
			LOG(LogType::LOG_ERROR, "Reload callback error: %s", e.what());
		}
	}

	return true;
}

void ScriptHotReloader::SetPreferMSBuild(bool prefer) {
	m_PreferMSBuild = prefer;
	SaveBuildPreference(prefer);

	LOG(LogType::LOG_INFO, "Build preference set to: %s", prefer ? "MSBuild" : "dotnet");

	if (prefer) {
		if (m_MSBuildPath.empty() || !std::filesystem::exists(m_MSBuildPath)) {
			if (!FindWorkingMSBuild()) {
				LOG(LogType::LOG_WARNING, "Preferencia establecida a MSBuild, pero no se encontró una instalación. Se usará dotnet si está disponible.");
			}
		}
	}
	else {
		if (m_DotnetPath.empty() || !std::filesystem::exists(m_DotnetPath)) {
			if (!FindWorkingDotnet()) {
				LOG(LogType::LOG_WARNING, "Preferencia establecida a dotnet, pero no se encontró una instalación. Se usará MSBuild si está disponible.");
			}
		}
	}
}

bool ScriptHotReloader::GetPreferMSBuild() const {
	return m_PreferMSBuild;
}

void ScriptHotReloader::SaveBuildPreference(bool preferMSBuild) {
	try {
		std::string configFile = m_ScriptFolder + "\\build_preference.txt";
		std::ofstream file(configFile);
		if (file.is_open()) {
			file << (preferMSBuild ? "MSBuild" : "dotnet");
			file.close();
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Error al guardar la preferencia de compilación");
	}
}

bool ScriptHotReloader::LoadBuildPreference() {
	try {
		std::string configFile = m_ScriptFolder + "\\build_preference.txt";
		if (std::filesystem::exists(configFile)) {
			std::ifstream file(configFile);
			if (file.is_open()) {
				std::string preference;
				std::getline(file, preference);
				file.close();

				return preference == "MSBuild";
			}
		}
	}
	catch (...) {
		LOG(LogType::LOG_WARNING, "Error al cargar la preferencia de compilación");
	}

	return false;
}

int ScriptHotReloader::ExecuteSilentProcess(const std::string& command, const std::string& workingDir) {
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE hOutRead, hOutWrite;
	if (!CreatePipe(&hOutRead, &hOutWrite, &sa, 0)) {
		LOG(LogType::LOG_ERROR, "Failed to create output pipe");
		return -1;
	}
	SetHandleInformation(hOutRead, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hOutWrite;
	si.hStdError = hOutWrite;
	si.wShowWindow = SW_HIDE;

	std::string fullCmd;
	if (!workingDir.empty()) {
		fullCmd = "cmd.exe /c cd /d \"" + workingDir + "\" && " + command;
	}
	else {
		fullCmd = "cmd.exe /c " + command;
	}

	char* cmdCopy = _strdup(fullCmd.c_str());

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	BOOL result = CreateProcessA(
		NULL,
		cmdCopy,
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi
	);

	free(cmdCopy);
	CloseHandle(hOutWrite);

	if (!result) {
		DWORD error = GetLastError();
		LOG(LogType::LOG_ERROR, "Failed to create process: error code %d", error);
		CloseHandle(hOutRead);
		return -1;
	}

	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	std::string output;

	while (ReadFile(hOutRead, buffer, BUFFER_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		output += buffer;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hOutRead);

	if (exitCode != 0 && !output.empty()) {
		std::string logFile = m_ScriptFolder + "\\process_output.log";
		std::ofstream logStream(logFile);
		if (logStream.is_open()) {
			logStream << "Command: " << command << std::endl;
			logStream << "Exit code: " << exitCode << std::endl;
			logStream << "Output:" << std::endl << output;
			logStream.close();
		}
	}

	return exitCode;
}