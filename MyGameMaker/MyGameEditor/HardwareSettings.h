#ifndef __HARDWARE_SETTINGS_H__
#define __HARDWARE_SETTINGS_H__
#pragma once

#include "Module.h"
#include "types.h"

#include <string>

struct HardwareSettings
{
	char sdlVersion[64] = "";
	float ram = 0.f;
	unsigned int cpuCores = 0;
	unsigned int cacheLineSize = 0;

	bool rdtsc = false;
	bool altiVec = false;
	bool mmx = false;
	bool now3d = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
	bool avx = false;
	bool avx2 = false;
	std::string gpuVendor;
	std::string gpuBrand;
	std::string gpuDriverVersion;
	float vramBudget = 0.f;
	float vramUsage = 0.f;
	float vramAvailable = 0.f;
	float vramReserved = 0.f;

};

class HardwareInfo : public Module
{
public: 
	HardwareInfo(App* app);
	virtual ~HardwareInfo();

	bool Awake();
	bool Start();

	bool CleanUp();

	const HardwareSettings& GetSettings() const;

private:
	mutable HardwareSettings settings;
};

#endif // !__HARDWARE_SETTINGS_H__
