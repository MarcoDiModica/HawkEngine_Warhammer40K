#include "ScriptComponent.h"
#include "MonoManager.h"
#include <fstream>
#include <filesystem>
#include <mono/jit/jit.h>

#include "../MyGameEditor/Log.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner) {
    updateInStop = false;
}

ScriptComponent::~ScriptComponent() {}

void ScriptComponent::Awake()
{
    if (monoScript) {
        MonoClass* scriptClass = mono_object_get_class(monoScript);
        MonoMethod* awakeMethod = mono_class_get_method_from_name(scriptClass, "Awake", 0);
        MonoObject* exception = nullptr;
        mono_runtime_invoke(awakeMethod, monoScript, nullptr, &exception);
        if (exception) {
            MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
            const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
            LOG(LogType::LOG_ERROR, "AwakeError: %s", exceptionStr);
            mono_free((void*)exceptionStr);
        }
    }
}

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
		std::string scriptname = scriptName;
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

void ScriptComponent::Destroy()
{
	/*if (monoScript) {
		MonoClass* scriptClass = mono_object_get_class(monoScript);
		MonoMethod* destroyMethod = mono_class_get_method_from_name(scriptClass, "Destroy", 0);

		MonoObject* exception = nullptr;
		mono_runtime_invoke(destroyMethod, monoScript, nullptr, &exception);

		if (exception) {
			MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
			const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
			LOG(LogType::LOG_ERROR, "DestroyError: %s", exceptionStr);
			mono_free((void*)exceptionStr);
		}
	}*/

    monoScript = nullptr;
}

bool ScriptComponent::LoadScript(const std::string& scriptName)
{
    std::string scriptPath = "../Script/" + scriptName + ".cs";

	this->scriptName = scriptName;

    if (!std::filesystem::exists(scriptPath)) {
        LOG(LogType::LOG_ERROR, "script %s not found in route %s", scriptName.c_str(), scriptPath.c_str());
        return false;
    }

    MonoClass* scriptClass = mono_class_from_name(MonoManager::GetInstance().GetImage(), "", scriptName.c_str());
    if (!scriptClass) {
        LOG(LogType::LOG_ERROR, "cant find class %s in assembly.", scriptName.c_str());
        return false;
    }

    monoScript = mono_object_new(MonoManager::GetInstance().GetDomain(), scriptClass);
    if (!monoScript) {
        LOG(LogType::LOG_ERROR, "not found instance of script %s.", scriptName.c_str());
        return false;
    }

    MonoObject* exception = nullptr;
    mono_runtime_object_init(monoScript);

    if (exception) {
        MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
        const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
        LOG(LogType::LOG_ERROR, "Error init Script %s: %s", scriptName.c_str(), exceptionStr);
        mono_free((void*)exceptionStr);
        return false;
    }


/*   Setting the cplusplus gameObject reference of the MonoBehaviour scripto        */

    if (MonoManager::GetInstance().scriptIDs.contains(scriptName) == false) {

     LOG(LogType::LOG_INFO, "Script %s Loaded successfully.", scriptName.c_str());



        MonoManager::GetInstance().scriptIDs.emplace(std::pair<std::string, int>(scriptName, MonoManager::GetInstance().GetNewScriptClassID()));
    }

    uintptr_t goPtr = reinterpret_cast<uintptr_t>(owner);
    MonoClassField* field = mono_class_get_field_from_name(scriptClass, "CplusplusInstance");
    mono_field_set_value(monoScript, field, &goPtr);

    if (std::filesystem::exists(scriptPath)) {
        lastWriteTime = std::filesystem::last_write_time(scriptPath);
    }

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

MonoObject* GetMonoFromGameObject(GameObject* gameObject) {
    if (!gameObject) return nullptr;

    MonoClass* gameObjectClass = MonoManager::GetInstance().GetClass("HawkEngine", "GameObject");
    if (!gameObjectClass) {
        return nullptr;
    }

    MonoObject* monoGameObject = mono_object_new(mono_domain_get(), gameObjectClass);
    if (!monoGameObject) {
        return nullptr;
    }

    MonoClassField* nativePtrField = mono_class_get_field_from_name(gameObjectClass, "CplusplusInstance");
    if (!nativePtrField) {
        return nullptr;
    }

    uintptr_t nativePtr = reinterpret_cast<uintptr_t>(gameObject);
    mono_field_set_value(monoGameObject, nativePtrField, &nativePtr);

    return monoGameObject;
}



void ScriptComponent::InvokeMonoMethod(const std::string& methodName, GameObject& other) {
    if (!monoScript) return;

    MonoClass* klass = mono_object_get_class(monoScript);
    MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 1);

    if (!method) return;

    MonoObject* monoOther = GetMonoFromGameObject(&other);

    if (!monoOther) {
        return;
    }

    void* args[1];
    args[0] = monoOther; 
    mono_runtime_invoke(method, monoScript, args, nullptr);
}