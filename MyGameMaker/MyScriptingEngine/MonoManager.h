#pragma once

class MonoManager 
{
public:
    static void Init();
    static void Shutdown();
private:
    static void InitMono();
    static void ShutdownMono();
};