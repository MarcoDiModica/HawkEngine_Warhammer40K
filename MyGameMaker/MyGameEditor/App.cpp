#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <algorithm>

#include "App.h"
#include "MyWindow.h"
#include "HardwareSettings.h"
#include "MyGUI.h"
#include "Input.h"
#include "Root.h"
#include "Log.h"
#include "UISettings.h"
#include "UIMainMenuBar.h"
#include "External/Optick/include/optick.h"
#include "MyAudioEngine/AudioManager.h"

App::App() {
	m_logs.reserve(MAX_LOGS);

	m_modules.reserve(10);

	window = new Window("HawkEngine", 1280, 720);
	input = new Input(this);
	hardwareInfo = new HardwareInfo(this);

#ifndef _BUILD
	gui = new MyGUI(this);
#endif

	root = new Root(this);

#ifndef _BUILD
	camera = new EditorCamera(this);
#endif

	physicsModule = new PhysicsModule();
	scene_serializer = new SceneSerializer(this);

	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardwareInfo, true);

#ifndef _BUILD
	AddModule(gui, true);
#endif

	AddModule(root, true);

#ifndef _BUILD
	AddModule(camera, true);
#endif

	AddModule(scene_serializer, true);

	m_lastFrameTime = high_res_clock::now();
	m_frameStart = m_lastFrameTime;
}

App::~App() {
	for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) {
		delete* it;
	}
	m_modules.clear();
}

bool App::Awake() {
	SetFpsCap(m_targetFrameRate);

#ifndef _BUILD
	camera->GetTransform().GetPosition() = vec3(0, 1, 4);
	camera->GetTransform().Rotate(glm::radians(180.0), vec3(0, 1, 0));
#endif

	for (auto* module : m_modules) {
		if (!module->Awake()) {
			return false;
		}
	}

	physicsModule->Awake();

	return true;
}

bool App::Start() {
	m_deltaTime = 0.016;

	for (auto* module : m_modules) {
		if (!module->Start()) {
			return false;
		}
	}

	physicsModule->Start();

	Application->window->SetTitle("Warhammer 40k: Blood & Thunder");

	return true;
}

bool App::Update()
{
#ifdef PROFILE
	OPTICK_FRAME("Main Loop")
#endif // PROFILE

	bool ret = true;
	PrepareUpdate();

	if (ret) ret = PreUpdate();
	if (ret) ret = DoUpdate();
	if (ret) ret = PostUpdate();

	/*Application->window->SetTitle(
		std::string("HawkEngine - FPS: ") + std::to_string(m_fps.load()) +
		" - Frame Time: " + std::to_string(m_deltaTime * 1000.0) + "ms");*/

	FinishUpdate();

	return ret;
}

void App::PrepareUpdate() {
	m_frameStart = high_res_clock::now();

	hasChangedScene = false;
}

bool App::PreUpdate() {
#ifdef PROFILE
	OPTICK_CATEGORY("PreUpdate", Optick::Category::GameLogic);
#endif

	for (auto* module : m_modules) {
		if (!module->active) continue;
		if (!module->PreUpdate()) return false;
	}

	physicsModule->PreUpdate();

	return true;
}

bool App::PerformFixedUpdate() {
	for (auto* module : m_modules) {
		if (!module->active) continue;
		if (!module->FixedUpdate()) return false;
	}
	return true;
}

bool App::DoUpdate() {
#ifdef PROFILE
	OPTICK_CATEGORY("DoUpdate", Optick::Category::GameLogic);
#endif

	m_fixedTimeAccumulator += m_deltaTime;

	int fixedUpdatesThisFrame = 0;

	while (m_fixedTimeAccumulator >= FIXED_TIME_STEP) {
		if (!PerformFixedUpdate()) return false;

		m_fixedTimeAccumulator -= FIXED_TIME_STEP;
		fixedUpdatesThisFrame++;

		if (fixedUpdatesThisFrame >= MAX_FIXED_UPDATES) {
			m_fixedTimeAccumulator = 0;
			break;
		}
	}

	for (auto* module : m_modules) {
		if (!module->active) continue;
		if (!module->Update(m_deltaTime)) return false;

		if (hasChangedScene) break;
	}

	physicsModule->Update(m_deltaTime);
	AudioManager::Update(m_deltaTime);

	return true;
}

bool App::PostUpdate() {
#ifdef PROFILE
	OPTICK_CATEGORY("PostUpdate", Optick::Category::GameLogic);
#endif

	for (auto* module : m_modules) {
		if (!module->active) continue;
		if (!module->PostUpdate()) return false;
	}

	physicsModule->PostUpdate();

	return true;
}

void App::FinishUpdate() {
	m_fpsUpdateTimer += m_deltaTime;
	m_frameCount++;

	if (m_fpsUpdateTimer >= 1.0) {
		m_fps.store(m_frameCount);
		m_frameCount = 0;
		m_fpsUpdateTimer = 0.0;

#ifndef _BUILD
		if (gui && gui->UIsettingsPanel) {
			gui->UIsettingsPanel->AddFpsMark(m_fps.load());
		}
#endif
	}

	if (m_capFrames && m_targetFrameRate > 0) {
		double targetFrameTime = 1.0 / m_targetFrameRate;
		double frameDuration = std::chrono::duration<double>(high_res_clock::now() - m_frameStart).count();

		if (frameDuration < targetFrameTime) {
			Uint32 delayMS = static_cast<Uint32>((targetFrameTime - frameDuration) * 1000);
			if (delayMS > 0) {
				SDL_Delay(delayMS);
			}
		}
	}

	auto now = high_res_clock::now();
	m_deltaTime = std::chrono::duration<double>(now - m_lastFrameTime).count();
	m_deltaTime = std::clamp(m_deltaTime, MIN_FRAME_TIME, MAX_FRAME_TIME);
	m_lastFrameTime = now;
}

bool App::CleanUP() {
	for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) {
		if (!(*it)->CleanUp()) {
			return false;
		}
	}
	return true;
}

void App::AddLog(LogType type, const char* entry) {
	std::string message = entry;

	for (auto it = m_logs.begin(); it != m_logs.end(); ++it) {
		if (it->type == type && it->message == message) {
			int newCount = it->repeatCount + 1;
			LogInfo updatedInfo = { type, message, newCount };
			*it = updatedInfo;
			return;
		}
	}

	if (m_logs.size() >= MAX_LOGS) {
		m_logs.erase(m_logs.begin());
	}

	m_logs.push_back({ type, message, 1 });
}

const std::vector<LogInfo>& App::GetLogs() const {
	return m_logs;
}

void App::CleanLogs() {
	m_logs.clear();
}

void App::AddModule(Module* module, bool activate) {
	if (activate) {
		module->Init();
	}
	m_modules.push_back(module);
}

void App::SetFpsCap(int fps) {
	m_targetFrameRate = (fps > 0) ? fps : 60;
	m_targetFrameTime = 1.0 / m_targetFrameRate;
}

