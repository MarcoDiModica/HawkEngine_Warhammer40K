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

bool ScriptComponent::LoadScript(const std::string& scriptName) {
    MonoDomain* currentDomain = mono_domain_get();
    MonoDomain* appDomain = MonoManager::GetInstance().GetDomain();

    LOG(LogType::LOG_INFO, "Current Domain: %p", currentDomain);
    LOG(LogType::LOG_INFO, "App Domain: %p", appDomain);

    if (currentDomain != appDomain) {
        LOG(LogType::LOG_ERROR, "Domain mismatch - attempting to set correct domain");
        mono_domain_set(appDomain, false);
        currentDomain = mono_domain_get();
        LOG(LogType::LOG_INFO, "New Current Domain: %p", currentDomain);
    }

    MonoAssembly* assembly = MonoManager::GetInstance().GetAssembly();
    MonoImage* image = mono_assembly_get_image(assembly);

    LOG(LogType::LOG_INFO, "Assembly: %p", assembly);
    LOG(LogType::LOG_INFO, "Image: %p", image);

    MonoClass* scriptClass = mono_class_from_name(image, "", scriptName.c_str());
    LOG(LogType::LOG_INFO, "Script Class: %p", scriptClass);

    if (!scriptClass) {
        LOG(LogType::LOG_ERROR, "Script class not found");
        return false;
    }

    LOG(LogType::LOG_INFO, "Class name: %s", mono_class_get_name(scriptClass));
    LOG(LogType::LOG_INFO, "Class namespace: %s", mono_class_get_namespace(scriptClass));
    LOG(LogType::LOG_INFO, "Parent class: %s", mono_class_get_name(mono_class_get_parent(scriptClass)));

    if (!mono_class_init(scriptClass)) {
        LOG(LogType::LOG_ERROR, "Failed to initialize class");
        return false;
    }

    LOG(LogType::LOG_INFO, "Attempting to create instance...");
    monoScript = mono_object_new(appDomain, scriptClass);
    LOG(LogType::LOG_INFO, "Instance created: %p", monoScript);

    if (!monoScript) {
        LOG(LogType::LOG_ERROR, "Failed to create instance");
        return false;
    }

    MonoObject* exception = nullptr;
    mono_runtime_invoke(mono_class_get_method_from_name(scriptClass, ".ctor", 0), monoScript, nullptr, &exception);

    if (exception) {
        MonoString* msg = mono_object_to_string(exception, nullptr);
        const char* str = mono_string_to_utf8(msg);
        LOG(LogType::LOG_ERROR, "Constructor error: %s", str);
        mono_free((void*)str);
        return false;
    }

    uintptr_t goPtr = reinterpret_cast<uintptr_t>(owner);
    mono_field_set_value(monoScript, mono_class_get_field_from_name(scriptClass, "CplusplusInstance"), &goPtr);

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