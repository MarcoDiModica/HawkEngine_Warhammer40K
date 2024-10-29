#ifndef __UI_SETTINGS_H__
#define __UI_SETTINGS_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

#define RECORD_SIZE 100

enum class SettingType
{
	PERFORMANCE,
	HARDWARE,
	SOFTWARE,
	DEFAULT
};

class UISettings : public UIElement
{
public:
	UISettings(UIType type, std::string name);
	~UISettings();

	bool Draw();
	void AddFpsMark(int fps);

private:

	void PerformanceData();
	void HardwareData();
	void SoftwareData();

private:

	SettingType settingType = SettingType::PERFORMANCE;

	std::vector<float> fpsRecord;
	std::vector<double> msRecord;

	const char* fpsMarkList[4]
	{
		"30",
		"60",
		"120",
		"240"
	};
	
};

#endif // !__UI_SETTINGS_H__
