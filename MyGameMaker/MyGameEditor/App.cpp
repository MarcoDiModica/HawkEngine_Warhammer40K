#include <SDL2/SDL.h>
#include <iostream>
#include "App.h"
#include "MyWindow.h"
#include "HardwareSettings.h"
#include "MyGUI.h"
#include "Input.h"
#include "Root.h"

#include "Log.h"

#include "UISettings.h"


#define MAX_LOGS_CONSOLE 1000
#define MAX_FIXED_UPDATES 5



App::App() {

	window = new Window("HawkEngine" , 1280,720);

	input = new Input(this);

	hardwareInfo = new HardwareInfo(this);

	gui = new MyGUI(this);

	root = new Root(this);

	camera = new EditorCamera(this);
	physicsModule = new PhysicsModule();

	//gizmos = new Gizmos(this);

	scene_serializer = new SceneSerializer(this);

	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardwareInfo, true);
	AddModule(gui, true);
	AddModule(root, true);
	AddModule(camera, true);
	//AddModule(gizmos, true);
	AddModule(scene_serializer, true);
	AddModule(physicsModule, true);

};


bool App::Awake() { 

	targetFrameDuration = (std::chrono::duration<double>)1 / frameRate;

	for (const auto& module : modules) {
		if (module->Awake()) continue;
		else {
			return false;
		}
	}
	return true; 
}

bool App::Start() { 
	
	dt = 0;

	for (const auto& module : modules) {
		
		if( module->Start()) continue;

		else { 
			return false; 
		}

	}
	
	
	return true; 
}

bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	/*if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;*/

	// Theese perform the lifecycle update calls for each module

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();

	//time_since_start = start_timer->ReadSec();

	//if (state == GameState::PLAY || state == GameState::PLAY_ONCE) {
	//	game_time = game_timer->ReadSec(scale_time);
	//}

	return ret;
}


void App::PrepareUpdate()
{
	frameStart = std::chrono::steady_clock::now();
}

bool App::PreUpdate()
{
	//OPTICK_CATEGORY("PreUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PreUpdate() == false)
			return false;
	}

	return true;
}

bool App::DoUpdate()
{
	//OPTICK_CATEGORY("DoUpdate", Optick::Category::GameLogic);

	fixedCounter += dt;
	int numFixedUpdates = 0;

	while (fixedCounter > FIXED_INTERVAL) {
		numFixedUpdates++;
		fixedCounter -= FIXED_INTERVAL;
		//Call fixedUpdate
		for (const auto& module : modules)
		{
			if (module->active == false)
				continue;

			if (module->FixedUpdate() == false)
				return false;
		}

		// Prevent spiral of death
		if (numFixedUpdates > MAX_FIXED_UPDATES) {
			break;
		}
	}
	//std::cout << std::endl << "There were " << numFixedUpdates << "fixed updates";

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Update(dt) == false)
			return false;
	}

	
	


	return true;
}

bool App::PostUpdate()
{
	//OPTICK_CATEGORY("PostUpdate", Optick::Category::GameLogic);

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PostUpdate() == false)
			return false;
	}

	return true;
}

void App::FinishUpdate()
{
	frameEnd = std::chrono::steady_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEnd - frameStart);

	dt = frameDuration.count();



	if (frameDuration < targetFrameDuration)
	{
		auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::max(targetFrameDuration - frameDuration, std::chrono::duration<double>(0))
		);

		// Delay to maintain target frame rate
		SDL_Delay(static_cast<Uint32>(sleepTime.count()));

		dt = targetFrameDuration.count();
	}
	//std::cout << std::endl << dt;

	dtCount += dt;
	frameCount++;

	if (dtCount >= 1)
	{
		fps = frameCount;
		frameCount = 0;
		dtCount = 0;
	}

	Application->gui->UIsettingsPanel->AddFpsMark(fps);

}

bool App::CleanUP() { return true; }

void App::AddLog(LogType type, const char* entry)
{
	if (logs.size() > MAX_LOGS_CONSOLE)
		logs.erase(logs.begin());

	std::string toAdd = entry;
	LogInfo info = { type, toAdd };

	logs.push_back(info);
}

std::vector<LogInfo> App::GetLogs() 
{ 
	return logs; 
}

void App::CleanLogs() 
{ 
	logs.clear(); 
}

// Add a new module to handle
void App::AddModule(Module* module, bool activate) { 

	if (activate == true){ module->Init(); }

	modules.push_back(module);

}

int App::GetFps() const { return frameRate;}

double App::GetDt() const { return dt; }

void App::SetFpsCap(int fps) {
	this->frameRate = frameRate == 0 ? Application->window->GetDisplayRefreshRate() : frameRate;
	targetFrameDuration = (std::chrono::duration<double>)1 / this->frameRate;
}

