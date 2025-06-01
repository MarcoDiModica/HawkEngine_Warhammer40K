#pragma once

#include <string>
#include <memory>

class GameObject;

class PrefabManager
{
public:
    static bool SavePrefab(const std::shared_ptr<GameObject>& go, const std::string& path);
    static bool ApplyPrefabToGameObject(GameObject* target, const std::string& prefabPath);
    static std::shared_ptr<GameObject> LoadPrefab(const std::string& path);
    static std::string GetPrefabDirectory();
    static void EnsurePrefabDirectoryExists();
    static std::string SanitizeName(const std::string& name);
    static std::string GetUniquePrefabPath(const std::string& baseName);
};
