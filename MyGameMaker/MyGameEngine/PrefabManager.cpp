#include "PrefabManager.h"  
#include "GameObject.h"  
#include "MyGameEditor/SceneSerializer.h"  
#include <yaml-cpp/yaml.h>  
#include <fstream>  
#include <iostream>  
#include <filesystem>  
#include <MyGameEditor/App.h>

bool PrefabManager::SavePrefab(const std::shared_ptr<GameObject>& go, const std::string& path)  
{  
   if (!go) return false;  

   try {  

        YAML::Node prefabNode = Application->scene_serializer->SerializeGameObject(*go); 

        std::ofstream fout(path);  
        if (!fout.is_open()) {  
            LOG(LogType::LOG_ERROR, "[SavePrefab] Cannot write to path: %s", path.c_str());  
            return false;  
        }  

        fout << prefabNode;
        fout.close();  

        go->SetPrefabSourcePath(path);
        return true;  
           
   }  
   catch (const std::exception& e) {  
       LOG(LogType::LOG_ERROR, "[SavePrefab] Exception: %s", e.what());  
       return false;  
   }  
}  
bool PrefabManager::ApplyPrefabToGameObject(GameObject* target, const std::string& prefabPath) {
    if (!target || prefabPath.empty()) return false;

    std::ifstream fin(prefabPath);
    if (!fin.is_open()) {
        LOG(LogType::LOG_ERROR, "[SyncComponentsFromPrefab] Cannot open prefab file: %s", prefabPath.c_str());
        return false;
    }

    try {
        YAML::Node prefabNode = YAML::Load(fin);
        fin.close();

        if (!prefabNode["Components"] || !prefabNode["Components"].IsMap()) return false;

        Application->scene_serializer->ApplyComponentDelta(target, prefabNode["Components"]);
        return true;

    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "[SyncComponentsFromPrefab] Exception: %s", e.what());
        return false;
    }
}

std::shared_ptr<GameObject> PrefabManager::LoadPrefab(const std::string& path)
{
    try {
        std::ifstream fin(path);
        if (!fin.is_open()) {
            LOG(LogType::LOG_ERROR, "[LoadPrefab] Cannot open prefab at: %s", path.c_str());
            return nullptr;
        }

        YAML::Node prefabNode = YAML::Load(fin);
        fin.close();

        if (!Application || !Application->scene_serializer) {
            LOG(LogType::LOG_ERROR, "[LoadPrefab] SceneSerializer is not initialized.");
            return nullptr;
        }

        std::shared_ptr<GameObject> go = Application->scene_serializer->DeserializeGameObject(prefabNode);
        if (!go) return nullptr;

        go->RegenerateUUID();  

        std::string baseName = go->GetName();
        std::string finalName = baseName + "_Instance";
        int suffix = 1;
        while (Application->root->FindGOByName(finalName)) {
            finalName = baseName + "_Instance" + std::to_string(++suffix);
        }
        go->SetName(finalName);

        go->SetPrefabSourcePath(path);
        return go;
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "[LoadPrefab] Exception: %s", e.what());
        return nullptr;
    }
}

void PrefabManager::ReloadGoToPrefab(GameObject* go, const std::string& prefabPath)
{
    if (!go || prefabPath.empty()) return;

    auto originalTransform = go->GetTransform();
    
	std::ifstream fin(prefabPath);
	if (!fin.is_open()) {
		LOG(LogType::LOG_ERROR, "[LoadPrefab] Cannot open prefab at: %s", prefabPath.c_str());
	}

	YAML::Node prefabNode = YAML::Load(fin);
	fin.close();

    go = Application->scene_serializer->DeserializeGameObject(prefabNode).get();

    go->GetTransform()->SetPosition(originalTransform->GetPosition());
    go->GetTransform()->SetRotation(originalTransform->GetEulerAngles());
    go->GetTransform()->SetScale(originalTransform->GetScale());
	LOG(LogType::LOG_INFO, "[ReloadGoToPrefab] Successfully reloaded GameObject from prefab: %s", prefabPath.c_str());
}

std::string PrefabManager::GetPrefabDirectory() {  
   return "Assets/Prefabs/";  
}  

void PrefabManager::EnsurePrefabDirectoryExists() {  
   std::filesystem::path prefabDir = GetPrefabDirectory();  
   if (!std::filesystem::exists(prefabDir)) {  
       std::filesystem::create_directories(prefabDir);  
   }  
}
std::string PrefabManager::SanitizeName(const std::string& name) {  
   std::string result = name;  
   for (char& c : result) {  
       if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {  
           c = '_';  
       }  
   }  
   return result;  
}
std::string PrefabManager::GetUniquePrefabPath(const std::string& baseName) {
    EnsurePrefabDirectoryExists();

    std::filesystem::path dir = GetPrefabDirectory();
    std::string cleanName = baseName;
    std::string ext = ".prefab.yaml";

    std::filesystem::path fullPath = dir / (cleanName + ext);
    int i = 1;

    while (std::filesystem::exists(fullPath)) {
        fullPath = dir / (cleanName + " (" + std::to_string(i++) + ")" + ext);
    }

    return fullPath.string();
}
