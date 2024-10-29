#ifndef __UI_SETTINGS_H__
#define __UI_SETTINGS_H__
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>

#define RECORD_SIZE 300

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
	void AddFpsValue(int fps);

private:

	void PerformanceData();
	void HardwareData();
	void SoftwareData();

private:

	SettingType settingType = SettingType::PERFORMANCE;

	std::vector<int> fpsRecord;
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
