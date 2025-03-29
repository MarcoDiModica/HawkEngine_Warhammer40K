#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <Windows.h>
#include <fstream>
#include "../MyGameEditor/Log.h"
#ifdef min
#undef min
#endif // 

#ifdef max
#undef max
#endif //

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

class ScriptHotReloader {
public:
	static ScriptHotReloader& GetInstance() {
		static ScriptHotReloader instance;
		return instance;
	}

	ScriptHotReloader(const ScriptHotReloader&) = delete;
	ScriptHotReloader& operator=(const ScriptHotReloader&) = delete;

	void Initialize(const std::string& scriptFolder, const std::string& outputAssembly) {
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		std::string fullPath(buffer);

		size_t lastSlash = fullPath.find_last_of("\\/");
		std::string exeDir = fullPath.substr(0, lastSlash);

		size_t x64Pos = exeDir.find_last_of("\\/");
		if (x64Pos != std::string::npos) {
			exeDir = exeDir.substr(0, x64Pos);
		}

		x64Pos = exeDir.find_last_of("\\/");
		if (x64Pos != std::string::npos) {
			exeDir = exeDir.substr(0, x64Pos);
		}

		std::string absoluteScriptFolder = exeDir + "\\Script";
		std::string absoluteOutputPath = exeDir + "\\Script\\obj\\Script.dll";

		m_ScriptFolder = absoluteScriptFolder;
		m_OutputAssembly = absoluteOutputPath;

		std::filesystem::path outputDir = std::filesystem::path(m_OutputAssembly).parent_path();
		if (!std::filesystem::exists(outputDir)) {
			try {
				std::filesystem::create_directories(outputDir);
			}
			catch (...) {
			}
		}

		m_LastCompilationTime = std::filesystem::file_time_type();

		RefreshScriptTimestamps();
	}

	void RegisterOnReloadCallback(std::function<void()> callback) {
		m_OnReloadCallbacks.push_back(callback);
	}

	bool CompileScripts() {
		// Usar el compilador que ya hemos encontrado
		std::string cscPath = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\Roslyn\\csc.exe";

		if (!std::filesystem::exists(cscPath)) {
			LOG(LogType::LOG_ERROR, "No se encontró el compilador específico.");
			return false;
		}

		LOG(LogType::LOG_INFO, "Usando compilador moderno CSC: %s", cscPath.c_str());

		// Verificar carpeta de scripts
		if (!std::filesystem::exists(m_ScriptFolder)) {
			LOG(LogType::LOG_ERROR, "Carpeta de scripts no encontrada: %s", m_ScriptFolder.c_str());
			return false;
		}

		// Asegurar que el directorio de salida existe
		std::filesystem::path outputDir = std::filesystem::path(m_OutputAssembly).parent_path();
		if (!std::filesystem::exists(outputDir)) {
			try {
				std::filesystem::create_directories(outputDir);
			}
			catch (...) {
				LOG(LogType::LOG_ERROR, "No se pudo crear el directorio para el assembly");
				return false;
			}
		}

		// Usar un archivo temporal para la compilación para evitar problemas de archivo bloqueado
		std::string tempAssembly = m_OutputAssembly.substr(0, m_OutputAssembly.length() - 4) + "_temp.dll";

		// Crear un archivo batch para la compilación
		std::string batchPath = m_ScriptFolder + "\\compile_csc.bat";
		std::ofstream batchFile(batchPath);
		if (!batchFile.is_open()) {
			LOG(LogType::LOG_ERROR, "No se pudo crear el archivo batch");
			return false;
		}

		batchFile << "@echo off" << std::endl;
		batchFile << "echo Compilando con CSC moderno..." << std::endl;

		// Crear comando para CSC con todas las referencias y archivos
		batchFile << "\"" << cscPath << "\" /target:library /out:\"" << tempAssembly << "\"";
		batchFile << " /langversion:latest"; // Usar la última versión del lenguaje

		// Opciones para ignorar warnings
		batchFile << " /nowarn:0626,0642,0649,0414,0169"; // Añadido CS0169 (campo nunca usado)
		batchFile << " /warnaserror-"; // No tratar warnings como errores

		// Añadir referencias básicas 
		batchFile << " /reference:System.dll";
		batchFile << " /reference:System.Core.dll";

		// Añadir System.Numerics específicamente para Vector3 y Quaternion
		batchFile << " /reference:System.Numerics.dll";

		// Otras referencias que podrían necesitarse
		batchFile << " /reference:System.Drawing.dll";

		// Buscar el Framework path para asegurar que se encuentran las referencias
		std::string frameworkPath;
		const char* windir = getenv("WINDIR");
		if (windir) {
			frameworkPath = std::string(windir) + "\\Microsoft.NET\\Framework64\\v4.0.30319";
			if (!std::filesystem::exists(frameworkPath)) {
				frameworkPath = std::string(windir) + "\\Microsoft.NET\\Framework\\v4.0.30319";
			}
		}

		if (!frameworkPath.empty() && std::filesystem::exists(frameworkPath)) {
			batchFile << " /lib:\"" << frameworkPath << "\"";
		}

		// Añadir todos los archivos .cs
		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				batchFile << " \"" << entry.path().string() << "\"";
			}
		}

		batchFile << std::endl;

		// Añadir código para copiar el assembly a la ubicación final si la compilación fue exitosa
		batchFile << "if %ERRORLEVEL% EQU 0 (" << std::endl;
		batchFile << "  if exist \"" << tempAssembly << "\" (" << std::endl;

		// Intentar múltiples veces ya que el archivo podría estar en uso temporalmente
		batchFile << "    for /L %%i in (1,1,10) do (" << std::endl;
		batchFile << "      copy /Y \"" << tempAssembly << "\" \"" << m_OutputAssembly << "\" > nul 2>&1" << std::endl;
		batchFile << "      if not errorlevel 1 goto :succeed" << std::endl;
		batchFile << "      timeout /t 1 > nul" << std::endl;
		batchFile << "    )" << std::endl;
		batchFile << "    echo Error: No se pudo copiar el assembly temporal a la ubicación final." << std::endl;
		batchFile << "    goto :failed" << std::endl;
		batchFile << "  )" << std::endl;
		batchFile << ")" << std::endl;

		// Etiquetas de éxito y fallo
		batchFile << ":succeed" << std::endl;
		batchFile << "echo Assembly copiado exitosamente." << std::endl;
		batchFile << "del \"" << tempAssembly << "\" > nul 2>&1" << std::endl;
		batchFile << "goto :end" << std::endl;

		batchFile << ":failed" << std::endl;
		batchFile << "echo Compilación fallida o no se pudo copiar el assembly." << std::endl;

		batchFile << ":end" << std::endl;
		batchFile << "echo Código de salida: %ERRORLEVEL% > compile_result.txt" << std::endl;
		batchFile.close();

		// Ejecutar el batch
		LOG(LogType::LOG_INFO, "Ejecutando script de compilación...");
		std::string command = "cmd /c \"" + batchPath + "\" > csc_output.txt 2>&1";
		int result = system(command.c_str());

		// Leer el resultado de compilación
		int compileResult = -1;
		try {
			std::ifstream resultFile(m_ScriptFolder + "\\compile_result.txt");
			if (resultFile.is_open()) {
				std::string line;
				if (std::getline(resultFile, line)) {
					size_t pos = line.find_last_of(":");
					if (pos != std::string::npos && pos + 1 < line.length()) {
						std::string codeStr = line.substr(pos + 1);
						codeStr.erase(0, codeStr.find_first_not_of(" \t"));
						try {
							compileResult = std::stoi(codeStr);
						}
						catch (...) {
							compileResult = -1;
						}
					}
				}
				resultFile.close();
			}
		}
		catch (...) {
			LOG(LogType::LOG_ERROR, "No se pudo leer el resultado de la compilación");
		}

		// Leer y mostrar la salida del compilador
		std::string compilerOutput;
		try {
			std::ifstream outputFile("csc_output.txt");
			if (outputFile.is_open()) {
				std::stringstream buffer;
				buffer << outputFile.rdbuf();
				compilerOutput = buffer.str();
				outputFile.close();
			}
		}
		catch (...) {
			compilerOutput = "No se pudo leer la salida del compilador";
		}

		if (!compilerOutput.empty()) {
			LOG(LogType::LOG_INFO, "Salida del compilador CSC: %s", compilerOutput.c_str());
		}

		if (compileResult != 0) {
			LOG(LogType::LOG_ERROR, "Error al compilar scripts con CSC. Código: %d", compileResult);
			return false;
		}

		// Verificar que el assembly se generó correctamente
		if (!std::filesystem::exists(m_OutputAssembly)) {
			LOG(LogType::LOG_ERROR, "No se generó el assembly con CSC");
			return false;
		}

		// Actualizar timestamps y ejecutar callbacks
		m_LastCompilationTime = std::filesystem::last_write_time(m_OutputAssembly);
		RefreshScriptTimestamps();

		for (auto& callback : m_OnReloadCallbacks) {
			try {
				callback();
			}
			catch (...) {
				LOG(LogType::LOG_ERROR, "Error en callback de recarga");
			}
		}

		LOG(LogType::LOG_INFO, "Scripts compilados correctamente con CSC");
		return true;
	}

	bool CheckForChanges() {
		bool changes = false;

		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				auto lastWriteTime = std::filesystem::last_write_time(entry.path());

				auto it = m_ScriptTimestamps.find(entry.path().string());
				if (it == m_ScriptTimestamps.end() || it->second < lastWriteTime) {
					changes = true;
					break;
				}
			}
		}

		return changes;
	}

	void RefreshScriptTimestamps() {
		m_ScriptTimestamps.clear();

		for (const auto& entry : std::filesystem::directory_iterator(m_ScriptFolder)) {
			if (entry.path().extension() == ".cs") {
				m_ScriptTimestamps[entry.path().string()] = std::filesystem::last_write_time(entry.path());
			}
		}
	}

	bool onetime = true;

	void Update() {
		/*if (CheckForChanges()) {
			CompileScripts();
		}*/

		if (onetime) {
			CompileScripts();
			onetime = false;
		}
	}

private:
	ScriptHotReloader() {}
	~ScriptHotReloader() {}

	std::string GetMonoPath() {
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		std::string fullPath(buffer);

		size_t lastSlash = fullPath.find_last_of("\\/");
		std::string exeDir = fullPath.substr(0, lastSlash);

		size_t x64Pos = exeDir.find_last_of("\\/");
		if (x64Pos != std::string::npos) {
			exeDir = exeDir.substr(0, x64Pos);  
		}

		x64Pos = exeDir.find_last_of("\\/");
		if (x64Pos != std::string::npos) {
			exeDir = exeDir.substr(0, x64Pos);  
		}

		return exeDir + "\\External\\Mono";
	}

	std::string GetMcsPath() {
		std::string monoPath = GetMonoPath();
		std::string mcsPath = monoPath + "\\lib\\mono\\4.5\\mcs.exe";

		if (!std::filesystem::exists(mcsPath)) {
			mcsPath = monoPath + "\\bin\\mcs.exe";

			if (!std::filesystem::exists(mcsPath)) {
				mcsPath = monoPath + "\\lib\\mono\\2.0\\mcs.exe";
			}
		}

		return mcsPath;
	}

	std::string m_ScriptFolder;
	std::string m_OutputAssembly;
	std::unordered_map<std::string, std::filesystem::file_time_type> m_ScriptTimestamps;
	std::filesystem::file_time_type m_LastCompilationTime;
	std::vector<std::function<void()>> m_OnReloadCallbacks;
};