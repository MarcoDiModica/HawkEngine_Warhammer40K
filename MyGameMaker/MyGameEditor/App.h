#ifndef __APP_H__
#define __APP_H__
#pragma once

#include <vector>
#include <list>
#include <chrono>

#include "Log.h"
#include "EditorCamera.h"
#include "SceneSerializer.h"
#include "Root.h"
//#include "../MyGameEngine/Mesh.h"
#include "../MyPhysicsEngine/PhysicsModule.h"

#define FIXED_INTERVAL 0.02

class  Module;
class Window;
class Input;
class HardwareInfo;
class SceneSerializer;
class MyGUI;
class Root;
class Camera;
class Gizmos;
class UIMainMenuBar;

class Mesh;

class App
{
public:

	App();
	~App();

	bool Awake();
	bool Start();
	bool Update();
	bool CleanUP();

	void PrepareUpdate();
	bool PreUpdate();
	bool DoUpdate();
	bool FixedUpdate();
	bool PostUpdate();
	void FinishUpdate();

	std::vector<LogInfo> GetLogs();
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	int GetFps() const;
	void SetFpsCap(int fps);
	double GetDt() const;

	// Add a new module to handle
	void AddModule(Module* module, bool activate);

	Window* window = nullptr;
	Input* input = nullptr;
	HardwareInfo* hardwareInfo = nullptr;
	MyGUI* gui = nullptr;
	Root* root = nullptr;
	EditorCamera* camera = nullptr;
	SceneSerializer* scene_serializer = nullptr;
	Gizmos* gizmos = nullptr;
	PhysicsModule* physicsModule = nullptr;

	Mesh ElMesh;

private:

	double fixedCounter = FIXED_INTERVAL;

	LogInfo logInfo;
	std::vector<LogInfo> logs;

	std::list<Module*> modules;

	std::chrono::duration<double> targetFrameDuration;
	std::chrono::steady_clock::time_point frameStart, frameEnd;

	int frameRate = 90;
	double dt = 0;
	double dtCount = 0;
	int frameCount = 0;
	int fps = 0;

protected:
	friend class UIMainMenuBar;
	bool play = false;
};

extern App* Application;



#endif	// !__APP_H__