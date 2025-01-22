#include "ScriptComponent.h"
#include "MonoManager.h"
#include <fstream>
#include <filesystem>
#include <mono/jit/jit.h>

#include "../MyGameEditor/Log.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner) {}

ScriptComponent::~ScriptComponent() {}

void ScriptComponent::Start() {
    if (monoScript) {
        MonoClass* scriptClass = mono_object_get_class(monoScript);

        MonoMethod* startMethod = mono_class_get_method_from_name(scriptClass, "Start", 0);

        mono_runtime_invoke(startMethod, monoScript, nullptr, nullptr);
    }
}

void ScriptComponent::Update(float deltaTime) {
    if (monoScript) {
        MonoClass* scriptClass = mono_object_get_class(monoScript);

        MonoMethod* updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 1);

        void* args[1];
        args[0] = &deltaTime;

        mono_runtime_invoke(updateMethod, monoScript, args, nullptr);
    }
}

bool ScriptComponent::LoadScript(const std::string& scriptName)
{
    std::string scriptPath = "../Script/" + scriptName + ".cs";

    if (!std::filesystem::exists(scriptPath)) {
        LOG(LogType::LOG_ERROR, "El script %s no se encontró en la ruta %s", scriptName.c_str(), scriptPath.c_str());
        return false;
    }

    MonoClass* scriptClass = mono_class_from_name(MonoManager::GetInstance().GetImage(), "", scriptName.c_str());
    if (!scriptClass) {
        LOG(LogType::LOG_ERROR, "No se pudo encontrar la clase %s en el ensamblado.", scriptName.c_str());
        return false;
    }

    monoScript = mono_object_new(MonoManager::GetInstance().GetDomain(), scriptClass);
    if (!monoScript) {
        LOG(LogType::LOG_ERROR, "No se pudo crear una instancia del script %s.", scriptName.c_str());
        return false;
    }

    mono_runtime_object_init(monoScript);

    LOG(LogType::LOG_INFO, "Script %s cargado correctamente.", scriptName.c_str());

/*   Setting the cplusplus gameObject reference of the MonoBehaviour scripto        */

    if (MonoManager::GetInstance().scriptIDs.contains(scriptName) == false) {

        MonoManager::GetInstance().scriptIDs.emplace(std::pair<std::string, int>(scriptName, MonoManager::GetInstance().GetNewScriptClassID()));
    }
    uintptr_t goPtr = reinterpret_cast<uintptr_t>(owner);
    MonoClassField* field = mono_class_get_field_from_name(scriptClass, "CplusplusInstance");
    mono_field_set_value(monoScript, field, &goPtr);


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
        "        // Lógica de actualización\n"
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