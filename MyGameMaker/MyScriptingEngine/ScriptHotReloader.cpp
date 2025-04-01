#include "ScriptHotReloader.h"
#include <sstream>
#include <thread>
#include <unordered_set>
#include <algorithm>
#include <iomanip>

ScriptHotReloader& ScriptHotReloader::GetInstance() {
	static ScriptHotReloader instance;
	return instance;
}

ScriptHotReloader::ScriptHotReloader() : m_IsCompiling(false), m_CompilationCooldown(false), m_LastCompilationSuccess(true) {
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

	if (m_DotnetPath.empty() || !std::filesystem::exists(m_DotnetPath)) {
		if (!FindWorkingDotnet()) {
			LOG(LogType::LOG_ERROR, "Failed to find a working dotnet installation. Please install .NET SDK or contact Hawk Developers.");
			return;
		}
	}

	m_IsCompiling = true;
	bool initialCompilationResult = CompileExistingProject();
	m_IsCompiling = false;
	m_LastCompilationSuccess = initialCompilationResult;

	if (!initialCompilationResult) {
		LOG(LogType::LOG_ERROR, "Initial compilation failed. Please solve the errors or ask Hawk Developers :)");
	}
}

void ScriptHotReloader::RegisterOnReloadCallback(ReloadCallbackType callback) {
	m_OnReloadCallbacks.push_back(callback);
}

bool ScriptHotReloader::CheckForChanges() {
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
	if (m_IsCompiling || m_CompilationCooldown) {
		LOG(LogType::LOG_INFO, "Compilation already in progress or cooling down. Please wait.");
		return false;
	}

	m_IsCompiling = true;

	bool result = CompileExistingProject();

	m_CompilationCooldown = true;
	m_LastCompilationSuccess = result;

	std::thread([this]() {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		m_CompilationCooldown = false;
		m_IsCompiling = false;
		}).detach();

	if (!result) {
		LOG(LogType::LOG_ERROR, "Forced recompilation failed.");
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
			LOG(LogType::LOG_INFO, "Found working standard dotnet: %s", dotnetPath.c_str());
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

	LOG(LogType::LOG_ERROR, "No working dotnet installation found. Please install .NET SDK or contact Hawk Developers.");
	return false;
}

bool ScriptHotReloader::TestDotnetCompilation(const std::string& dotnetPath) {
	if (m_ProjectFile.empty()) {
		LOG(LogType::LOG_ERROR, "No .csproj file found for testing dotnet!");
		return false;
	}

	std::string versionFile = m_ScriptFolder + "\\dotnet_version.txt";
	std::string testVersionBatch = m_ScriptFolder + "\\test_dotnet_version.bat";

	{
		std::ofstream batch(testVersionBatch);
		if (!batch.is_open()) {
			LOG(LogType::LOG_ERROR, "Couldn't create version test batch file");
			return false;
		}

		batch << "@echo off" << std::endl;
		batch << "\"" << dotnetPath << "\" --version > \"" << versionFile << "\" 2>&1" << std::endl;
		batch.close();

		std::string command = "cmd /c \"" + testVersionBatch + "\"";
		int versionResult = system(command.c_str());

		try {
			std::filesystem::remove(testVersionBatch);
		}
		catch (...) {}

		if (versionResult != 0) {
			LOG(LogType::LOG_INFO, "Dotnet failed to report version, not a valid dotnet installation");
			try {
				std::filesystem::remove(versionFile);
			}
			catch (...) {}
			return false;
		}

		bool validVersionFound = false;
		try {
			std::ifstream versionOutput(versionFile);
			if (versionOutput.is_open()) {
				std::string versionLine;
				if (std::getline(versionOutput, versionLine) && !versionLine.empty()) {
					validVersionFound = true;
				}
				versionOutput.close();
			}
		}
		catch (...) {}

		try {
			std::filesystem::remove(versionFile);
		}
		catch (...) {}

		if (!validVersionFound) {
			LOG(LogType::LOG_INFO, "Dotnet did not return a valid version string");
			return false;
		}
	}

	std::string testBuildBatch = m_ScriptFolder + "\\test_dotnet_build.bat";
	std::string testOutputFile = m_ScriptFolder + "\\test_dotnet_output.txt";
	std::string testErrorFile = m_ScriptFolder + "\\test_dotnet_error.txt";

	{
		std::ofstream batch(testBuildBatch);
		if (!batch.is_open()) {
			LOG(LogType::LOG_ERROR, "Couldn't create build test batch file");
			return false;
		}

		batch << "@echo off" << std::endl;
		batch << "cd /d \"" << m_ScriptFolder << "\"" << std::endl;
		batch << "\"" << dotnetPath << "\" build \"" << m_ProjectFile << "\" -c Release --no-incremental --nologo > \"" << testOutputFile << "\" 2> \"" << testErrorFile << "\"" << std::endl;
		batch << "echo %ERRORLEVEL% > \"" << m_ScriptFolder << "\\test_dotnet_result.txt\"" << std::endl;
		batch.close();

		std::string command = "cmd /c \"" + testBuildBatch + "\"";
		system(command.c_str());

		int buildResult = -1;
		try {
			std::ifstream resultFile(m_ScriptFolder + "\\test_dotnet_result.txt");
			if (resultFile.is_open()) {
				std::string line;
				if (std::getline(resultFile, line) && !line.empty()) {
					buildResult = std::stoi(line);
				}
				resultFile.close();
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

		try {
			std::filesystem::remove(testBuildBatch);
			std::filesystem::remove(testOutputFile);
			std::filesystem::remove(testErrorFile);
			std::filesystem::remove(m_ScriptFolder + "\\test_dotnet_result.txt");
		}
		catch (...) {}

		if (buildResult != 0 || buildHasErrors) {
			LOG(LogType::LOG_INFO, "Dotnet failed to build the project, result code: %d", buildResult);
			return false;
		}
	}

	return true;
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
		LOG(LogType::LOG_ERROR, "Valid dotnet installation not found. Please install .NET SDK or contact Hawk Developers.");
		return false;
	}

	TryDeleteFile(m_ScriptFolder + "\\build_output.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_result.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_warnings.txt");
	TryDeleteFile(m_ScriptFolder + "\\build_errors.txt");

	std::string batchFile = m_ScriptFolder + "\\build.bat";
	std::ofstream batch(batchFile);
	if (!batch.is_open()) {
		LOG(LogType::LOG_ERROR, "Couldn't create batch file");
		return false;
	}

	batch << "@echo off" << std::endl;
	batch << "echo Compiling project please raise your hands" << std::endl;
	batch << "cd /d \"" << m_ScriptFolder << "\"" << std::endl;
	batch << "\"" << m_DotnetPath << "\" build \"" << m_ProjectFile << "\" -c Release > build_output.txt 2>&1" << std::endl;
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

	std::string versionedFilename = GenerateVersionedFilename("Script.dll");
	std::string stagedPath = m_StagingDirectory + "\\" + versionedFilename;

	try {
		std::filesystem::copy(assemblyPath, stagedPath, std::filesystem::copy_options::overwrite_existing);

		CleanupOldVersions(5);
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
	auto now = std::chrono::system_clock::now();
	auto time_t_now = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << "Script_";

	std::tm tm_now;
	localtime_s(&tm_now, &time_t_now);
	ss << std::put_time(&tm_now, "%Y%m%d_%H%M%S");

	std::string extension = ".dll";
	size_t dotPos = baseFilename.find_last_of('.');
	if (dotPos != std::string::npos) {
		extension = baseFilename.substr(dotPos);
	}

	ss << extension;
	return ss.str();
}

void ScriptHotReloader::CleanupOldVersions(int keepCount) {
	try {
		std::vector<std::filesystem::directory_entry> versionedFiles;

		for (const auto& entry : std::filesystem::directory_iterator(m_StagingDirectory)) {
			if (entry.is_regular_file() &&
				entry.path().filename().string().find("Script_") == 0) {
				versionedFiles.push_back(entry);
			}
		}

		std::sort(versionedFiles.begin(), versionedFiles.end(),
			[](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
				return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
			});

		if (versionedFiles.size() > keepCount) {
			for (size_t i = keepCount; i < versionedFiles.size(); ++i) {
				std::filesystem::remove(versionedFiles[i]);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		LOG(LogType::LOG_ERROR, "Error cleaning up old assemblies: %s", e.what());
	}
}