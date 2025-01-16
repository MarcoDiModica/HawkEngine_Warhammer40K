#include "MonoEnvironment.h"
#include "SharpBinder.h"
#include <mono/metadata/assembly.h>
#include <Windows.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>

#include "../MyGameEditor/Log.h"// ilegal

using namespace SharpBinder;

std::string getExecutablePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string fullPath(buffer);
	size_t lastSlash = fullPath.find_last_of("\\/");
	return fullPath.substr(0, lastSlash);
}

void HandleConsoleOutput(MonoString* message) /*C# strings are parse by mnono as MonoString */
{
	if (message == nullptr)
		return;

	char* msg = mono_string_to_utf8(message);
	LOG(LogType::LOG_C_SHARP, msg);
}

MonoEnvironment::MonoEnvironment() {
	/* Attemot to set up C# debugging, doesn't work */
	std::string option = "--debugger-agent=transport=dt_socket,address=127.0.0.1:55555,server=y,suspend=n";
	char* option_cstr = const_cast<char*>(option.c_str()); /* Convert std::string to C - style string */
	char* options[] = { option_cstr };
	mono_jit_parse_options(1, options);
	//mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	
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
	for (int i = rows - 1; i > 0; --i)
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
	
	
	LinkEngineMethods();
	
	/* Test by creating a TestObject and callinf its Start method*/
	
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
	
	
	CreateGO();
}


void MonoEnvironment::LinkEngineMethods() {

	//TODO map cpp methods accesible by the user to C#
	   //                (C#  namespace   class     method)  ( void* to C++ method )
	mono_add_internal_call("HawkEngine.EngineCalls::print", (const void*)HandleConsoleOutput);
	mono_add_internal_call("HawkEngine.EngineCalls::CreateGameObject", (const void*) CreateGameObjectSharp );
	mono_add_internal_call("HawkEngine.GameObject::GetName", (const void*)GameObjectGetName);
	mono_add_internal_call("HawkEngine.EngineCalls::Destroy", (const void*)Destroy);
	mono_add_internal_call("HawkEngine.GameObject::AddChild", (const void*)GameObjectAddChild);
	mono_add_internal_call("HawkEngine.Input::GetKey", (const void*)GetKey);
	mono_add_internal_call("HawkEngine.Input::GetKeyDown", (const void*)GetKeyDown);
	mono_add_internal_call("HawkEngine.Input::GetKeyUp", (const void*)GetKeyUp);
	mono_add_internal_call("HawkEngine.Input::GetMouseButton", (const void*)GetMouseButton);
	mono_add_internal_call("HawkEngine.Input::GetMouseButtonDown", (const void*)GetMouseButtonDown);
	mono_add_internal_call("HawkEngine.Input::GetMouseButtonUp", (const void*)GetMouseButtonUp);
	mono_add_internal_call("HawkEngine.Input::GetAxis", (const void*)GetAxis);
}


void MonoEnvironment::CreateGO() {

	MonoClass* klass = mono_class_from_name(m_ptr_GameAssemblyImg, "HawkEngine", "EngineCalls");
	MonoMethod* method = mono_class_get_method_from_name(klass, "CreateGameObject", 1);

	MonoString* arg = mono_string_new(mono_domain_get(), "Samson");
	void* args[1] = { arg };

	mono_runtime_invoke(method, nullptr, args, nullptr);

}

void MonoEnvironment::DestroyGo() {
	MonoClass* klass = mono_class_from_name(m_ptr_GameAssemblyImg, "HawkEngine", "EngineCalls");
	MonoMethod* method = mono_class_get_method_from_name(klass, "Destroy", 1);

	MonoString* arg = mono_string_new(mono_domain_get(), "Samson");
	void* args[1] = { arg };

	mono_runtime_invoke(method, nullptr, args, nullptr);
}

std::string generateScriptContent(const std::string& className, const std::string& baseClassName) {
	std::string content =
		"using System;\n"
		"using System.Collections.Generic;\n"
		"using System.Linq;\n"
		"using System.Text;\n"
		"using System.Threading.Tasks;\n\n"
		"public class " + className + " : " + baseClassName + "\n"
		"{\n"
		"    public override void Start()\n"
		"    {\n"
		"        HawkEngine.EngineCalls.print(\"Hola desde \" + this.GetType().Name);\n"
		"    }\n\n"
		"    public override void Update(float deltaTime)\n"
		"    {\n"
		"        // Lógica de actualización\n"
		"    }\n"
		"}\n";
	return content;
}

bool createCSharpScriptFile(const std::string& className, const std::string& baseClassName, const std::string& filePath) 
{
	std::string scriptContent = generateScriptContent(className, baseClassName);

	std::ofstream scriptFile(filePath);

	if (!scriptFile.is_open()) {
		std::cerr << "Error al crear el archivo: " << filePath << std::endl;
		return false;
	}

	scriptFile << scriptContent;

	scriptFile.close();

	return true;
}