#include "MonoEnvironment.h"
//#include <mono/metadata/assembly.h>
//#include <Windows.h>
//#include <mono/metadata/debug-helpers.h>
//#include <mono/jit/jit.h>
//#include <filesystem>
//#include <string>

//
//std::string getExecutablePath() {
//	//char buffer[MAX_PATH];
//	//GetModuleFileNameA(NULL, buffer, MAX_PATH);
//	//std::string fullPath(buffer);
//	//size_t lastSlash = fullPath.find_last_of("\\/");
//	//return fullPath.substr(0, lastSlash);
//	return std::string("DDD");
//}
//
//void HandleConsoleOutput(MonoString* message) /*C# strings are parse by mnono as MonoString */
//{
//	//if (message == nullptr)
//	//	return;
//
//	//char* msg = mono_string_to_utf8(message);
//	//LOG(LogType::LOG_C_SHARP, msg);
//}

MonoEnvironment::MonoEnvironment() = default;


	/* Attemot to set up C# debugging, doesn't work */
	//std::string option = "--debugger-agent=transport=dt_socket,address=127.0.0.1:55555,server=y,suspend=n";
	//char* option_cstr = const_cast<char*>(option.c_str()); /* Convert std::string to C - style string */
	//char* options[] = { option_cstr };
	//mono_jit_parse_options(1, options);
	////mono_debug_init(MONO_DEBUG_FORMAT_MONO);

	//std::string path = getExecutablePath() + "\\..\\..\\External\\Mono";
	//mono_set_dirs(std::string(path + "\\lib").c_str(),
	//	std::string(path + "\\etc").c_str());

	//m_ptr_MonoDomain = mono_jit_init("C#App");


	//if (m_ptr_MonoDomain) {
	//	std::string p = std::string(getExecutablePath() + "\\..\\..\\Script\\obj\\Debug\\Script.dll").c_str();
	//	m_ptr_GameAssembly = mono_domain_assembly_open(m_ptr_MonoDomain, p.c_str());
	//}

	//m_ptr_GameAssemblyImg = mono_assembly_get_image(m_ptr_GameAssembly);

	//const MonoTableInfo* table_info = mono_image_get_table_info(m_ptr_GameAssemblyImg, MONO_TABLE_TYPEDEF);
	//int rows = mono_table_info_get_rows(table_info);

	//MonoClass* klass = nullptr;

	//user_classes.clear();
	//for (int i = rows - 1; i > 0; --i)
	//{
	//	uint32_t cols[MONO_TYPEDEF_SIZE];
	//	mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
	//	const char* name = mono_metadata_string_heap(m_ptr_GameAssemblyImg, cols[MONO_TYPEDEF_NAME]);
	//	if (name[0] != '<')
	//	{
	//		const char* name_space = mono_metadata_string_heap(m_ptr_GameAssemblyImg, cols[MONO_TYPEDEF_NAMESPACE]);
	//		klass = mono_class_from_name(m_ptr_GameAssemblyImg, name_space, name);

	//		std::string s = mono_class_get_namespace(klass);

	//		// Classes with the Script namespace aren't processed
	//		if (klass != nullptr && strcmp(mono_class_get_namespace(klass), "Script") != 0)
	//		{
	//			if (!mono_class_is_enum(klass)) {
	//				user_classes.push_back(klass);
	//			}
	//			//LOG("%s", mono_class_get_name(_class));
	//		}
	//	}
	//}


	//LinkEngineMethods();

	///* Test by creating a TestObject and callinf its Start method*/

	//MonoMethod* method = mono_class_get_method_from_name(user_classes[user_classes.size() - 1], "Start", 0);
	//MonoObject* instance = mono_object_new(m_ptr_MonoDomain, user_classes[user_classes.size() - 1]);
	//if (method)
	//{
	//	// Call the Start method
	//	mono_runtime_invoke(method, instance, nullptr, nullptr);
	//}
	//else
	//{
	//	LOG(LogType::LOG_INFO, "Start method not found!");
	//}
	///* Test if the value has changed */
	//MonoClassField* field = mono_class_get_field_from_name(user_classes[user_classes.size() - 1], "testValue");
	//int value = 0;
	//mono_field_static_get_value(mono_class_vtable(m_ptr_MonoDomain, user_classes[user_classes.size() - 1]), field, &value);
	//LOG(LogType::LOG_INFO, "testValue: %d", value);


	////CreateGO();





void MonoEnvironment::LinkEngineMethods() {

	//TODO map cpp methods accesible by the user to C#

	//                   (C#  namespace   class     method)  ( void* to C++ method )
	//mono_add_internal_call("HawkEngine.EngineCalls::print", (const void*)HandleConsoleOutput);
	//const void* method = Root::CreateGameObject;
	//mono_add_internal_call("HawkEngine.EngineCalls::CreateGameObject", (const void*) CreateGameObjectSharp );

}

MonoEnvironment::~MonoEnvironment() 
{
	/*if (m_ptr_MonoDomain) {
		LOG(LogType::LOG_C_SHARP, "Destroying C# domain");
		mono_jit_cleanup(m_ptr_MonoDomain);
	}*/
}