#include "ScriptComponent.h"
#include "MonoManager.h"
#include <fstream>
#include <filesystem>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>
#include "../MyGameEditor/Log.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner) {}

ScriptComponent::~ScriptComponent() {}

void ScriptComponent::Start() {
    if (monoScript) {
        MonoClass* scriptClass = mono_object_get_class(monoScript);
        MonoMethod* startMethod = mono_class_get_method_from_name(scriptClass, "Start", 0);

        MonoObject* exception = nullptr;
        mono_runtime_invoke(startMethod, monoScript, nullptr, &exception);

        if (exception) {
            MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
            const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
            LOG(LogType::LOG_ERROR, "StartError: %s", exceptionStr);
            mono_free((void*)exceptionStr);
        }
    }
}

void ScriptComponent::Update(float deltaTime) {
    if (monoScript) {
        MonoClass* scriptClass = mono_object_get_class(monoScript);
        MonoMethod* updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 1);

        void* args[1];
        args[0] = &deltaTime;

        MonoObject* exception = nullptr;
        mono_runtime_invoke(updateMethod, monoScript, args, &exception);

        if (exception) {
            MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
            const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
            LOG(LogType::LOG_ERROR, "UpdateError: %s", exceptionStr);
            mono_free((void*)exceptionStr);
        }
    }
}

static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* klass = mono_class_from_name(image, namespaceName, className);

    if (klass == nullptr)
    {
        // Log error here
        return nullptr;
    }

    return klass;
}

bool ScriptComponent::LoadScript(const std::string& scriptName) 
{
    MonoManager& monoManager = MonoManager::GetInstance();
	MonoAssembly* assembly = monoManager.GetAssembly();
	MonoClass* scriptClass = GetClassInAssembly(assembly, "", scriptName.c_str());

	if (scriptClass == nullptr) {
		LOG(LogType::LOG_ERROR, "No se ha encontrado la clase %s en el ensamblado.", scriptName.c_str());
		return false;
	}

	MonoObject* scriptObject = mono_object_new(monoManager.GetDomain(), scriptClass);
	if (scriptObject == nullptr) {
        LOG(LogType::LOG_ERROR, "No se ha podido crear la instancia de la clase %s.", scriptName.c_str());
        return false;
    }
    mono_runtime_object_init(scriptObject);

	monoScript = scriptObject;

	return true;
}

bool ScriptComponent::CreateNewScript(const std::string& scriptName, const std::string& baseScriptName)
{
    std::string scriptContent = "using System;\n\n"
        "public class " + scriptName + " : " + baseScriptName + "\n"
        "{\n"
        "    public override void Start()\n"
        "    {\n"
        "        HawkEngine.EngineCalls.print(\"Hola desde " + scriptName + "!\");\n"
        "    }\n\n"
        "    public override void Update(float deltaTime)\n"
        "    {\n"
        "        // L�gica de actualizaci�n\n"
        "    }\n"
        "}\n";

    std::string scriptPath = "../Script/" + scriptName + ".cs";

    std::ofstream scriptFile(scriptPath);
    if (!scriptFile.is_open()) {
        LOG(LogType::LOG_ERROR, "Error al crear el archivo: %s", scriptPath.c_str());
        return false;
    }

    scriptFile << scriptContent;
    scriptFile.close();

    LOG(LogType::LOG_INFO, "Script %s creado correctamente en %s.", scriptName.c_str(), scriptPath.c_str());
    return true;
}

std::string ScriptComponent::GetTypeName() const
{
	if (monoScript) {
		MonoClass* scriptClass = mono_object_get_class(monoScript);
		const char* name = mono_class_get_name(scriptClass);
		return std::string(name);
	}

	return "";
}