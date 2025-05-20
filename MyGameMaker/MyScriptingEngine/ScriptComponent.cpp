#include "ScriptComponent.h"
#include "MonoManager.h"
#include <fstream>
#include <filesystem>
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>

#include "../MyGameEditor/Log.h"
#include "mono/metadata/mono-gc.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner) {
	updateInStop = false;
	hasErrors = false;
}

ScriptComponent::~ScriptComponent() {
	if (monoScript) {
		MonoManager::GetInstance().UnregisterMonoObject(this);
	}
}

void ScriptComponent::Awake()
{
	if (!monoScript || hasErrors) return;
    MonoClass* scriptClass = mono_object_get_class(monoScript);
    MonoMethod* awakeMethod = mono_class_get_method_from_name(scriptClass, "Awake", 0);

	if (!awakeMethod) return;

    MonoObject* exception = nullptr;
    mono_runtime_invoke(awakeMethod, monoScript, nullptr, &exception);
    if (exception) {
        MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
        const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
        LOG(LogType::LOG_ERROR, "AwakeError: %s", exceptionStr);
        mono_free((void*)exceptionStr);
    }
}

void ScriptComponent::Start() {
	if (!monoScript || hasErrors) return;

	MonoClass* scriptClass = mono_object_get_class(monoScript);
	MonoMethod* startMethod = mono_class_get_method_from_name(scriptClass, "Start", 0);

	if (!startMethod) return;

	MonoObject* exception = nullptr;
	mono_runtime_invoke(startMethod, monoScript, nullptr, &exception);

	HandleException(exception, "Start");
}

void ScriptComponent::Update(float deltaTime) {
	MonoObject* currentScript = monoScript;
	if (!currentScript || hasErrors) return;

	try {
		MonoClass* scriptClass = mono_object_get_class(currentScript);
		if (!scriptClass) {
			LOG(LogType::LOG_ERROR, "Failed to get class for script %s", GetTypeName().c_str());
			hasErrors = true;
			return;
		}

		MonoMethod* updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 1);
		if (!updateMethod) {
			return;
		}

		void* args[1];
		args[0] = &deltaTime;

		MonoObject* exception = nullptr;
		mono_runtime_invoke(updateMethod, monoScript, args, &exception);

		HandleException(exception, "Update");
	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "C++ Exception in Update: %s - Script: %s",
			e.what(), GetTypeName().c_str());
		hasErrors = true;
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Unknown C++ exception in Update - Script: %s",
			GetTypeName().c_str());
		hasErrors = true;
	}
}

void ScriptComponent::Destroy()
{
	if (!monoScript || hasErrors) {
		monoScript = nullptr;
		return;
	}

	MonoClass* scriptClass = mono_object_get_class(monoScript);
	MonoMethod* destroyMethod = mono_class_get_method_from_name(scriptClass, "Destroy", 0);

	if (destroyMethod) {
		MonoObject* exception = nullptr;
		mono_runtime_invoke(destroyMethod, monoScript, nullptr, &exception);

		HandleException(exception, "Destroy");
	}

	monoScript = nullptr;
}

std::unique_ptr<Component> ScriptComponent::Clone(GameObject* new_owner)
{
	auto newScript = std::make_unique<ScriptComponent>(new_owner);
	newScript->monoScript = monoScript;
	newScript->currentScriptName = currentScriptName;
	newScript->lastWriteTime = lastWriteTime;
	newScript->hasErrors = hasErrors;

	return newScript;
}

bool ScriptComponent::HandleException(MonoObject* exception, const std::string& methodName) {
	if (!exception) return false;

	try {
		std::string exceptionDetails = GetMonoExceptionDetails(exception);
		LOG(LogType::LOG_ERROR, "Script Error in %s.%s: %s",
			currentScriptName.c_str(), methodName.c_str(), exceptionDetails.c_str());
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Failed to get exception details for %s.%s",
			currentScriptName.c_str(), methodName.c_str());
	}

	hasErrors = true;
	return true;
}

std::string ScriptComponent::GetMonoExceptionDetails(MonoObject* exception) {
	std::string result;

	MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
	if (exceptionMessage) {
		const char* exceptionStr = mono_string_to_utf8(exceptionMessage);
		result = exceptionStr;
		mono_free((void*)exceptionStr);
	}
	else {
		result = "Unknown exception";
	}

	MonoClass* exceptionClass = mono_object_get_class(exception);
	MonoProperty* stackTraceProperty = mono_class_get_property_from_name(exceptionClass, "StackTrace");
	if (stackTraceProperty) {
		MonoMethod* getStackTrace = mono_property_get_get_method(stackTraceProperty);
		if (getStackTrace) {
			MonoObject* exception2 = nullptr;
			MonoObject* stackTraceObj = mono_runtime_invoke(getStackTrace, exception, nullptr, &exception2);
			if (stackTraceObj && !exception2) {
				MonoString* stackTraceStr = (MonoString*)stackTraceObj;
				const char* stackTrace = mono_string_to_utf8(stackTraceStr);
				result += "\nStack Trace:\n";
				result += stackTrace;
				mono_free((void*)stackTrace);
			}
		}
	}

	MonoProperty* sourceProperty = mono_class_get_property_from_name(exceptionClass, "Source");
	if (sourceProperty) {
		MonoMethod* getSource = mono_property_get_get_method(sourceProperty);
		if (getSource) {
			MonoObject* exception2 = nullptr;
			MonoObject* sourceObj = mono_runtime_invoke(getSource, exception, nullptr, &exception2);
			if (sourceObj && !exception2) {
				MonoString* sourceStr = (MonoString*)sourceObj;
				const char* source = mono_string_to_utf8(sourceStr);
				result += "\nSource: ";
				result += source;
				mono_free((void*)source);
			}
		}
	}

	return result;
}

bool ScriptComponent::LoadScript(const std::string& scriptName)
{
	hasErrors = false;

	std::string scriptPath = "../Script/" + scriptName + ".cs";

	if (!std::filesystem::exists(scriptPath)) {
		LOG(LogType::LOG_ERROR, "script %s not found in route %s", scriptName.c_str(), scriptPath.c_str());
		return false;
	}

	currentScriptName = scriptName;

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
		HandleException(exception, "Constructor");
		return false;
	}

	if (MonoManager::GetInstance().scriptIDs.contains(scriptName) == false) {
		MonoManager::GetInstance().scriptIDs.emplace(std::pair<std::string, int>(scriptName, MonoManager::GetInstance().GetNewScriptClassID()));
	}

	uintptr_t goPtr = reinterpret_cast<uintptr_t>(owner);
	MonoClassField* field = mono_class_get_field_from_name(scriptClass, "CplusplusInstance");
	mono_field_set_value(monoScript, field, &goPtr);

	if (std::filesystem::exists(scriptPath)) {
		lastWriteTime = std::filesystem::last_write_time(scriptPath);
	}

	if (monoScript) {
		MonoManager::GetInstance().RegisterMonoObject(this, monoScript);
	}

	return true;
}

bool ScriptComponent::RefreshScriptInstance()
{
	if (currentScriptName.empty()) {
		return false;
	}

	hasErrors = false;

	monoScript = nullptr;

	return LoadScript(currentScriptName);
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

MonoObject* GetMonoObjectFromGameObject(GameObject* gameObject) {
	if (!gameObject) return nullptr;
	if (gameObject->IsDestroyed()) return nullptr;

	MonoObject* registeredObject = MonoManager::GetInstance().GetMonoObjectForNative(gameObject);
	if (registeredObject) return registeredObject;

	try {
		MonoObject* monoObject = gameObject->GetSharp();
		if (monoObject) {
			MonoManager::GetInstance().RegisterMonoObject(gameObject, monoObject);
			return monoObject;
		}
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Exception creating Mono object for GameObject %s",
			gameObject->GetName().c_str());
	}

	return nullptr;
}

void ScriptComponent::InvokeMonoMethod(const std::string& methodName, GameObject& other) {
	if (!monoScript || hasErrors) return;
	if (owner && owner->IsDestroyed()) return;
	if (other.IsDestroyed()) return;

	MonoObject* currentScript = monoScript;
	auto& monoManager = MonoManager::GetInstance();

	class MonoObjectGuard {
		MonoManager& manager;
		void* thisPtr;
		void* otherPtr;
		MonoObject* newMonoOther;
		bool registeredOther;

	public:
		MonoObjectGuard(MonoManager& mgr, void* thisP, void* otherP)
			: manager(mgr), thisPtr(thisP), otherPtr(otherP), newMonoOther(nullptr), registeredOther(false) {
		}

		void setOtherMono(MonoObject* mono) {
			newMonoOther = mono;
			if (mono && !manager.GetMonoObjectForNative(otherPtr)) {
				manager.RegisterMonoObject(otherPtr, mono);
				registeredOther = true;
			}
		}

		~MonoObjectGuard() {
			if (registeredOther && newMonoOther) {
				manager.UnregisterMonoObject(otherPtr);
			}
		}
	};

	MonoObjectGuard guard(monoManager, this, &other);

	try {
		MonoClass* klass = mono_object_get_class(currentScript);
		if (!klass) {
			LOG(LogType::LOG_ERROR, "Failed to get mono class for script %s", GetTypeName().c_str());
			return;
		}

		MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 1);
		if (!method) {
			return;
		}

		MonoObject* monoOther = monoManager.GetMonoObjectForNative(&other);
		if (!monoOther) {
			monoOther = other.GetSharp();
			if (!monoOther) {
				LOG(LogType::LOG_ERROR, "Failed to get C# reference for GameObject %s", other.GetName().c_str());
				return;
			}
			guard.setOtherMono(monoOther);
		}

		void* args[1] = { monoOther };

		MonoObject* exception = nullptr;
		mono_runtime_invoke(method, currentScript, args, &exception);

		HandleException(exception, methodName);

	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "C++ Exception in InvokeMonoMethod: %s - Script: %s, Method: %s",
			e.what(), GetTypeName().c_str(), methodName.c_str());
		hasErrors = true;
	}
	catch (...) {
		LOG(LogType::LOG_ERROR, "Unknown C++ exception in InvokeMonoMethod - Script: %s, Method: %s",
			GetTypeName().c_str(), methodName.c_str());
		hasErrors = true;
	}
}

YAML::Node ScriptComponent::encode()
{
	YAML::Node node;
	node["name"] = GetTypeName();

	if (!monoScript) return node;

	MonoClass* scriptClass = mono_object_get_class(monoScript);
	void* iter = nullptr;
	MonoClassField* field = nullptr;
	YAML::Node fieldsNode;

	while ((field = mono_class_get_fields(scriptClass, &iter))) {
		uint32_t flags = mono_field_get_flags(field);
		if ((flags & MONO_FIELD_ATTR_STATIC) || !(flags & MONO_FIELD_ATTR_PUBLIC)) continue;

		const char* fieldName = mono_field_get_name(field);
		MonoType* type = mono_field_get_type(field);
		int typeCode = mono_type_get_type(type);

		switch (typeCode) {
		case MONO_TYPE_STRING: {
			MonoString* str = nullptr;
			mono_field_get_value(monoScript, field, &str);
			if (str) {
				char* cstr = mono_string_to_utf8(str);
				fieldsNode[fieldName] = std::string(cstr);
				mono_free(cstr);
			}
			break;
		}
		case MONO_TYPE_BOOLEAN: {
			bool value = false;
			mono_field_get_value(monoScript, field, &value);
			fieldsNode[fieldName] = value;
			break;
		}
		case MONO_TYPE_I4: {
			int value = 0;
			mono_field_get_value(monoScript, field, &value);
			fieldsNode[fieldName] = value;
			break;
		}
		case MONO_TYPE_R4: {
			float value = 0.0f;
			mono_field_get_value(monoScript, field, &value);
			fieldsNode[fieldName] = value;
			break;
		}
		case MONO_TYPE_CLASS: {
			MonoClass* fieldClass = mono_class_from_mono_type(type);
			const char* className = mono_class_get_name(fieldClass);
			const char* nameSpace = mono_class_get_namespace(fieldClass);

			MonoObject* fieldObj = nullptr;
			mono_field_get_value(monoScript, field, &fieldObj);

			if (!fieldObj) break;

			if (strcmp(nameSpace, "HawkEngine") == 0) {
				if (strcmp(className, "Prefab") == 0) {
					MonoClassField* pathField = mono_class_get_field_from_name(fieldClass, "path");
					if (pathField) {
						MonoString* str = nullptr;
						mono_field_get_value(fieldObj, pathField, &str);
						if (str) {
							char* cstr = mono_string_to_utf8(str);
							fieldsNode[fieldName] = std::string(cstr);
							mono_free(cstr);
						}
					}
				}
				else if (strcmp(className, "GameObject") == 0) {
					MonoClassField* cppField = mono_class_get_field_from_name(fieldClass, "CplusplusInstance");
					if (cppField) {
						uintptr_t ptr = 0;
						mono_field_get_value(fieldObj, cppField, &ptr);
						if (ptr) {
							GameObject* refGO = reinterpret_cast<GameObject*>(ptr);
							fieldsNode[fieldName] = refGO->GetName();
						}
					}
				}
			}
			break;
		}
		}
	}

	node["fields"] = fieldsNode;
	return node;
}




bool ScriptComponent::decode(const YAML::Node& node)
{
	if (!node["name"]) return false;

	std::string scriptName = node["name"].as<std::string>();
	if (!LoadScript(scriptName)) return false;

	if (!monoScript || !node["fields"]) return true;

	YAML::Node fieldsNode = node["fields"];
	MonoClass* scriptClass = mono_object_get_class(monoScript);

	void* iter = nullptr;
	MonoClassField* field = nullptr;

	while ((field = mono_class_get_fields(scriptClass, &iter))) {
		uint32_t flags = mono_field_get_flags(field);
		if ((flags & MONO_FIELD_ATTR_STATIC) || !(flags & MONO_FIELD_ATTR_PUBLIC)) continue;

		const char* fieldName = mono_field_get_name(field);
		if (!fieldsNode[fieldName]) continue;

		MonoType* type = mono_field_get_type(field);
		int typeCode = mono_type_get_type(type);

		switch (typeCode) {
		case MONO_TYPE_STRING: {
			std::string value = fieldsNode[fieldName].as<std::string>();
			MonoString* monoStr = mono_string_new(mono_domain_get(), value.c_str());
			mono_field_set_value(monoScript, field, monoStr);
			break;
		}
		case MONO_TYPE_BOOLEAN: {
			bool value = fieldsNode[fieldName].as<bool>();
			mono_field_set_value(monoScript, field, &value);
			break;
		}
		case MONO_TYPE_I4: {
			int value = fieldsNode[fieldName].as<int>();
			mono_field_set_value(monoScript, field, &value);
			break;
		}
		case MONO_TYPE_R4: {
			float value = fieldsNode[fieldName].as<float>();
			mono_field_set_value(monoScript, field, &value);
			break;
		}
		case MONO_TYPE_CLASS: {
			MonoClass* fieldClass = mono_class_from_mono_type(type);
			const char* className = mono_class_get_name(fieldClass);
			const char* nameSpace = mono_class_get_namespace(fieldClass);

			if (strcmp(nameSpace, "HawkEngine") == 0) {
				if (strcmp(className, "Prefab") == 0) {
					std::string path = fieldsNode[fieldName].as<std::string>();
					MonoObject* prefabObj = MonoManager::GetInstance().CreatePrefabReference(path);
					if (prefabObj) {
						MonoClassField* pathField = mono_class_get_field_from_name(fieldClass, "path");
						if (pathField) {
							MonoString* str = mono_string_new(mono_domain_get(), path.c_str());
							mono_field_set_value(prefabObj, pathField, str);
						}
						mono_field_set_value(monoScript, field, prefabObj);
					}
				}
				else if (strcmp(className, "GameObject") == 0) {
					std::string goName = fieldsNode[fieldName].as<std::string>();
					std::shared_ptr<GameObject> target = Application->root->FindGOByName(goName);

					if (target) {
						MonoObject* managedGO = MonoManager::GetInstance().CreateGameObjectReference(target.get());
						if (managedGO) {
							mono_field_set_value(monoScript, field, managedGO);
						}
					}
					else {
						Application->scene_serializer->g_PendingScriptReferences.push_back({ this, field, goName });
					}
				}
			}
			break;
		}
		}
	}

	return true;
}