#pragma once

#ifndef __MODULE_H__
#define __MODULE_H__


class App;

class Module
{
public:

	Module() {}

	Module(App* parent) : active(false) ,Application(parent)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake()
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(double dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	void Enable() 
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable() 
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	bool active;
	App* Application;
};

#endif // !__MODULE_H__
