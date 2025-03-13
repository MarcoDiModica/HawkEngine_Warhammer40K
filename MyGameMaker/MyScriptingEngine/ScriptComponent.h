#pragma once


#include "../MyGameEngine/Component.h"
#include <mono/metadata/object.h>
#include <filesystem>
#include "MyGameEditor/Log.h"

class ScriptComponent : public Component
{
public:
    ScriptComponent(GameObject* owner);
    ~ScriptComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override {};
    ComponentType GetType() const override { return ComponentType::SCRIPT; };

    std::unique_ptr<Component> Clone(GameObject* new_owner) override { return std::make_unique<ScriptComponent>(new_owner); }

    void SetMonoScript(MonoObject* script) { monoScript = script; }

    bool LoadScript(const std::string& scriptName);
    bool CreateNewScript(const std::string& scriptName, const std::string& baseScriptName);

    MonoObject* GetSharpObject() { return monoScript; }
    std::string GetTypeName() const;

    void InvokeMonoMethod(const std::string& methodName, GameObject* other);

    MonoObject* monoScript = nullptr;

    std::filesystem::file_time_type GetLastWriteTime() const { return lastWriteTime; }
    void SetLastWriteTime(std::filesystem::file_time_type newTime) { lastWriteTime = newTime; }

    std::filesystem::file_time_type lastWriteTime;

protected:
	friend class SceneSerializer;
    
	YAML::Node encode() override {
		YAML::Node node;
		node["name"] = GetTypeName();
		return node;
	}
    
	bool decode(const YAML::Node& node) override {
		if (!node["name"]) {
			return false;
		}

		std::string name = node["name"].as<std::string>();
		bool success = LoadScript(name);
        if (!success) {
			LOG(LogType::LOG_ERROR, "Script %s not found", name.c_str());
		}
		else {
			LOG(LogType::LOG_INFO, "Script %s loaded", name.c_str());
            Start();
		}

		return success;
	}
};