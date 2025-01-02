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

#include "../MyGameEditor/Log.h" // ilegal


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
	MonoEnvironment()
	{
		std::string path = getExecutablePath() + "\\..\\..\\External\\Mono";
		mono_set_dirs(std::string(path + "\\lib").c_str(),
			std::string(path + "\\etc").c_str());

		m_ptr_MonoDomain = mono_jit_init("C#App");


		if (m_ptr_MonoDomain) {
			std::string p = std::string(getExecutablePath() + "\\..\\..\\Script\\obj\\Debug\\Script.dll").c_str();
			m_ptr_GameAssembly = mono_domain_assembly_open(m_ptr_MonoDomain, p.c_str());
		}

		m_ptr_GameAssemblyImg = mono_assembly_get_image(m_ptr_GameAssembly);

		const MonoTableInfo* table_info = mono_image_get_table_info(m_ptr_GameAssemblyImg, MONO_TABLE_TYPEDEF);
		int rows = mono_table_info_get_rows(table_info);

		MonoClass* klass = nullptr;

		user_classes.clear();
		for (int i = rows-1; i > 0; --i)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(m_ptr_GameAssemblyImg, cols[MONO_TYPEDEF_NAME]);
			if (name[0] != '<')
			{
				const char* name_space = mono_metadata_string_heap(m_ptr_GameAssemblyImg, cols[MONO_TYPEDEF_NAMESPACE]);
				klass = mono_class_from_name(m_ptr_GameAssemblyImg, name_space, name);

				std::string s = mono_class_get_namespace(klass);

				// Classes with the Script namespace aren't processed
				if (klass != nullptr && strcmp(mono_class_get_namespace(klass), "Script") != 0)
				{
					if (!mono_class_is_enum(klass)) {
						user_classes.push_back(klass);
					}
					//LOG("%s", mono_class_get_name(_class));
				}
			}
		}

		MonoMethod* method = mono_class_get_method_from_name(user_classes[0], "Start", 0);
		MonoObject* instance = mono_object_new(m_ptr_MonoDomain, user_classes[0]);
		if (method)
		{
			// Call the Start method
			mono_runtime_invoke(method, instance, nullptr, nullptr);
		}
		else
		{
			LOG( LogType::LOG_INFO ,"Start method not found!");
		}
		/* Test if the value has changed */
		MonoClassField* field = mono_class_get_field_from_name(user_classes[0], "testValue");
		int value = 0;
		mono_field_static_get_value(mono_class_vtable(m_ptr_MonoDomain, user_classes[0]), field, &value);
		LOG(LogType::LOG_INFO ,"testValue: %d", value);
		
	}

	~MonoEnvironment() {

		if (m_ptr_MonoDomain) {
			mono_jit_cleanup(m_ptr_MonoDomain);
		}

	};


private:


	MonoDomain* m_ptr_MonoDomain;
	MonoAssembly* m_ptr_GameAssembly;
	MonoImage* m_ptr_GameAssemblyImg;

	std::vector<MonoClass*> user_classes;

};

class Pu {
public:
	Pu();

	int pu = 0;
};

#endif