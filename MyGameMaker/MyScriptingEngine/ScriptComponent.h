#pragma once
#include "../MyGameEngine/Component.h"
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <filesystem>
#include "MyGameEditor/Log.h"
class ScriptComponent : public Component
{
public:
	ScriptComponent(GameObject* owner);
	~ScriptComponent() override;

	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	ComponentType GetType() const override { return ComponentType::SCRIPT; };
	std::unique_ptr<Component> Clone(GameObject* new_owner) override;

	void SetMonoScript(MonoObject* script) { monoScript = script; }
	bool LoadScript(const std::string& scriptName);

	bool RefreshScriptInstance();
	MonoObject* GetSharpObject() const { return monoScript; }

	std::string GetTypeName() const;
	std::string GetCurrentScriptName() const { return currentScriptName; }
	
	void InvokeMonoMethod(const std::string& methodName, GameObject& other);

	bool HasErrors() const { return hasErrors; }
	void ResetErrorState() { hasErrors = false; }

	MonoObject* monoScript = nullptr;
	std::filesystem::file_time_type GetLastWriteTime() const { return lastWriteTime; }
	void SetLastWriteTime(std::filesystem::file_time_type newTime) { lastWriteTime = newTime; }
	std::filesystem::file_time_type lastWriteTime;
	std::string currentScriptName;
	
protected:
	friend class SceneSerializer;
	YAML::Node encode();
	
	bool decode(const YAML::Node& node);

private:
	bool HandleException(MonoObject* exception, const std::string& methodName);
	std::string GetMonoExceptionDetails(MonoObject* exception);
	bool hasErrors = false;
};