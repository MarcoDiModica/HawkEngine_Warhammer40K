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
#include "../MyAudioEngine/AudioEngine.h"
#include "../MyParticlesEngine/ParticleFX.h"
#include "../MyGameEngine/Image.h"

#define FIXED_TIME_STEP 0.016667f  
#define MAX_FIXED_UPDATES 3 
#define MIN_FRAME_TIME 0.0005
#define MAX_FRAME_TIME 0.1

#undef PROFILE

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

	void AddLog(LogType type, const char* entry);
	const std::vector<LogInfo>& GetLogs() const;
	void CleanLogs();

	int GetFps() const { return m_fps; }
	double GetDt() const { return m_deltaTime; }
	void SetFpsCap(int fps);
	void EnableFrameCap(bool enable) { m_capFrames = enable; }

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
			"Assets/Textures/BloodSplash_decals1_Yiwei.png",
			"Assets/Textures/fireSpritesheetpx2.png",
			"Assets/Textures/MawlocSmokeSpritesheet.png",
			"Assets/Textures/rock.png",
			"Assets/Textures/VFX_lictor_leap.png",
			"Assets/Textures/VFX_lictor_dodge_cue.png",
			"Assets/Textures/Magnet_Area_animation_SpriteSheet_Yiwei.png",
			"Assets/Textures/TreeSpritesheet.png",
			"Assets/Textures/Tree2Spritesheet.png",
			"Assets/Textures/Tree3Spritesheet.png",
			"Assets/Textures/Red_Thirst_Sprite_sheet.png",
			"Assets/Textures/niebla30Frames.png",
			"Assets/Textures/LictorSlashSpritesheetv6.png",
			"Assets/Textures/Lictor-blood.png",
			"Assets/Textures/MawlocSlashSpritesheet.png",
			"Assets/Textures/Ability_recharge_vfx.png",
			"Assets/Textures/grenade_shot_smoke_vfx.png",
			"Assets/Textures/Bullet Eject vfx.png",
			"Assets/Textures/Red_thirst_lost_vfx.png",
			"Assets/Textures/leaf3.png"

		};

		for (const auto& path : texturePaths) {
			auto image = std::make_shared<Image>();
			if (image->LoadTexture(path)) {
				loadedPartTextures[path] = image;
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

	std::unordered_map<std::string, std::shared_ptr<Image>> loadedPartTextures;

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

	bool m_capFrames = false;
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



#endif	// !__APP_H__