#pragma once
#include "UIElement.h"
#include <filesystem>
#include <vector>
#include <map>
#include <future>

#define ICON_SIZE 64.0f

class Image;

class UIProject : public UIElement {
public:
    enum class SortOption {
        Name,
        Type,
        Size,
        LastModified
    };

    UIProject(UIType type, std::string name);
    ~UIProject();

    bool Draw() override;

private:
    std::filesystem::path directoryPath;
    std::filesystem::path selectedDirectory;
    std::filesystem::path selectedFile;
    std::filesystem::path renamePath;
    std::filesystem::path newItemPath;
    std::string currentSceneFile;
    std::vector<std::filesystem::path> currentDirectoryEntries;
    std::future<std::vector<std::filesystem::path>> directoryListingFuture;

    std::map<std::string, Image*> iconCache;

    SortOption currentSortOption;
    bool sortAscending;
    bool firstDraw = true;
    bool isLoading = false;
    bool isCreatingNewItem;
    bool isNewItemFolder;
    std::string newItemName;

    void DrawMainLayout();
    void DrawContentArea();
    void DrawDirectoryTree();
    void DrawBreadcrumbs();
    void DrawFolderContents(const std::filesystem::path& path);
    void DrawActionButtons();
    void DrawGridItem(const std::filesystem::path& entry, const std::string& filename);
    void DrawRenamingItem(const std::filesystem::path& entry, const std::string& filename, Image* icon);
    void DrawNormalItem(const std::filesystem::path& entry, const std::string& filename, Image* icon, bool isDirectory);
    void DrawNewItemCreation();
    void DrawTruncatedLabel(const std::string& filename);

    void HandleItemInteractions(const std::filesystem::path& entry, const std::string& filename, Image* icon, bool isDirectory);
    void HandleFileSelection(const std::filesystem::path& filePath);
    void HandleRename(const std::filesystem::path& entry, const char* newName);

    void StartDirectoryListing(const std::filesystem::path& path);
    void SortDirectoryEntries();
    void ShowContextMenu();
    void CreateNewItem(const std::filesystem::path& path, bool isFolder);
    Image* GetIconForFile(const std::filesystem::path& filePath);
};