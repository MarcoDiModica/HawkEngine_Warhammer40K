#ifndef __APP_H__
#define __APP_H__
#include <vector>
#include <list>
#include "Log.h"
#include "Camera.h"
#include "Root.h"
#include "MyGameEngine/Mesh.h"

#pragma once

class  Module;
class Window;
class Input;
class MyGUI;
class Root;

class Mesh;

class App
{
public:

	App();
	~App();

	bool Awake();
	bool Start();
	bool DoUpdate();

	void PrepareUpdate();
	bool PreUpdate();

	bool Update();

	bool PostUpdate();
	void FinishUpdate();

	bool CleanUP();

	void AddLog(LogType type, const char* entry);

	// Add a new module to handle
	void AddModule(Module* module, bool activate);

	Window* window = nullptr;
	Input* input = nullptr;
	MyGUI* gui = nullptr;
	Root* root = nullptr;

	Mesh ElMesh;

private:

	LogInfo logInfo;
	std::vector<LogInfo> logs;

	std::list<Module*> modules;

};

extern App* Application;



#endif	// !__APP_H__