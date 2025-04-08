#pragma once

#include "HawkUUID.h"
#include <unordered_map>
#include <mutex>

class GameObject;

class ObjectRegistry {
public:
	static void RegisterObject(const HawkUUID& id, GameObject* object);

	static void UnregisterObject(const HawkUUID& id);

	static GameObject* FindObject(const HawkUUID& id);

	static void Clear();

private:
	static std::unordered_map<HawkUUID, GameObject*> s_ObjectMap;

	static std::mutex s_RegistryMutex;
};