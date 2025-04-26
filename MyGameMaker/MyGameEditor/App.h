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
#include "../MyAudioEngine/AudioEngine.h"

#define FIXED_INTERVAL 0.02
#undef PROFILE

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

using hrclock = std::chrono::high_resolution_clock;

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

	void LoadAllParticleTextures() {
		// Lista de texturas utilizadas en los presets
		std::vector<std::string> texturePaths = {
			"Assets/Textures/Smoke30Frames.png",
			"Assets/Textures/fire_spritesheet.png",
			"Assets/Textures/smoke_spritesheet.png",
			"Assets/Textures/ixplosion.png",
			"Assets/Textures/muzzle.png",
			"Assets/Textures/ShotGun Muzzle Flash_Spritesheet_Yiwei.png",
			"Assets/Textures/EnemyDash.png",
			"Assets/Textures/Acid_Splash.png",
			"Assets/Textures/acid_puddle.png",
			"Assets/Textures/ElectricityBall.png",
			"Assets/Textures/RailGunAuto.png",
			"Assets/Textures/RailGunSemi.png",
			"Assets/Textures/dropplet.png",
			"Assets/Textures/Spark.png",
			"Assets/Textures/thundaaar2.png",
			"Assets/Textures/Medicae_Stim.png",
			"Assets/Textures/Project3_Velocity_effect.png",
			"Assets/Textures/Project3_AmmunitionBlessing_effect_Yiwei.png",
			"Assets/Textures/BloodSplash_decals1_Yiwei.png"
		};

		for (const auto& path : texturePaths) {
			auto image = std::make_shared<Image>();
			if (image->LoadTexture(path)) {
				Application->loadedPartTextures[path] = image;
			}
			else {
				std::cerr << "Failed to load texture: " << path << std::endl;
			}
		}
	}

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
	std::unordered_map<std::string, std::shared_ptr<Image>> loadedPartTextures;

	bool play = false;
	bool hasChangedScene = false;

private:

	double fixedCounter = FIXED_INTERVAL;

	LogInfo logInfo;
	std::vector<LogInfo> logs;

	std::list<Module*> modules;

	std::chrono::duration<double> targetFrameDuration;
	std::chrono::steady_clock::time_point frameStart, frameEnd;

	bool capFrames = true; //false para tener el maximo de fps posible
	int frameRate = 240; //Fake frameRate no borro por si acaso
	glm::uint32 frameRateCap = 16.67; //forlmula para saber que numero poner aqui: 1000ms / desired fps ej: 1000ms / 60fps = 16,67
	double dt = 0;
	double dtCount = 0;
	int frameCount = 0;
	int fps = 0;
	hrclock::time_point lastTime = hrclock::now();
protected:
	friend class UIMainMenuBar;
	
};

extern App* Application;



#endif	// !__APP_H__