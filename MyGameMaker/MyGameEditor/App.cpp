#include "App.h"
#include "MyWindow.h"
#include "HardwareSettings.h"
#include "MyGUI.h"
#include "Input.h"
#include "Root.h"
#include "Log.h"

#define MAX_LOGS_CONSOLE 1000

App::App() {

	window = new Window("Who is George" , 1280,720);

	input = new Input(this);

	hardwareInfo = new HardwareInfo(this);

	gui = new MyGUI(this);

	root = new Root(this);



	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardwareInfo, true);
	AddModule(gui, true);
	AddModule(root, true);


};


bool App::Awake() { 

	for (const auto& module : modules) {
		if (module->Awake()) continue;
		else {
			return false;
		}
	}
	return true; 
}
bool App::Start() { 
	

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
	/*frameStart = std::chrono::steady_clock::now();*/
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

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Update(0.016f) == false)
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
	// dt calculation
	//frameEnd = std::chrono::steady_clock::now();
	//auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEnd - frameStart);

	//dt = frameDuration.count();

	//if (frameDuration < targetFrameDuration)
	//{
	//	std::chrono::duration<double> sleepTime = targetFrameDuration - frameDuration;
	//	std::this_thread::sleep_for(sleepTime);

	//	dt = targetFrameDuration.count();
	//}

	//// fps calculation
	//dtCount += dt;
	//frameCount++;

	//if (dtCount >= 1)
	//{
	//	fps = frameCount;
	//	frameCount = 0;
	//	dtCount = 0;
	//}

	//app->gui->panelSettings->AddFpsValue(fps);
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

