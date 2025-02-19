#pragma once

#include <filesystem>

class MonoManager 
{
public:
    static void Init();
    static void Shutdown();

    static bool LoadAssembly(const std::filesystem::path& filepath);
    static bool LoadAppAssembly(const std::filesystem::path& filepath);

    static void ReloadAssembly();
private:
    static void InitMono();
    static void ShutdownMono();
};