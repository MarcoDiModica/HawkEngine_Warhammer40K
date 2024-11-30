#ifndef __UI_PROJECT_H__
#define __UI_PROJECT_H__
#pragma once

#include "UIElement.h"
#include <filesystem>

class UIProject : public UIElement
{
public:
	UIProject(UIType type, std::string name);
	~UIProject();

	bool Draw();
	std::pair<bool, uint32_t> DirectoryView(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

public:
	std::string directoryPath;
};

#endif // !__UI_PROJECT_H__