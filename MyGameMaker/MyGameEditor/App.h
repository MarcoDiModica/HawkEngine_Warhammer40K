#ifndef __APP_H__
#define __APP_H__

#pragma once

class  Module;
class Window;
class Input;

class App
{
public:

	App();
	~App();

	bool Awake();
	bool Start();
	bool Update();
	bool CleanUP();

	// Add a new module to handle
	void AddModule(Module* module, bool activate);

	Window* window = nullptr;
	Input* input = nullptr;

};

extern App* Application;

#endif	// !__APP_H__