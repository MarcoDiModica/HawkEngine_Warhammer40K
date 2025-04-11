#ifndef __APP_H__
#define __APP_H__
#pragma once

#include <vector>
#include <list>
#include <chrono>
#include <atomic>
#include <memory>

#include "Log.h"
#include "EditorCamera.h"
#include "SceneSerializer.h"
#include "Root.h"
#include "../MyPhysicsEngine/PhysicsModule.h"

#define FIXED_TIME_STEP 0.016667f  
#define MAX_FIXED_UPDATES 3 
#define MIN_FRAME_TIME 0.0005
#define MAX_FRAME_TIME 0.1

class Module;
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
	bool PostUpdate();
	void FinishUpdate();

	const std::vector<LogInfo>& GetLogs() const;
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	int GetFps() const { return m_fps; }
	double GetDt() const { return m_deltaTime; }
	void SetFpsCap(int fps);
	void EnableFrameCap(bool enable) { m_capFrames = enable; }

	void AddModule(Module* module, bool activate);

	// TODO: HACERLOS UNIQUE PTR
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

	bool play = false;
	bool hasChangedScene = false;

private:
	using high_res_clock = std::chrono::high_resolution_clock;
	using time_point = std::chrono::time_point<high_res_clock>;

	bool PerformFixedUpdate();

	std::vector<LogInfo> m_logs;
	static constexpr size_t MAX_LOGS = 1000;

	std::vector<Module*> m_modules;

	time_point m_frameStart;
	time_point m_lastFrameTime;
	double m_deltaTime = 0.016;
	double m_fixedTimeAccumulator = 0.0;

	bool m_capFrames = true;
	int m_targetFrameRate = 60;
	double m_targetFrameTime = 1.0 / 60.0;

	std::atomic<int> m_fps;
	std::atomic<int> m_frameCount;
	double m_fpsUpdateTimer = 0.0;

	double m_longestFrame = 0.0;
	double m_shortestFrame = 1.0;
	double m_averageFrameTime = 0.0;
	int m_framesForAverage = 0;

protected:
	friend class UIMainMenuBar;
};

extern App* Application;

#endif // !__APP_H__