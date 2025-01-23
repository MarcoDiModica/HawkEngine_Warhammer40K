#ifndef __UI_PROJECT_H__
#define __UI_PROJECT_H__
#pragma once

#include "UIElement.h"
#include "MyGameEngine/Image.h"
#include <filesystem>

class UIProject : public UIElement
{
public:
	UIProject(UIType type, std::string name);
	~UIProject();

	bool Draw() override;
	std::pair<bool, uint32_t> DirectoryView(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

public:
	std::string directoryPath;
	std::string currentSceneFile;

	void HandleFileSelection(const std::string& filePath);

private:
	Image* folderIcon;

	Image* imageIcon;
	Image* sceneIcon;
	Image* matIcon;
	Image* meshIcon;

};

#endif // !__UI_PROJECT_H__