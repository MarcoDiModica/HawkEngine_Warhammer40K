// UIProject.h
#pragma once

#include "UIElement.h"
#include "MyGameEngine/Image.h"

#include <filesystem>
#include <string>
#include <future>
#include <unordered_map>
#include <vector>

class UIProject : public UIElement
{
public:
    UIProject(UIType type, std::string name);
    ~UIProject();

    bool Draw() override;

private:
    void DrawDirectoryTree();
    void DrawFolderContents(const std::filesystem::path& path);
    void DrawBreadcrumbs();
    void HandleFileSelection(const std::filesystem::path& filePath);
    Image* GetIconForFile(const std::filesystem::path& filePath);
    void ShowContextMenu();
    void StartDirectoryListing(const std::filesystem::path& path);
    void CreateNewItem(const std::filesystem::path& path, const std::string& itemName, bool isDirectory);

    std::filesystem::path directoryPath;
    std::filesystem::path renamePath;
    std::filesystem::path selectedDirectory;
    std::filesystem::path selectedFile;
    std::string currentSceneFile;
    bool firstDraw = true;

    std::unordered_map<std::string, Image*> iconCache;

    std::future<std::vector<std::filesystem::path>> directoryListingFuture;
    std::vector<std::filesystem::path> currentDirectoryEntries;
    bool isLoading = false;

    enum class SortOption {
        Name,
        Type,
        Size,
        LastModified
    };

    SortOption currentSortOption;
    bool sortAscending;
};