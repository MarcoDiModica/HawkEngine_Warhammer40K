#include "ObjectRegistry.h"
#include "GameObject.h"

std::unordered_map<HawkUUID, GameObject*> ObjectRegistry::s_ObjectMap;
std::mutex ObjectRegistry::s_RegistryMutex;

void ObjectRegistry::RegisterObject(const HawkUUID& id, GameObject* object) {
	if (!id.IsValid() || !object)
		return;

	std::lock_guard<std::mutex> lock(s_RegistryMutex);
	s_ObjectMap[id] = object;
}

void ObjectRegistry::UnregisterObject(const HawkUUID& id) {
	if (!id.IsValid())
		return;

	std::lock_guard<std::mutex> lock(s_RegistryMutex);
	s_ObjectMap.erase(id);
}

GameObject* ObjectRegistry::FindObject(const HawkUUID& id) {
	if (!id.IsValid())
		return nullptr;

	std::lock_guard<std::mutex> lock(s_RegistryMutex);
	auto it = s_ObjectMap.find(id);
	if (it != s_ObjectMap.end())
		return it->second;

	return nullptr;
}

void ObjectRegistry::Clear() {
	std::lock_guard<std::mutex> lock(s_RegistryMutex);
	s_ObjectMap.clear();
}