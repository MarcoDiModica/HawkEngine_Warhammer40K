#pragma once
#ifndef __MONOENVIRONMENT_H__
#define __MONOENVIRONMENT_H__

#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>
#include <filesystem>
#include <string>

class MonoEnvironment
{
public:
	MonoEnvironment() {



		mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");
		//mono_config_parse(NULL);
		m_ptr_MonoDomain = mono_jit_init("myapp");


		//auto s = std::filesystem::current_path();
		//int y = 0;
		//std::string path = "C:\\Users\\marco\\OneDrive\\Documentos\\GitHub\\HawkEngine\\MyGameMaker\\x64\\Debug\\Mono\\4.5";
		//mono_set_assemblies_path()
		//mono_set_dirs(path.c_str(), ".");

		//m_ptr_MonoDomain = mono_jit_init("csGame");
	
		//if (m_ptr_MonoDomain) {
		//	int i = 0;
		//}
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