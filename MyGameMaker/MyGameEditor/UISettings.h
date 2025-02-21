#ifndef __UI_SETTINGS_H__
#define __UI_SETTINGS_H__
#pragma once

#include <vector>
#include <string>
#include "imgui.h"

#include "UIElement.h"

#define RECORD_SIZE 100

enum class SettingType
{
	PERFORMANCE,
	HARDWARE,
	SOFTWARE,
	EDITORCAM,
	DEFAULT
};

enum class Theme
{
	DARK,
	LIGHT,
	CUSTOM
};

class UISettings : public UIElement
{
public:
	UISettings(UIType type, std::string name);
	~UISettings();

	bool Draw();
	void AddFpsMark(int fps);
	void AddMemoryMark();

	// Style Management

	void ApplyDarkTheme(ImGuiStyle& style);
	void ApplyLightTheme(ImGuiStyle& style);
	void ApplyCustomTheme(ImGuiStyle& style);

private:

	void PerformanceData();
	void HardwareData();
	void SoftwareData();
	void EditorCamData();

	bool IsWindowMinimized();

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