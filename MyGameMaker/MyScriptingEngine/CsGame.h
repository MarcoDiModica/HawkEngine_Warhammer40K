#pragma once
#ifndef __MONOENVIRONMENT_H__
#define __MONOENVIRONMENT_H__

#include <mono/metadata/assembly.h>
#include <Windows.h>
#include "../External/Mono/include/mono-2.0/mono/metadata/assembly.h"
//#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>
#include <filesystem>
#include <string>

std::string getExecutablePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string fullPath(buffer);
	size_t lastSlash = fullPath.find_last_of("\\/");
	return fullPath.substr(0, lastSlash);
}
class MonoEnvironment
{
public:
	MonoEnvironment() {



		std::string path = getExecutablePath() + "\\..\\..\\External\\Mono";
		mono_set_dirs(std::string(path + "\\lib").c_str(),
			std::string(path + "\\etc").c_str());

		m_ptr_MonoDomain = mono_jit_init("C#App");


		//auto s = std::filesystem::current_path();
		//int y = 0;
		//std::string path = "C:\\Users\\marco\\OneDrive\\Documentos\\GitHub\\HawkEngine\\MyGameMaker\\x64\\Debug\\Mono\\4.5";
		//mono_set_assemblies_path()
		//mono_set_dirs(path.c_str(), ".");

		//m_ptr_MonoDomain = mono_jit_init("csGame");
	
		if (m_ptr_MonoDomain) {
			int y = 0;
		}
	}


private:


	MonoDomain* m_ptr_MonoDomain;
	MonoAssembly* m_ptr_GameAssembly;
	MonoImage* m_ptr_GameAssemblyImg;

};

class Pu {
public:
	Pu();

	int pu = 0;
};

#endif